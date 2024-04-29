// Copyright © 2024 Jacob Curlin

#include "scene/root_node.h"

#include <iomanip>

namespace cgx::scene
{
RootNode::RootNode(std::string tag)
    : Node(std::move(tag), ecs::MAX_ENTITIES)
{}

RootNode::~RootNode() = default;

std::string RootNode::get_path_prefix() const
{
    return Node::get_path_prefix() + "/" + NodeType::get_lower_typename(NodeType::Root) + "/";
}

NodeType::Type RootNode::get_node_type() const
{
    return NodeType::Root;
}
}
