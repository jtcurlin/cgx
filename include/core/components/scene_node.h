// Copyright © 2024 Jacob Curlin

#pragma once

#include <vector>

namespace cgx::ecs
{
    using Entity = std::uint32_t;
} // namespace cgx::ecs

namespace cgx::component
{
    enum class NodeDeleteMode
    {
        DeleteChildren,
        ReparentToGrandparent,
        Orphanchildren,
        AdoptBySibling
    };

    struct SceneNode
    {
        cgx::ecs::Entity parent;
        std::vector<cgx::ecs::Entity> children;
        NodeDeleteMode delete_mode;
    };


} // namespace cgx::component