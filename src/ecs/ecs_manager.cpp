// Copyright © 2024 Jacob Curlin

#include "ecs/ecs_manager.h"

namespace cgx::ecs
{
    ECSManager::ECSManager()
    {
        m_entity_registry = std::make_unique<EntityRegistry>();
        m_component_registry = std::make_unique<ComponentRegistry>();
        m_system_registry = std::make_unique<SystemRegistry>(this);
    }

    ECSManager::~ECSManager() = default;

    void ECSManager::update(const float dt) const
    {
        m_system_registry->update(dt);
    }
}