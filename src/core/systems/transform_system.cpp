// Copyright © 2024 Jacob Curlin

#include "core/systems/transform_system.h"
#include "ecs/ecs_manager.h"
#include "core/components/hierarchy.h"
#include "core/systems/hierarchy_system.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cgx::core
{

TransformSystem::TransformSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager) {}

TransformSystem::~TransformSystem() = default;

void TransformSystem::initialize(HierarchySystem* hierarchy_system)
{
    m_hierarchy_system = hierarchy_system;
}

void TransformSystem::frame_update(float dt)
{
    // do nothing
}

void TransformSystem::fixed_update(float dt)
{
    for (const auto& entity : m_hierarchy_system->get_order()) {
        if (m_entities.find(entity) == m_entities.end()) {
            continue; // continue if entity lacks transform component
        }
        auto& transform = m_ecs_manager->get_component<component::Transform>(entity);
        if (transform.dirty) {

            ecs::Entity parent = ecs::MAX_ENTITIES;
            if (m_ecs_manager->has_component<component::Hierarchy>(entity)) {
                mark_children_dirty(entity);
                parent = m_ecs_manager->get_component<component::Hierarchy>(entity).parent;
            }

            if (parent != ecs::MAX_ENTITIES) {
                const glm::mat4& parent_matrix = m_ecs_manager->get_component<component::Transform>(parent).
                        world_matrix;
                update_world_matrix(transform, parent_matrix);
            }
            else {
                update_world_matrix(transform, glm::mat4(1.0f));
            }

            transform.dirty = false;
        }
    }
}

void TransformSystem::on_entity_added(const ecs::Entity entity) {}

void TransformSystem::on_entity_removed(const ecs::Entity entity) {}

void TransformSystem::mark_children_dirty(const ecs::Entity parent)
{
    auto& parent_hierarchy = m_ecs_manager->get_component<component::Hierarchy>(parent);
    for (const auto& child : parent_hierarchy.children) {
        if (m_ecs_manager->has_component<component::Transform>(child)) {
            auto& child_transform = m_ecs_manager->get_component<component::Transform>(child);
            child_transform.dirty = true;
        }
        mark_children_dirty(child);
    }
}

void TransformSystem::update_world_matrix(component::Transform& transform, const glm::mat4& parent_matrix)
{
    auto local_matrix = glm::mat4(1.0f);
    local_matrix      = glm::scale(local_matrix, transform.scale);
    local_matrix      = glm::translate(local_matrix, transform.translation);
    local_matrix      = glm::rotate(local_matrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    local_matrix      = glm::rotate(local_matrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    local_matrix      = glm::rotate(local_matrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    transform.world_matrix = parent_matrix * local_matrix;
}
}
