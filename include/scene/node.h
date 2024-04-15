// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/hierarchy.h"

namespace cgx::ecs
{
using Entity = std::uint32_t;
}

namespace cgx::scene
{
using NodeID = std::size_t;
constexpr NodeID k_invalid_id = core::k_invalid_id;

enum class NodeType
{
    Entity,
    Camera,
    Light,
    Unknown
};

std::string get_node_typename();

class Node : public core::Hierarchy
{
public:
    explicit Node(NodeType type, const std::string &tag = "");
    ~Node() override;

    const NodeType&    get_node_type() const;
    const std::string& get_node_typename() const;

    std::string get_path_prefix() const override;

private:
    const NodeType node_type;
    const std::string m_node_typename;
};

class EntityNode final : public Node
{
public:
    EntityNode(ecs::Entity entity_id, const std::string &tag);
    ~EntityNode() override;

    const ecs::Entity& get_entity() const;

private:
    ecs::Entity m_entity;
};

class CameraNode final : public Node
{
public:
    explicit CameraNode(const std::string &tag);
    ~CameraNode() override;
};

class LightNode final : public Node
{
public:
    explicit LightNode(const std::string &tag);
    ~LightNode() override;
};



}
