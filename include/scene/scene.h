// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"

#include <string>

namespace cgx::scene
{
class Scene
{
public:
    explicit Scene(std::string label);
    ~Scene();

    [[nodiscard]] Node* get_root() const;
    const std::string& get_label() const;

private:
    std::shared_ptr<Node> m_root;
    std::string           m_label;
};
}
