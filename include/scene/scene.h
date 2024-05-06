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

    Node* add_node(NodeType::Type type, std::string tag, ecs::Entity entity, Node* parent=nullptr);

    void remove_node(Node* node);
    void remove_node_recursive(Node* node);

private:
    std::shared_ptr<Node> m_root;
    std::string                        m_label;
    // std::vector<std::shared_ptr<Node>> m_roots{};
};
}
