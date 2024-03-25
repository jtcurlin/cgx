// Copyright © 2024 Jacob Curlin

#include "geometry/primitive_mesh.h"


namespace cgx::geometry
{
    std::shared_ptr<cgx::render::Mesh> create_plane(const PrimitiveMeshInfo& info,
                                Axis horizontal_axis,
                                Axis vertical_axis,
                                WindingOrder winding,
                                glm::vec3 offset,
                                glm::vec2 u_range,
                                glm::vec2 v_range)
    {
        CGX_INFO("primitive_mesh.cpp : create_plane");
        CGX_INFO(" > Info : Segments = ({} , {} , {}) ; Size = ({} , {} , {})", info.segments[0], info.segments[1], info.segments[2], info.size.x, info.size.y, info.size.z);
        CGX_INFO(" > offset = ({}, {}, {})", offset.x, offset.y, offset.z);
        CGX_INFO(" u_range: ({}, {})", u_range.x, u_range.y);
        CGX_INFO(" v_range: ({}, {})", v_range.x, v_range.y);

        const uint32_t horizontal_count = cgx::math::clamp(info.segments[static_cast<int>(horizontal_axis)], 1u, 10u);
        const uint32_t vertical_count = cgx::math::clamp(info.segments[static_cast<int>(vertical_axis)], 1u, 10u);

        const float horizontal_step = 1.0f / horizontal_count;
        const float vertical_step = 1.0f / vertical_count;

        const float u_step = (u_range.y - u_range.x) / horizontal_count;
        const float v_step = (v_range.y - v_range.x) / vertical_count;

        glm::vec3 size_scale(info.size.x, info.size.y, info.size.z);

        std::vector<cgx::render::Vertex> vertices;
        std::vector<uint32_t> indices;

        for (uint32_t j = 0; j <= vertical_count; ++j)
        {
            for (uint32_t i = 0; i <= horizontal_count; ++i)
            {
                glm::vec3 current_horizontal_step(0);
                glm::vec3 current_vertical_step(0);
                current_horizontal_step[static_cast<int>(horizontal_axis)] = i * horizontal_step;
                current_vertical_step[static_cast<int>(vertical_axis)] = j * vertical_step;

                glm::vec3 position = offset + current_horizontal_step + current_vertical_step;
                position *= size_scale;

                glm::vec3 base_horizontal(0.0f), base_vertical(0.0f);
                base_horizontal[static_cast<int>(horizontal_axis)] = 1.0f;
                base_vertical[static_cast<int>(vertical_axis)] = 1.0f;

                glm::vec3 normal = glm::cross(base_horizontal, base_vertical);
                if (winding == WindingOrder::CW)
                {
                    normal = -normal; // invert normal if CW winding order
                }

                glm::vec2 uv(u_range.x + (i * (u_range.y - u_range.x) / horizontal_count),
                             v_range.x + (j * (v_range.y - v_range.x) / vertical_count));

                vertices.push_back({position, normal, uv});
            }
        }

        const uint32_t row_length = horizontal_count + 1;
        for (uint32_t j = 0; j < vertical_count; ++j)
        {
            for (uint32_t i = 0; i < horizontal_count; ++i)
            {
                uint32_t bottom_left = i + j * row_length;
                uint32_t bottom_right = i + 1 + j * row_length;
                uint32_t top_left = i + (j + 1) * row_length;
                uint32_t top_right = i + 1 + (j + 1) * row_length;

                indices.push_back(top_left);
                indices.push_back(top_right);
                indices.push_back(bottom_right);

                indices.push_back(bottom_right);
                indices.push_back(bottom_left);
                indices.push_back(top_left);
            }
        }
        return std::make_shared<cgx::render::Mesh>(cgx::render::Mesh(vertices, indices, nullptr));
    }

} // namespace cgx::geometry