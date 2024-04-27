// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"

namespace cgx::scene
{
class MeshNode final : public Node
{
public:
    MeshNode(std::string tag, ecs::Entity entity);
    ~MeshNode() override;

    std::string get_path_prefix() const override;
    NodeType::Type get_node_type() const override;
};
}