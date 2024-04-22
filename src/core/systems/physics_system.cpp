// Copyright © 2024 Jacob Curlin

#include "core/systems/physics_system.h"

#include "core/components/transform.h"
#include "core/components/rigid_body.h"

namespace cgx::core
{
PhysicsSystem::PhysicsSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager)
{
}

PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::update(const float dt)
{
    for (auto const& entity : m_entities) {
        auto& rigid_body = get_component<component::RigidBody>(entity);
        auto& transform  = get_component<component::Transform>(entity);

        transform.local_position += rigid_body.velocity * dt;
        rigid_body.velocity += rigid_body.acceleration * dt;

        transform.local_rotation += rigid_body.angular_velocity * dt;
        transform.local_scale += rigid_body.scale_rate * dt;

        transform.dirty = true;
    }
}
}
