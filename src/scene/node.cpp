// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

#include "core/events/ecs_events.h"
#include "core/event_handler.h"

#include <iomanip>
#include <sstream>

#include "cgx.h"

namespace cgx::scene
{

Node::Node(const ecs::Entity entity, const std::string& tag)
    : Hierarchy{tag}
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
    core::event::Event event(core::event::component::hierarchy::PARENT_UPDATE);
    event.set_param(core::event::component::hierarchy::CHILD, get_entity());
    event.set_param(core::event::component::hierarchy::OLD_PARENT, old_parent_entity);
    event.set_param(core::event::component::hierarchy::NEW_PARENT, new_parent_entity);
    core::EventHandler::get_instance().send_event(event);
}

ecs::Entity Node::get_entity() const
{
    return m_entity;
}

core::ItemType::Type Node::get_item_type() const
{
    // return Hierarchy::get_item_type();
    return core::ItemType::Node;
}

std::string Node::get_default_tag()
{
    static size_t node_counter = 0;
    std::stringstream tag_ss;
    tag_ss << "Node " << std::setw(3) << std::setfill('0') << node_counter++;
    return tag_ss.str();
}

}
