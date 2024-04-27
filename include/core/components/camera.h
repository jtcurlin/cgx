// Copyright © 2024 Jacob Curlin

#pragma once
#include <glm/glm.hpp>

namespace cgx::component
{
struct Camera
{
    float fov{45.0f};
    float aspect_ratio{1.0f};
    float near_plane{0.1f};
    float far_plane{100.0f};
    float zoom{45.0f};

    glm::mat4 view_matrix = glm::mat4(1.0f);
};
}
