// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

namespace cgx::scene
{
std::string translate_node_typename(const NodeType node_type)
{
    switch (node_type) {
        case NodeType::Entity: return "Entity";
        case NodeType::Camera: return "Camera";
        case NodeType::Light: return "Light";
        default:
            return "unknown_node_type";
    }
}

Node::Node(const NodeType type, const std::string& tag)
    : Hierarchy(core::ItemType::Node, tag)
    , node_type(type)
    , m_node_typename(translate_node_typename(type)) {}

Node::~Node() = default;

const NodeType& Node::get_node_type() const
{
    return node_type;
}

const std::string& Node::get_node_typename() const
{
    return m_node_typename;
}

std::string Node::get_path_prefix() const
{
    return Hierarchy::get_path_prefix() + get_node_typename() + "/";
}

EntityNode::EntityNode(const ecs::Entity entity_id, const std::string& tag)
    : Node(NodeType::Entity, tag)
    , m_entity(entity_id) {}

EntityNode::~EntityNode() = default;

const ecs::Entity& EntityNode::get_entity() const
{
    return m_entity;
}

CameraNode::CameraNode(const std::string& tag)
    : Node(NodeType::Camera, tag) {}

CameraNode::~CameraNode() = default;

LightNode::LightNode(const std::string& tag)
    : Node(NodeType::Light, tag) {}

LightNode::~LightNode() = default;
}
