// Copyright © 2024 Jacob Curlin

#pragma once

#include <glm/glm.hpp>

namespace cgx::component
{
struct Controllable
{
    bool position_control_active{false};
    bool orientation_control_active{false};

    glm::vec3 movement_speed = glm::vec3(2.5f, 2.5f, 2.5f);
    glm::vec2 rotation_speed = glm::vec2(0.1f, 0.1f);

};
}