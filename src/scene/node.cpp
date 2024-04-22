// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

#include "core/events/engine_events.h"
#include "ecs/event_handler.h"

namespace cgx::scene
{

Node::Node(const ecs::Entity entity, const std::string& tag)
    : Hierarchy{core::ItemType::Node, tag}
    , m_entity{entity}
{}

Node::~Node() = default;

void Node::handle_parent_update(Hierarchy* old_parent, Hierarchy* new_parent)
{
    Hierarchy::handle_parent_update(old_parent, new_parent);
    const auto old_parent_node = dynamic_cast<Node*>(old_parent);
    const auto new_parent_node = dynamic_cast<Node*>(new_parent);

    const ecs::Entity old_parent_entity = old_parent_node ? old_parent_node->get_entity() : ecs::MAX_ENTITIES;
    const ecs::Entity new_parent_entity = new_parent_node ? new_parent_node->get_entity() : ecs::MAX_ENTITIES;

    CGX_INFO("Sending Event PARENT_UPDATE: child={}, old_parent={}, new_parent={}", get_entity(), old_parent_entity, new_parent_entity);
    ecs::Event event(events::hierarchy::PARENT_UPDATE);
    event.set_param(events::hierarchy::CHILD, get_entity());
    event.set_param(events::hierarchy::OLD_PARENT, old_parent_entity);
    event.set_param(events::hierarchy::NEW_PARENT, new_parent_entity);
    ecs::EventHandler::get_instance().send_event(event);
}

ecs::Entity Node::get_entity() const
{
    return m_entity;
}
}