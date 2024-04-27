// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"

namespace cgx::scene
{
class CameraNode final : public Node
{
public:
    CameraNode(std::string tag, ecs::Entity entity);
    ~CameraNode() override;

    std::string get_path_prefix() const override;
    NodeType::Type get_node_type() const override;
};
}
