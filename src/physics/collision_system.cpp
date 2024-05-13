// Copyright © 2024 Jacob Curlin

#include "physics/collision_system.h"

#include "asset/model.h"
#include "asset/mesh.h"

#include "core/components/collider.h"
#include "core/components/render.h"
#include "core/components/transform.h"
#include "core/components/rigid_body.h"

namespace cgx::physics
{

CollisionSystem::CollisionSystem(ecs::ECSManager* ecs_manager): System{ecs_manager} {}

CollisionSystem::~CollisionSystem() = default;

void CollisionSystem::frame_update(float dt) {}

void CollisionSystem::fixed_update(float dt)
{
    for (auto& e1 : m_entities) {
        auto& t1 = get_component<component::Transform>(e1);
        auto& c1 = get_component<component::Collider>(e1);

        for (auto& e2 : m_entities) {
            if (e1 == e2) {
                continue;
            }
            auto& t2 = get_component<component::Transform>(e2);
            auto& c2 = get_component<component::Collider>(e2);

            if (check_collision(t1, c1, t2, c2)) {
                CGX_TRACE("Collision Detected: Entities {} & {}", e1, e2);
                resolve_collision(e1, e2);
            }
        }
    }
}

void CollisionSystem::on_entity_added(ecs::Entity entity)
{
    auto& cc = get_component<component::Collider>(entity);
    if (m_ecs_manager->has_component<component::Render>(entity)) {
        if (cc.type == component::Collider::Type::AABB) {
            auto& rc = get_component<component::Render>(entity);

            glm::vec3 min_bounds = glm::vec3(std::numeric_limits<float>::max());
            glm::vec3 max_bounds = glm::vec3(std::numeric_limits<float>::lowest());

            auto& meshes = rc.model->get_meshes();
            for (const auto& mesh : meshes) {
                min_bounds = glm::min(min_bounds, mesh->get_min_bounds());
                max_bounds = glm::max(max_bounds, mesh->get_max_bounds());
            }

            cc.size = max_bounds - min_bounds;
        }
    }
}

void CollisionSystem::on_entity_removed(ecs::Entity entity) {}

bool CollisionSystem::check_collision(
    const component::Transform& t1,
    const component::Collider&  c1,
    const component::Transform& t2,
    const component::Collider&  c2)
{
    glm::vec3 min1 = t1.translation - c1.size / 2.0f;
    glm::vec3 max1 = t1.translation + c1.size / 2.0f;
    glm::vec3 min2 = t2.translation - c2.size / 2.0f;
    glm::vec3 max2 = t2.translation + c2.size / 2.0f;

    return (min1.x <= max2.x && max1.x >= min2.x) && (min1.y <= max2.y && max1.y >= min2.y) && (
               min1.z <= max2.z && max1.z >= min2.z);
}

void CollisionSystem::resolve_collision(const ecs::Entity e1, const ecs::Entity e2)
{
    auto& t1 = get_component<component::Transform>(e1);
    auto& t2 = get_component<component::Transform>(e2);

    auto& c1 = get_component<component::Collider>(e1);
    auto& c2 = get_component<component::Collider>(e2);

    bool is_static1 = !m_ecs_manager->has_component<component::RigidBody>(e1);
    bool is_static2 = !m_ecs_manager->has_component<component::RigidBody>(e2);

    if (is_static1 && is_static2) {
        return;
    }

    glm::vec3 delta = t2.translation - t1.translation;
    glm::vec3 overlap(0.0f);
    for (int i = 0 ; i < 3 ; ++i) {
        float penentration = (c1.size[i] + c2.size[i]) * 0.5f - std::abs(delta[i]);
        if (penentration < 0.0f) {
            return;
        }
        overlap[i] = penentration;
    }

    float penetration = std::min(overlap.x, std::min(overlap.y, overlap.z));
    int   axis        = (overlap.x < overlap.y) ? ((overlap.x < overlap.z) ? 0 : 2) : ((overlap.y < overlap.z) ? 1 : 2);

    glm::vec3 collision_normal(0.0f);
    collision_normal[axis] = (delta[axis] < 0.0f) ? -1.0f : 1.0f;

    if (!is_static1 && !is_static2) {
        auto& r1 = get_component<component::RigidBody>(e1);
        auto& r2 = get_component<component::RigidBody>(e2);

        const glm::vec3 relative_velocity = r1.velocity - r2.velocity;
        const float     normal_velocity   = glm::dot(relative_velocity, collision_normal);

        if (normal_velocity < 0.0f) {
            float restitution       = 0.5f;
            float impulse_magnitude = -(1.0f + restitution) * normal_velocity;
            impulse_magnitude /= (1.0f / r1.mass) + (1.0f / r2.mass);

            glm::vec3 impulse = impulse_magnitude * collision_normal;
            r1.velocity -= impulse / r1.mass;
            r2.velocity += impulse / r2.mass;
        }
    }

    if (is_static1 || is_static2) {
        if (!is_static1) {
            auto& r1 = get_component<component::RigidBody>(e1);
            t1.translation -= collision_normal * penetration;
            r1.velocity = glm::reflect(r1.velocity, collision_normal) * 0.5f;
            t1.dirty = true;
        }
        else if (!is_static2) {
            auto& r2 = get_component<component::RigidBody>(e2);
            t2.translation += collision_normal * penetration;
            r2.velocity = glm::reflect(r2.velocity, -collision_normal) * 0.5f;
            t2.dirty = true;
        }
    }
    else {
        auto& r1 = m_ecs_manager->get_component<component::RigidBody>(e1);
        auto& r2 = m_ecs_manager->get_component<component::RigidBody>(e2);
        const float total_mass = r1.mass + r2.mass;
        const float r1_factor  = r2.mass / total_mass;
        const float r2_factor  = r1.mass / total_mass;

        t1.translation += collision_normal * penetration * r1_factor;
        t1.dirty = true;

        t2.translation += collision_normal * penetration * r2_factor;
        t2.dirty = true;
    }
}
}
