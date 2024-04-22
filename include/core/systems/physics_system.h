// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"

namespace cgx::ecs
{
class ComponentRegistry;
}

namespace cgx::ecs
{
class EventHandler;
}

namespace cgx::core
{
class PhysicsSystem : public ecs::System
{
public:
    explicit PhysicsSystem(ecs::ECSManager* ecs_manager);
    ~PhysicsSystem() override;

    void on_entity_added(ecs::Entity entity) override {}
    void on_entity_removed(ecs::Entity entity) override {}

    void update(float dt) override;
};
}