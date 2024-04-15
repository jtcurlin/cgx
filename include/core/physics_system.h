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

    void update(float dt) override;
};
}
