// Copyright © 2024 Jacob Curlin

#include "core/transform_system.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cgx::core
{

TransformSystem::TransformSystem(const std::shared_ptr<ecs::ComponentRegistry>& component_registry)
    : System(component_registry)
{}
TransformSystem::~TransformSystem() = default;

void TransformSystem::update(float dt)
{
    for (const auto& entity : m_update_order) {
        auto& transform = m_component_registry->get_component<component::Transform>(entity);
        if (transform.dirty) {
            update_world_matrix(transform);
            transform.dirty = false;
        }
    }
}

void TransformSystem::on_entity_added(const ecs::Entity entity)
{
    m_update_order.push_back(entity);
    sort_by_depth();
}

void TransformSystem::on_entity_removed(const ecs::Entity entity)
{
    const auto new_end = std::remove(m_update_order.begin(), m_update_order.end(), entity);
    m_update_order.erase(new_end, m_update_order.end());
    sort_by_depth();
}

void TransformSystem::on_component_updated(const ecs::Entity entity)
{
    auto& transform = m_component_registry->get_component<component::Transform>(entity);
    transform.dirty = true;
    for (auto& child : transform.children) {
        on_component_updated(child);
    }
}

void TransformSystem::update_world_matrix(component::Transform& transform)
{
    auto parent_matrix = glm::mat4(1.0f);
    if (transform.parent != ecs::MAX_ENTITIES) {
        auto& parent_transform = m_component_registry->get_component<component::Transform>(transform.parent);
        parent_matrix          = parent_transform.world_matrix;
    }

    auto local_matrix = glm::mat4(1.0f);
    local_matrix = glm::scale(local_matrix, transform.local_scale);
    local_matrix = glm::rotate(local_matrix, glm::radians(transform.local_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(transform.local_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(transform.local_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    local_matrix = glm::translate(local_matrix, transform.local_position);

    transform.world_matrix = parent_matrix * local_matrix;
}

void TransformSystem::sort_by_depth()
{
    m_update_order.clear();

    for (auto& entity : m_entities) {
        m_update_order.push_back(entity);
    }

    std::sort(
        m_update_order.begin(),
        m_update_order.end(),
        [this](size_t a, size_t b) {
            return get_depth(a) < get_depth(b);
        });
}

size_t TransformSystem::get_depth(const ecs::Entity entity)
{
    size_t depth     = 0;
    auto&  transform = m_component_registry->get_component<component::Transform>(entity);
    while (transform.parent != ecs::MAX_ENTITIES) {
        depth++;
        transform = m_component_registry->get_component<component::Transform>(transform.parent);
    }
    return depth;
}
}
