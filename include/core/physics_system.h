// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"

namespace cgx::ecs
{
class ComponentRegistry;
}

namespace cgx::event
{
class EventHandler;
}

namespace cgx::core
{
class PhysicsSystem : public ecs::System
{
public:
    explicit PhysicsSystem(
        const std::shared_ptr<ecs::ComponentRegistry> &component_registry
    );
    ~PhysicsSystem() override;

    void on_entity_added(ecs::Entity entity) override {}
    void on_entity_removed(ecs::Entity entity) override {}
    void on_component_updated(ecs::Entity entity) override {}

    void update(float dt) override;
};
}
