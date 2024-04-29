// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"

namespace cgx::scene
{
class RootNode final : public Node
{
public:
    RootNode(std::string tag);
    ~RootNode() override;

    std::string get_path_prefix() const override;
    NodeType::Type get_node_type() const override;
};
}
