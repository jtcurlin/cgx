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

enum class NodeFlag : std::uint32_t
{
    None   = 0,
    Mesh   = 1 << 2,
    Camera = 1 << 3,
    Light  = 1 << 4
};

inline NodeFlag operator|(NodeFlag a, NodeFlag b)
{
    return static_cast<NodeFlag>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline bool has_flag(NodeFlag flags, NodeFlag flag)
{
    return (static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(flag)) != 0;
}

std::string get_node_flag_name(NodeFlag flag, bool lower = false);
std::string get_node_flags_string(NodeFlag flags);

class Node : public core::Hierarchy
{
public:
    Node(std::string tag, ecs::Entity entity, NodeFlag flags = NodeFlag::None);

    void handle_parent_update(Hierarchy* old_parent, Hierarchy* new_parent) override;

    ecs::Entity get_entity() const;

    bool is_camera() const;
    bool is_mesh() const;
    bool is_empty() const;
    bool is_light() const;

    void set_flag(NodeFlag flag);
    void clear_flag(NodeFlag flag);

    core::ItemType::Type get_item_type() const override;

private:
    ecs::Entity m_entity{ecs::MAX_ENTITIES};
    NodeFlag    m_flags{NodeFlag::None};
};
}
