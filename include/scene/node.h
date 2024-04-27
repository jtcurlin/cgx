// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/hierarchy.h"
#include "ecs/ecs_manager.h"
#include "ecs/common.h"

namespace cgx::ecs
{
using Entity = std::uint32_t;
}

namespace cgx::scene
{
using NodeID                  = std::size_t;
constexpr NodeID k_invalid_id = core::k_invalid_id;

struct NodeType
{
    enum Type
    {
        Mesh,
        Camera
    };

    static std::string get_typename(Type type);
    static std::string get_lower_typename(Type type);
};

std::string get_node_typename();

class Node : public core::Hierarchy
{
public:
    Node(std::string tag, ecs::Entity entity);

    void handle_parent_update(Hierarchy* old_parent, Hierarchy* new_parent) override;

    ecs::Entity get_entity() const;

    core::ItemType::Type get_item_type() const override;

    virtual NodeType::Type get_node_type() const = 0;
    virtual std::string get_node_typename() const;

private:
    ecs::Entity m_entity;
};
}
