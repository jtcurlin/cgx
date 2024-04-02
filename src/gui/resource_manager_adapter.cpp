// Copyright © 2024 Jacob Curlin

#include "gui/resource_manager_adapter.h"
#include "event/events/engine_events.h"

namespace cgx::gui
{
    ResourceManagerAdapter::ResourceManagerAdapter(std::shared_ptr<cgx::event::EventHandler> event_handler)
        : m_event_handler(event_handler)
    {
        RegisterEventListeners();
    }

    const std::vector<cgx::resource::ResourceEntry>& ResourceManagerAdapter::GetResourceEntries(cgx::resource::ResourceType type) const
    {
        return m_resources.at(type);
    }

    void ResourceManagerAdapter::RegisterEventListeners()
    {
        m_event_handler->AddListener(cgx::events::resource::RESOURCE_REGISTERED, [this](cgx::event::Event& event) 
        { 
            cgx::resource::ResourceEntry entry {
                event.GetParam<cgx::resource::RUID>(cgx::events::resource::RESOURCE_UID),
                event.GetParam<std::string>(cgx::events::resource::RESOURCE_TAG)
            };
            m_resources[event.GetParam<cgx::resource::ResourceType>(cgx::events::resource::RESOURCE_TYPE)].push_back(entry);
        });

        m_event_handler->AddListener(cgx::events::resource::RESOURCE_UNREGISTERED, [this](cgx::event::Event& event)
        {
            cgx::resource::RUID ruid = event.GetParam<cgx::resource::RUID>(cgx::events::resource::RESOURCE_UID);
            auto& resource_entries = m_resources[event.GetParam<cgx::resource::ResourceType>(cgx::events::resource::RESOURCE_TYPE)];

            resource_entries.erase(std::remove_if(resource_entries.begin(), resource_entries.end(),
                                                  [ruid](const cgx::resource::ResourceEntry& entry) {
                                                    return entry.ruid == ruid;
                                                  }), resource_entries.end());
        });
    }

} // namespace cgx::gui