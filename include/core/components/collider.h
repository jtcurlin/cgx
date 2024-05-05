// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>

namespace cgx::component
{

struct Collider
{
    enum class Type
    {
        AABB,
        Sphere
    };

    Type      type;
    bool is_static {false};
    glm::vec3 size = glm::vec3(1.0f);
};

}
