// Copyright © 2024 Jacob Curlin

#include "core/physics_system.h"

#include "ecs/components/transform.h"
#include "ecs/components/rigid_body.h"

namespace cgx::core
{
PhysicsSystem::PhysicsSystem(const std::shared_ptr<ecs::ComponentRegistry> &component_registry)
    : System(component_registry) {}

PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::update(const float dt)
{
    for (auto const &entity: m_entities) {
        auto &rigid_body = GetComponent<component::RigidBody>(entity);
        auto &transform = GetComponent<component::Transform>(entity);

        transform.local_position += rigid_body.velocity * dt;
        rigid_body.velocity += rigid_body.acceleration * dt;
    }
}
}
