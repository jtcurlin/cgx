// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

#include "core/events/ecs_events.h"
#include "core/event_handler.h"

namespace cgx::scene
{
std::string get_node_flag_name(const NodeFlag flag, const bool lower)
{
    static const std::unordered_map<NodeFlag, std::string> flag_names_lower = {
        {NodeFlag::None, "none"}, {NodeFlag::Mesh, "mesh"}, {NodeFlag::Camera, "camera"}, {NodeFlag::Light, "light"}
    };
    static const std::unordered_map<NodeFlag, std::string> flag_names_upper = {
        {NodeFlag::None, "None"}, {NodeFlag::Mesh, "Mesh"}, {NodeFlag::Camera, "Camera"}, {NodeFlag::Light, "Light"}
    };

    if (lower) {
        const auto it = flag_names_lower.find(flag);
        return it != flag_names_lower.end() ? it->second : "unknown";
    }
    const auto it = flag_names_upper.find(flag);
    return it != flag_names_upper.end() ? it->second : "Unknown";
}

std::string get_node_flags_string(const NodeFlag flags)
{
    std::string result;

    bool first = true;
    if (has_flag(flags, NodeFlag::Camera)) {
        result += "Camera";
        first = false;
    }
    if (has_flag(flags, NodeFlag::Mesh)) {
        if (!first) result += "|";
        result += "Mesh";
        first = false;
    }
    if (has_flag(flags, NodeFlag::Light)) {
        if (!first) result += "|";
        result += "Light";
        first = false;
    }


    return result.empty() ? "None" : result;
}

Node::Node(std::string tag, const ecs::Entity entity, NodeFlag flags)
    : Hierarchy{std::move(tag)}
    , m_entity{entity}
    , m_flags{flags} {}

void Node::handle_parent_update(Hierarchy* old_parent, Hierarchy* new_parent)
{
    Hierarchy::handle_parent_update(old_parent, new_parent);
    const auto old_parent_node = dynamic_cast<Node*>(old_parent);
    const auto new_parent_node = dynamic_cast<Node*>(new_parent);

    const ecs::Entity old_parent_entity = old_parent_node ? old_parent_node->get_entity() : ecs::MAX_ENTITIES;
    const ecs::Entity new_parent_entity = new_parent_node ? new_parent_node->get_entity() : ecs::MAX_ENTITIES;

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

bool Node::is_camera() const
{
    return has_flag(m_flags, NodeFlag::Camera);
}

bool Node::is_mesh() const
{
    return has_flag(m_flags, NodeFlag::Mesh);
}

bool Node::is_empty() const
{
    return m_flags == NodeFlag::None;
}

bool Node::is_light() const
{
    return has_flag(m_flags, NodeFlag::Light);
}

void Node::set_flag(NodeFlag flag)
{
    m_flags = static_cast<NodeFlag>(static_cast<std::uint32_t>(m_flags) | static_cast<std::uint32_t>(flag));
}

void Node::clear_flag(NodeFlag flag)
{
    m_flags = static_cast<NodeFlag>(static_cast<std::uint32_t>(m_flags) & ~static_cast<std::uint32_t>(flag));
}

core::ItemType::Type Node::get_item_type() const
{
    return core::ItemType::Node;
}
}
