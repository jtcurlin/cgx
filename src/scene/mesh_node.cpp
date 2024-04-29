// Copyright © 2024 Jacob Curlin

#include "scene/mesh_node.h"

#include <iomanip>

namespace cgx::scene
{
MeshNode::MeshNode(std::string tag, const ecs::Entity entity)
    : Node(std::move(tag), entity)
{}

MeshNode::~MeshNode() = default;

std::string MeshNode::get_path_prefix() const
{
    return Node::get_path_prefix() + "/" + NodeType::get_lower_typename(NodeType::Mesh) + "/";
}

NodeType::Type MeshNode::get_node_type() const
{
    return NodeType::Mesh;
}
}
