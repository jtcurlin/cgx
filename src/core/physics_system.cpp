// Copyright © 2024 Jacob Curlin

#include "core/physics_system.h"

#include "ecs/component_manager.h"
#include "ecs/components/transform.h"
#include "ecs/components/rigid_body.h"

namespace cgx::core
{
    PhysicsSystem::PhysicsSystem(std::shared_ptr<cgx::ecs::ComponentManager> component_registry)
        : System(component_registry) {}

    void PhysicsSystem::Update(float dt)
    {
        for (auto const& entity : m_entities)
        {
            auto& rigid_body = GetComponent<cgx::component::RigidBody>(entity);
            auto& transform = GetComponent<cgx::component::Transform>(entity);

            auto old_position = transform.local_position;

            transform.local_position += rigid_body.velocity * dt;
            rigid_body.velocity += rigid_body.acceleration * dt;
        }
    }

}