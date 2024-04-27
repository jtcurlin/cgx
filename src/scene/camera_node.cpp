// Copyright © 2024 Jacob Curlin

#include "scene/camera_node.h"

#include <iomanip>
#include <sstream>

namespace cgx::scene
{
CameraNode::CameraNode(std::string tag, const ecs::Entity entity)
    : Node(std::move(tag), entity)
{}

CameraNode::~CameraNode() = default;

std::string CameraNode::get_path_prefix() const
{
    return Node::get_path_prefix() + "/" + NodeType::get_lower_typename(NodeType::Camera) + "/";
}

NodeType::Type CameraNode::get_node_type() const
{
    return NodeType::Camera;
}
}
