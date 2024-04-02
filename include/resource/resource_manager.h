// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"
#include "resource/import/resource_importer.h"

#include "event/event_handler.h"
#include "event/events/engine_events.h"

#include <iomanip>
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

        void setEventHandler(std::shared_ptr<cgx::event::EventHandler> event_handler)
        {
            m_event_handler = event_handler;
        }

        template<typename ResourceType>
        void RegisterImporter(const std::shared_ptr<ResourceImporter>& importer)
        {
            std::lock_guard<std::recursive_mutex> lock(m_importers_mutex);
            m_importers[std::type_index(typeid(ResourceType))] = importer;
        }

        template<typename ResourceType>
        void DeregisterImporter()
        {
            std::lock_guard<std::recursive_mutex> lock(m_importers_mutex);
            m_importers.erase(std::type_index(typeid(ResourceType)));
        }
        
        template<typename ResourceType>
        RUID ImportResource(const std::string& path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            auto resource_it = m_path_to_id.find(path);
            if (resource_it != m_path_to_id.end())
            {
                return resource_it->second;
            }
            else
            {
                std::lock_guard<std::recursive_mutex> importer_lock(m_importers_mutex);

                auto importer_it = m_importers.find(std::type_index(typeid(ResourceType)));
                if (importer_it != m_importers.end())
                {
                    if (importer_it->second->Initialize(path))
                    {
                        return importer_it->second->Import();
                    }
                    else
                    {
                        CGX_ERROR("ResourceManager: Failed to import resource at specified path [{}]. " \
                                  "(Importer initialization was unsuccessful)", path);
                        return 0;
                    }
                }
                else
                {
                    CGX_ERROR("ResourceManager: Failed to import resource at specified path [{}]. " \
                              "(No importer registered for resource type)", path);
                    return 0;
                }
            }
        }

        template <typename ResourceType>
        RUID RegisterResource(const std::shared_ptr<ResourceType>& resource, bool permit_duplicate_path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            if (resource == nullptr)
            {
                CGX_ERROR("ResourceManager: failed to register resource. (null resource)");
                return k_invalid_id;
            }
            if (resource->getPath().empty())
            {
                CGX_ERROR("ResourceManager:: failed to register resource. (no path assigned)");
                return k_invalid_id;
            }

            auto original_path = resource->getPath();
            auto search_result = m_path_to_id.find(original_path);

            if (search_result != m_path_to_id.end() && !permit_duplicate_path)
            {
                CGX_ERROR("ResourceManager: failed to register resource. (path "  \
                          "[{}] already registered, duplicates not permitted)", original_path);
                return k_invalid_id;
            }

            std::string new_path = original_path;
            if (search_result != m_path_to_id.end())
            {
                bool path_found = false;
                for (int i=1; i<=999; ++i)
                {
                    std::ostringstream oss;
                    oss << original_path << "_" << std::setw(3) << std::setfill('0') << i;
                    new_path = oss.str();

                    if (m_path_to_id.find(new_path) == m_path_to_id.end())
                    {
                        CGX_INFO("ResourceManager:: path {} already registered. assigning resource " \
                                 "to path {}.", original_path, new_path);
                        path_found = true;
                        break;
                    }
                }
                if (!path_found)
                {
                    CGX_ERROR("ResourceManager:: failed to register resource (reached maximum " \
                              "duplicate paths while attempting to register path {}", original_path);
                    return k_invalid_id;
                }
            }

            RUID id = GenerateID();
            resource->setID(id);
            resource->setPath(new_path);

            m_path_to_id[resource->getPath()] = resource->getID();
            m_resources[resource->getID()] = resource;

            if (resource->hasTag())
            {
                m_tag_to_id[resource->getTag()].push_back(resource->getID());
            }

            cgx::event::Event event(cgx::events::resource::RESOURCE_REGISTERED);
            event.SetParam(cgx::events::resource::RESOURCE_UID, resource->getID());
            event.SetParam(cgx::events::resource::RESOURCE_PATH, resource->getPath());
            event.SetParam(cgx::events::resource::RESOURCE_TAG, resource->getTag());
            event.SetParam(cgx::events::resource::RESOURCE_TYPE, resource->getType());
            m_event_handler->SendEvent(event);

            return id;
        }
        
        bool DeregisterResource(RUID id)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            auto it = m_resources.find(id);

            if (it != m_resources.end())
            {
                // remove resource's tag->id mapping (if present)
                auto& resource = it->second;
                if (resource->hasTag())
                {
                    
                    auto& tags = m_tag_to_id[resource->getTag()];
                    if (!tags.empty())
                    {
                        tags.erase(std::remove(tags.begin(), tags.end(), resource->getID()), tags.end());
                    }
                }

                // remove resource's path->id mapping
                m_path_to_id.erase(resource->getPath());

                id = resource->getID();

                m_resources.erase(id); // remove resource (remove id->resource mapping)
                resource->setID(k_invalid_id); // set resource's id to invalid_id 

                cgx::event::Event event(cgx::events::resource::RESOURCE_UNREGISTERED);
                event.SetParam(cgx::events::resource::RESOURCE_UID, id);
                m_event_handler->SendEvent(event);

                return true;
            }
            else
            {
                CGX_WARN("ResourceManager: failed to remove resource. (resource ID [{}] not registered)", id);
                return false;
            }
        }

        template <typename ResourceType>
        std::shared_ptr<ResourceType> getResource(RUID id)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            auto resource_it = m_resources.find(id);
            if (resource_it != m_resources.end())
            {
                return std::static_pointer_cast<ResourceType>(resource_it->second);
            }

            CGX_ERROR("ResourceManager: failed to get resource. (resource ID [{}] not registered)", id);
            return nullptr;
        }

        // get all resources with the specified name (non-templated variant)
        std::vector<RUID> getIDByTag(const std::string& tag)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            auto resource_it = m_tag_to_id.find(tag);
            if (resource_it != m_tag_to_id.end())
            {
                return resource_it->second;
            }

            CGX_WARN("ResourceManager: failed to get ID(s) for tag. (tag {} not registered)", tag);
            return {};
        }

        // get all resources of a particular type with the specified name (templated variant)
        template <typename ResourceType>
        std::vector<RUID> getResourceIDByTag(const std::string& tag)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            std::vector<RUID> id_list;

            auto resource_it = m_tag_to_id.find(tag);
            if (resource_it != m_tag_to_id.end())
            {
                for (auto id : resource_it->second)
                {
                    auto casted_resource = std::dynamic_pointer_cast<ResourceType>(m_resources[id]);
                    if (casted_resource)
                    {
                        id_list.push_back(casted_resource->getID());
                    }
                }
                return id_list;
            }
            
            CGX_WARN("ResourceManager:: failed to get ID(s) for tag. (tag [{}] not registered)", tag);
            return {};
        }

        // get all resources with the specified path (non-templated variant)
        RUID getIDbyPath(const std::string& path)
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            auto resource_it = m_path_to_id.find(path);
            if (resource_it != m_path_to_id.end())
            {
                return resource_it->second;
            }

            CGX_WARN("ResourceManager: failed to get ID(s) for specified path. (path [{}] not registered)", path);
            return k_invalid_id;
        }

        // get all resource uid's (non-templated variant)
        std::vector<RUID> getAllIDs()
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            std::vector<RUID> id_list;
            for (auto& [id, resource] : m_resources)
            {
                id_list.push_back(id);
            }
            return id_list;
        }
 
        // get all resource uid's of a particular type (templated variant)
        template<typename ResourceType>
        std::vector<RUID> getAllIDs()
        {
            std::lock_guard<std::recursive_mutex> lock(m_resources_mutex);

            std::vector<RUID> id_list;
            for (auto& [id, resource] : m_resources)
            {
                auto casted_resource = std::dynamic_pointer_cast<ResourceType>(resource);
                if (casted_resource)
                {
                    id_list.push_back(id);
                }
            }
            return id_list;
        }


    private:
        ResourceManager() {}

        static RUID GenerateID() { 
            static RUID s_next_id = 1;
            static std::mutex s_id_mutex;
            std::lock_guard<std::mutex> lock(s_id_mutex);
            return s_next_id++;
        }

        std::shared_ptr<cgx::event::EventHandler> m_event_handler;

        std::unordered_map<RUID, std::shared_ptr<Resource>> m_resources;
        std::recursive_mutex m_resources_mutex;

        std::unordered_map<std::type_index, std::shared_ptr<ResourceImporter>> m_importers;
        std::recursive_mutex m_importers_mutex;

        std::unordered_map<std::string, RUID> m_path_to_id;
        std::unordered_map<std::string, std::vector<RUID>> m_tag_to_id;

    }; // class ResourceManager

}; // namespace cgx::resource