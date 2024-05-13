// Copyright © 2024 Jacob Curlin

#include "physics/physics_system.h"
#include "core/components/transform.h"
#include "core/components/rigid_body.h"

namespace cgx::physics
{
PhysicsSystem::PhysicsSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager)
{}

PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::frame_update(const float dt)
{
    // do nothing
}

void PhysicsSystem::fixed_update(const float dt)
{
    for (auto const& entity : m_entities) {
        auto& rigid_body = get_component<component::RigidBody>(entity);
        auto& transform  = get_component<component::Transform>(entity);

        transform.translation += rigid_body.velocity * dt;
        rigid_body.velocity += rigid_body.acceleration * dt;

        transform.rotation += rigid_body.angular_velocity * dt;
        transform.scale += rigid_body.scale_rate * dt;
        transform.dirty = true;
    }
}
}
