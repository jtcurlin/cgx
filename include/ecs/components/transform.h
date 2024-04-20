// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>
#include "ecs/common.h"

namespace cgx::component
{
struct Transform
{
    glm::vec3 local_position = glm::vec3(0.0f);
    glm::vec3 local_rotation = glm::vec3(0.0f);
    glm::vec3 local_scale    = glm::vec3(1.0f);

    ecs::Entity parent{ecs::MAX_ENTITIES};
    std::vector<ecs::Entity> children{};

    glm::mat4 world_matrix = glm::mat4(1.0f);
    bool dirty{false};
};
}
