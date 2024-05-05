// Copyright © 2024 Jacob Curlin

#include "ecs/system_registry.h"
#include "ecs/system.h"
#include "ecs/ecs_manager.h"

namespace cgx::ecs
{
SystemRegistry::SystemRegistry(ECSManager* ecs_manager)
    : m_ecs_manager(ecs_manager) {}

SystemRegistry::~SystemRegistry() = default;

void SystemRegistry::frame_update(const float dt)
{
    for (auto& [type_id, system] : m_systems) {
        system->frame_update(dt);
    }
}

void SystemRegistry::fixed_update(const float fixed_dt)
{
    for (auto& [type_id, system] : m_systems) {
        system->fixed_update(fixed_dt);
    }
}

void SystemRegistry::on_entity_released(const Entity entity) const
{
    CGX_TRACE("SystemManager :: EntityDestroyed Called.");
    for (auto const& pair : m_systems) {
        auto const& system = pair.second;

        system->on_entity_removed(entity);
        system->m_entities.erase(entity);
    }
}

void SystemRegistry::on_entity_updated(const Entity entity, const Signature entitySignature)
{
    for (auto const& pair : m_systems) {
        auto const& type             = pair.first;
        auto const& system           = pair.second;
        auto const& system_signature = m_signatures[type];

        if ((entitySignature & system_signature) == system_signature) {
            if (system->m_entities.insert(entity).second) {
                system->on_entity_added(entity);
            }
        }

        else {
            if (system->m_entities.erase(entity) > 0) {
                system->on_entity_removed(entity);
            }
        }
    }
}
}
