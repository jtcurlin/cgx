// Copyright © 2024 Jacob Curlin

#pragma once

#include "glm/glm.hpp"
#include <memory>

namespace cgx::asset
{
class Mesh;
}

namespace cgx::geometry
{
enum class Axis
{
    x,
    y,
    z
};

std::shared_ptr<asset::Mesh> create_plane(
    uint32_t  x_segments      = 1,
    uint32_t  y_segments      = 1,
    uint32_t  z_segments      = 1,
    glm::vec3 size            = glm::vec3(1.0f),
    Axis      horizontal_axis = Axis::x,
    Axis      vertical_axis   = Axis::z,
    glm::vec3 offset          = glm::vec3(0.0f),
    glm::vec2 u_range         = {0.0f, 1.0f},
    glm::vec2 v_range         = {0.0f, 1.0f});

std::shared_ptr<asset::Mesh> create_sphere(uint32_t sector_count = 10, uint32_t stack_count = 10, float radius = 1);

std::shared_ptr<asset::Mesh> create_cube(
    glm::vec3 size    = glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3 offset  = glm::vec3(0.0f),
    glm::vec2 u_range = {0.0f, 1.0f},
    glm::vec2 v_range = {0.0f, 1.0f});
}
