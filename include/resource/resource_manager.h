// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"
#include "resource/import/resource_importer.h"

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace cgx::resource
{
    class ResourceManager
    {
    public:

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        static ResourceManager& getSingleton()
        {
            static ResourceManager s_instance;
            return s_instance;
        }

        template<typename ResourceType>
        void registerImporter(const std::shared_ptr<ResourceImporter>& importer)
        {
            std::lock_guard<std::recursive_mutex> lock(m_importers_mutex);
            m_importers[std::type_index(typeid(ResourceType))] = importer;
        }

        template<typename ResourceType>
        void deregisterImporter()
        {
            std::lock_guard<std::recursive_mutex> lock(m_importers_mutex);
            m_importers.erase(std::type_index(typeid(ResourceType)));
        }
        
        template<typename ResourceType>
        std::shared_ptr<ResourceType> importResource(const std::string& path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            auto resource_it = m_derived_path_to_ruid.find(path);
            if (resource_it != m_derived_path_to_ruid.end())
            {
                return std::static_pointer_cast<ResourceType>(m_resources[resource_it->second]);
            }
            else
            {
                std::lock_guard<std::recursive_mutex> importer_lock(m_importers_mutex);

                auto importer_it = m_importers.find(std::type_index(typeid(ResourceType)));
                if (importer_it != m_importers.end())
                {
                    importer_it->second->Initialize(path);
                    RUID ruid = importer_it->second->Import();
                    return getResource<ResourceType>(ruid);
                }
            }
            CGX_WARN("Failed to import resource from specified path {}", path);
            return nullptr;
        }

        template <typename ResourceType>
        std::shared_ptr<ResourceType> loadResource(const std::shared_ptr<ResourceType>& resource)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);

            ResourceMetadata metadata = resource->getMetadata();

            m_resources[metadata.ruid] = resource;

            if (!(metadata.name.empty()))
            {
                m_name_to_ruid[metadata.name].push_back(metadata.ruid);
            }
            if (!(metadata.source_path.empty()))
            {
                m_source_path_to_ruid[metadata.source_path].push_back(metadata.ruid);
            }
            if (!(metadata.derived_path.empty()))
            {
                m_derived_path_to_ruid[metadata.derived_path] = metadata.ruid;
            }
            
            return std::static_pointer_cast<ResourceType>(m_resources[metadata.ruid]);
        }
        
        void removeResource(size_t ruid)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            
            auto it = m_resources.find(ruid);
            if (it != m_resources.end())
            {
                auto resource = it->second;
                auto metadata = resource->getMetadata();

                if (!metadata.name.empty())
                {
                    auto& names = m_name_to_ruid[metadata.name];
                    names.erase(std::remove(names.begin(), names.end(), ruid), names.end());
                    if (names.empty())
                    {
                        m_name_to_ruid.erase(metadata.name);
                    }
                }

                if (!metadata.source_path.empty())
                {
                    auto& sources = m_source_path_to_ruid[metadata.source_path];
                    sources.erase(std::remove(sources.begin(), sources.end(), ruid), sources.end());
                    if (sources.empty())
                    {
                        m_source_path_to_ruid.erase(metadata.source_path);
                    }
                }

                if (!metadata.derived_path.empty())
                {
                    m_derived_path_to_ruid.erase(metadata.derived_path);
                }

                m_resources.erase(it);
            }
            else
            {
                CGX_ERROR("Failed to remove resource: RUID {} not found.", ruid);
            }
        }

        template <typename ResourceType>
        std::shared_ptr<ResourceType> getResource(RUID ruid)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            auto it = m_resources.find(ruid);
            if (it != m_resources.end())
            {
                return std::static_pointer_cast<ResourceType>(it->second);
            }
            else
            {
                return nullptr;
            }
        }

        // get all resources with the specified name (non-templated variant)
        std::vector<RUID> getRUIDByName(const std::string& name)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            auto resource_it = m_name_to_ruid.find(name);
            if (resource_it != m_name_to_ruid.end())
            {
                ruid_list = resource_it->second;
            }
            return ruid_list;
        }

        // get all resources of a particular type with the specified name (templated variant)
        template <typename ResourceType>
        std::vector<RUID> getRUIDByName(const std::string& name)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            auto resource_it = m_name_to_ruid.find(name);
            if (resource_it != m_name_to_ruid.end())
            {
                for (auto ruid : resource_it->second)
                {
                    auto casted_resource = std::dynamic_pointer_cast<ResourceType>(m_resources[ruid]);
                    if (casted_resource)
                    {
                        ruid_list.push_back(casted_resource->getRUID());
                    }
                }
            }
            return ruid_list;
        }

        // get all resources with the specified source path (non-templated variant)
        std::vector<RUID> getRUIDbySourcePath(const std::string& path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            auto resource_it = m_source_path_to_ruid.find(path);
            if (resource_it != m_source_path_to_ruid.end())
            {
                ruid_list = resource_it->second;
            }
            return ruid_list;
        }

        // get all resource uid's of a particular type with the specified source path (templated variant)
        template<typename ResourceType>
        std::vector<RUID> getRUIDbySourcePath(const std::string& path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            auto resource_it = m_source_path_to_ruid.find(path);
            if (resource_it != m_source_path_to_ruid.end())
            {
                for (auto ruid : resource_it->second)
                {
                    auto resource = std::dynamic_pointer_cast<ResourceType>(m_resources[ruid]);
                    if (resource)
                    {
                        ruid_list.push_back(resource->getRUID());
                    }
                }
            }
            return ruid_list;
        }

        // get all resource uid's (non-templated variant)
        std::vector<RUID> getAllRUIDs()
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            for (auto& [ruid, resource] : m_resources)
            {
                ruid_list.push_back(ruid);
            }
            return ruid_list;
        }
 
        // get all resource uid's of a particular type (templated variant)
        template<typename ResourceType>
        std::vector<RUID> getAllRUIDs()
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            std::vector<RUID> ruid_list;

            for (auto& [ruid, resource] : m_resources)
            {
                auto casted_resource = std::dynamic_pointer_cast<ResourceType>(resource);
                if (casted_resource)
                {
                    ruid_list.push_back(ruid);
                }
            }
            return ruid_list;
        }

        ResourceMetadata getResourceMetadata(RUID ruid)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resource_mutex);
            ResourceMetadata metadata;

            auto resource_it = m_resources.find(ruid);
            if (resource_it != m_resources.end())
            {
                return resource_it->second->getMetadata();
            }
            else
            {
                CGX_ERROR("ResourceManager::getResourceMetadata() : RUID {} not found", ruid);
                return ResourceMetadata();
            }
        }

    private:
        ResourceManager() {}

        std::unordered_map<RUID, std::shared_ptr<Resource>> m_resources;
        std::recursive_mutex m_resource_mutex;

        std::unordered_map<std::type_index, std::shared_ptr<ResourceImporter>> m_importers;
        std::recursive_mutex m_importers_mutex;

        std::unordered_map<std::string, std::vector<RUID>> m_name_to_ruid;
        std::unordered_map<std::string, std::vector<RUID>> m_source_path_to_ruid;;
        std::unordered_map<std::string, RUID> m_derived_path_to_ruid;

    }; // class ResourceManager

}; // namespace cgx::resource