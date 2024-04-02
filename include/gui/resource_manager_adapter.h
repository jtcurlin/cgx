// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"
#include "resource/resource_manager.h"
#include "event/event_handler.h"

#include <string>
#include <unordered_map>


namespace cgx::resource
{
    struct ResourceEntry
    {
        cgx::resource::RUID ruid;
        std::string tag;
    };
}

namespace cgx::gui
{

    class ResourceManagerAdapter
    {
    public:
        ResourceManagerAdapter(std::shared_ptr<cgx::event::EventHandler> event_handler);
        ~ResourceManagerAdapter() = default;
        
        const std::vector<cgx::resource::ResourceEntry>& GetResourceEntries(cgx::resource::ResourceType type) const;

    private:
        std::shared_ptr<cgx::event::EventHandler> m_event_handler;
        std::unordered_map<cgx::resource::ResourceType, std::vector<cgx::resource::ResourceEntry>> m_resources;

        void RegisterEventListeners();

    }; // class ResourceManagerAdapter

} // namespace cgx::gui