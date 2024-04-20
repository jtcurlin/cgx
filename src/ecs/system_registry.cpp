// Copyright © 2024 Jacob Curlin

#include "ecs/system_registry.h"

namespace cgx::ecs
{
SystemRegistry::SystemRegistry(const std::shared_ptr<ComponentRegistry>& component_registry)
    : m_component_registry(component_registry) {}

SystemRegistry::~SystemRegistry() = default;

void SystemRegistry::entity_destroyed(const Entity entity) const
{
    CGX_TRACE("SystemManager :: EntityDestroyed Called.");
    for (auto const& pair : m_systems) {
        auto const& system = pair.second;

        system->m_entities.erase(entity);
        system->on_entity_removed(entity);
    }
}

void SystemRegistry::entity_signature_changed(const Entity entity, const Signature entitySignature)
{
    for (auto const& pair : m_systems) {
        auto const& type             = pair.first;
        auto const& system           = pair.second;
        auto const& system_signature = m_signatures[type];

        if ((entitySignature & system_signature) == system_signature) {
            system->m_entities.insert(entity);
            system->on_entity_added(entity);
        }

        else {
            system->m_entities.erase(entity);
            system->on_entity_removed(entity);
        }
    }
}
}
