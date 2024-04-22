// Copyright © 2024 Jacob Curlin

#include "core/systems/hierarchy_system.h"
#include "core/components/Transform.h"

#include "ecs/event_handler.h"
#include "core/events/engine_events.h"

namespace cgx::core
{
HierarchySystem::HierarchySystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager)
{
    ecs::EventHandler::get_instance().add_listener(
        events::hierarchy::PARENT_UPDATE,
        [this](ecs::Event& event) {
            this->on_parent_update(
                event.get_param<ecs::Entity>(events::hierarchy::CHILD),
                event.get_param<ecs::Entity>(events::hierarchy::OLD_PARENT),
                event.get_param<ecs::Entity>(events::hierarchy::NEW_PARENT));
        });
}

HierarchySystem::~HierarchySystem() = default;

void HierarchySystem::update(float dt) {} // do nothing

void HierarchySystem::on_entity_added(const ecs::Entity entity)
{
    m_order.push_back(entity);
    sort_order_by_depth();
}

void HierarchySystem::on_entity_removed(const ecs::Entity entity)
{
    const auto& transform = m_ecs_manager->get_component<component::Hierarchy>(entity);

    // update removed's entity's parent to [none], effectively removing it from the hierarchy
    on_parent_update(entity, transform.parent, ecs::MAX_ENTITIES);
}

void HierarchySystem::on_parent_update(const ecs::Entity child, const ecs::Entity old_parent, const ecs::Entity new_parent)
{
    CGX_INFO("on_parent_update called: child:{} | old_parent:{} | new_parent: {}", child, old_parent, new_parent);
    CGX_ASSERT(m_ecs_manager->has_component<component::Hierarchy>(child), "no hierarchy component associated with specified child");

    if (old_parent != ecs::MAX_ENTITIES && m_entities.find(old_parent) != m_entities.end()) {
        auto& old_parent_component = m_ecs_manager->get_component<component::Hierarchy>(old_parent);
        auto it = std::find(old_parent_component.children.begin(), old_parent_component.children.end(), child);
        if (it != old_parent_component.children.end()) {
            old_parent_component.children.erase(it);
        }
    }

    // if new parent exists,
    if (new_parent != ecs::MAX_ENTITIES && m_entities.find(new_parent) != m_entities.end()) {
        auto& new_parent_component = m_ecs_manager->get_component<component::Hierarchy>(new_parent);
        new_parent_component.children.push_back(child);
    }

    auto& child_component = m_ecs_manager->get_component<component::Hierarchy>(child);
    child_component.parent = new_parent;
    if (m_ecs_manager->has_component<component::Transform>(child)) {
        auto& child_transform = m_ecs_manager->get_component<component::Transform>(child);
        child_transform.dirty = true;
    }

    sort_order_by_depth();
}

void HierarchySystem::sort_order_by_depth()
{
    m_order.clear();
    std::unordered_set<ecs::Entity> visited;

    for (const auto& entity : m_entities) {
        auto& hierarchy = m_ecs_manager->get_component<component::Hierarchy>(entity);
        if (hierarchy.parent == ecs::MAX_ENTITIES && visited.find(entity) == visited.end()) {
            dfs_children(entity, visited);
        }
    }
}

void HierarchySystem::dfs_children(const ecs::Entity entity, std::unordered_set<ecs::Entity>& visited)
{
    visited.insert(entity);
    m_order.push_back(entity);

    auto& hierarchy = m_ecs_manager->get_component<component::Hierarchy>(entity);
    for (const auto& child : hierarchy.children) {
        if (visited.find(child) == visited.end()) {
            dfs_children(child, visited);
        }
    }

}

const std::vector<ecs::Entity>& HierarchySystem::get_order() const
{
    return m_order;
}


}
