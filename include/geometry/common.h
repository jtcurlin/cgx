// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>

namespace cgx::geometry
{
    enum class Axis
    {
        x,
        y,
        z
    };

    enum class WindingOrder
    {
        CW,
        CCW
    };

    struct Vertex
    {
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec2 uv{};
    };


}

