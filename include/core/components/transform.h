// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include <glm/glm.hpp>

namespace cgx::component
{
struct Transform
{
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation    = glm::vec3(0.0f);
    glm::vec3 scale     = glm::vec3(1.0f);

    glm::mat4 world_matrix = glm::mat4(1.0f);
    bool      dirty{false};
};
}


