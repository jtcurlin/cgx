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

std::string get_node_typename();

class Node final : public core::Hierarchy
{
public:
    Node(ecs::Entity entity, const std::string& tag);
    ~Node() override;

    void handle_parent_update(Hierarchy* old_parent, Hierarchy* new_parent) override;

    ecs::Entity get_entity() const;

    core::ItemType::Type get_item_type() const override;

private:
    ecs::Entity m_entity;
};
}
