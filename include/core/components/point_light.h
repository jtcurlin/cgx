// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>

namespace cgx::component
{
struct PointLight
{
    glm::vec3 color{0.961f, 0.957f, 0.925f};
    float intensity{1.0f};
    float range{1.0f};
};
}
