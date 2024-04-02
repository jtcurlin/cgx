// Copyright © 2024 Jacob Curlin

#include "ecs/system_manager.h"

namespace cgx::ecs
{
    SystemManager::SystemManager(std::shared_ptr<cgx::ecs::ComponentManager> component_registry)
        : m_component_registry(component_registry) {}

    void SystemManager::EntityDestroyed(Entity entity)
    {
        CGX_TRACE("SystemManager :: EntityDestroyed Called.");
        for (auto const& pair : m_systems)
        {
            auto const& system = pair.second;

            system->m_entities.erase(entity);
        }
    }

    void SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        for (auto const& pair : m_systems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& system_signature = m_signatures[type];

            if ((entitySignature & system_signature) == system_signature)
            {
                system->m_entities.insert(entity);
            }

            else
            {
                system->m_entities.erase(entity);   
            }
        }
    }
}