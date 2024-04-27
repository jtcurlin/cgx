// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

#include "core/events/ecs_events.h"
#include "core/event_handler.h"

#include <iomanip>
#include <sstream>

namespace cgx::scene
{

std::string NodeType::get_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Mesh, "Mesh"}, {Camera, "Camera"}
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "Unknown";
}

std::string NodeType::get_lower_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Mesh, "mesh"}, {Camera, "camera"},
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "unknown";
}


Node::Node(std::string tag, const ecs::Entity entity)
    : Hierarchy{std::move(tag)}
    , m_entity{entity}
{}

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
    return core::ItemType::Node;
}

std::string Node::get_node_typename() const
{
    return NodeType::get_typename(get_node_type());
}
}
