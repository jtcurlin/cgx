// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>

namespace cgx::component
{
struct PointLight
{
    glm::vec3 color{237.0f, 211.0f, 78.0f};
    float intensity{1.0f};
    float range{1.0f};
};
}
