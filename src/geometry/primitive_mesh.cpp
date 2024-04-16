// Copyright © 2024 Jacob Curlin

#include "geometry/primitive_mesh.h"
#include "asset/mesh.h"
#include "utility/math.h"


namespace cgx::geometry
{
std::shared_ptr<asset::Mesh> create_plane(
    uint32_t  x_segments,
    uint32_t  y_segments,
    uint32_t  z_segments,
    glm::vec3 size,
    Axis      horizontal_axis,
    Axis      vertical_axis,
    glm::vec3 offset,
    glm::vec2 u_range,
    glm::vec2 v_range)
{
    uint32_t horizontal_segments;
    uint32_t vertical_segments;

    switch (horizontal_axis) {
        case Axis::x: horizontal_segments = x_segments;
            break;
        case Axis::y: horizontal_segments = y_segments;
            break;
        case Axis::z: horizontal_segments = z_segments;
            break;
        default: horizontal_segments = z_segments;
    }
    switch (vertical_axis) {
        case Axis::x: vertical_segments = x_segments;
            break;
        case Axis::y: vertical_segments = y_segments;
            break;
        case Axis::z: vertical_segments = z_segments;
            break;
        default: vertical_segments = z_segments;
    }

    const uint32_t horizontal_count = cgx::math::clamp(horizontal_segments, 1u, 10u);
    const uint32_t vertical_count   = cgx::math::clamp(vertical_segments, 1u, 10u);

    const float horizontal_step = 1.0f / horizontal_count;
    const float vertical_step   = 1.0f / vertical_count;

    const float u_step = (u_range.y - u_range.x) / horizontal_count;
    const float v_step = (v_range.y - v_range.x) / vertical_count;

    glm::vec3 size_scale(size.x, size.y, size.z);

    std::vector<asset::Vertex> vertices;
    std::vector<uint32_t>      indices;

    for (uint32_t j = 0 ; j <= vertical_count ; ++j) {
        for (uint32_t i = 0 ; i <= horizontal_count ; ++i) {
            glm::vec3 current_horizontal_step(0);
            glm::vec3 current_vertical_step(0);
            current_horizontal_step[static_cast<int>(horizontal_axis)] = i * horizontal_step;
            current_vertical_step[static_cast<int>(vertical_axis)]     = j * vertical_step;

            glm::vec3 position = offset + current_horizontal_step + current_vertical_step;
            position *= size_scale;

            glm::vec3 base_horizontal(0.0f), base_vertical(0.0f);
            base_horizontal[static_cast<int>(horizontal_axis)] = 1.0f;
            base_vertical[static_cast<int>(vertical_axis)]     = 1.0f;

            glm::vec3 normal = glm::cross(base_horizontal, base_vertical);

            glm::vec2 uv(
                u_range.x + (i * (u_range.y - u_range.x) / horizontal_count),
                v_range.x + (j * (v_range.y - v_range.x) / vertical_count));

            vertices.push_back({position, normal, uv});
        }
    }

    const uint32_t row_length = horizontal_count + 1;
    for (uint32_t j = 0 ; j < vertical_count ; ++j) {
        for (uint32_t i = 0 ; i < horizontal_count ; ++i) {
            uint32_t bottom_left  = i + j * row_length;
            uint32_t bottom_right = i + 1 + j * row_length;
            uint32_t top_left     = i + (j + 1) * row_length;
            uint32_t top_right    = i + 1 + (j + 1) * row_length;

            indices.push_back(top_left);
            indices.push_back(top_right);
            indices.push_back(bottom_right);

            indices.push_back(bottom_right);
            indices.push_back(bottom_left);
            indices.push_back(top_left);
        }
    }

    return std::make_shared<asset::Mesh>("cgx://asset/mesh/primitive_plane", "primitive_plane", vertices, indices, nullptr);
}


std::shared_ptr<asset::Mesh> create_sphere(const uint32_t sector_count, const uint32_t stack_count, const float radius)
{
    float pi = glm::pi<float>();

    float sector_step = (2 * pi) / sector_count;
    float stack_step  = pi / stack_count;

    float sector_angle; // θ (latitude)
    float stack_angle;  // ϕ (longitude)

    float length_inv = 1.0f / radius;

    std::vector<asset::Vertex> vertices;

    float xy, z;
    for (uint32_t i = 0 ; i <= stack_count ; ++i) {
        stack_angle = (pi / 2) - i * stack_step;
        xy          = radius * cosf(stack_angle);
        z           = radius * sinf(stack_angle);

        for (uint32_t j = 0 ; j <= sector_count ; ++j) {
            asset::Vertex vertex;

            sector_angle = j * sector_step;

            vertex.position.x = xy * cosf(sector_angle);
            vertex.position.y = xy * sinf(sector_angle);
            vertex.position.z = z;

            vertex.normal.x = vertex.position.x * length_inv;
            vertex.normal.y = vertex.position.y * length_inv;
            vertex.normal.z = vertex.position.z * length_inv;

            vertex.uv.s = static_cast<float>(j) / sector_count;
            vertex.uv.t = static_cast<float>(i) / stack_count;
            vertices.push_back(vertex);
        }
    }

    std::vector<uint32_t> indices;
    std::vector<uint32_t> line_indices;
    uint32_t              k1; // index of current stack's top left vertex
    uint32_t              k2; // index of current stack' bottom left vertex

    for (uint32_t i = 0 ; i < stack_count ; ++i) {
        k1 = i * (sector_count + 1);
        k2 = k1 + sector_count + 1;

        for (uint32_t j = 0 ; j < sector_count ; ++j, ++k1, ++k2) {
            if (i != 0) // top left triangle
            {
                indices.push_back(k1);     // top left
                indices.push_back(k2);     // bottom left
                indices.push_back(k1 + 1); // top right
            }

            if (i != stack_count - 1) // bottom right triangle
            {
                indices.push_back(k1 + 1); // top right
                indices.push_back(k2);     // bottom left
                indices.push_back(k2 + 1); // bottom right
            }

            // vertical line
            line_indices.push_back(k1);
            line_indices.push_back(k2);

            // horizontal line (exclude first stack)
            if (i != 0) {
                line_indices.push_back(k1);
                line_indices.push_back(k1 + 1);
            }
        }
    }

    return std::make_shared<asset::Mesh>("cgx:://asset/mesh/primitive_sphere", "primitive_sphere", vertices, indices, nullptr);
}

std::shared_ptr<asset::Mesh> create_cube(
    const glm::vec3 size,
    const glm::vec3 offset,
    const glm::vec2 u_range,
    const glm::vec2 v_range)
{
    // std::vector<asset::Vertex> vertices;
    // std::vector<uint32_t>      indices;

    const glm::vec3 half_size = size * 0.5f;

    std::vector<asset::Vertex> vertices = {
        {
            offset + glm::vec3(-half_size.x, -half_size.y, half_size.z), glm::vec3(0.0, 0.0, 1.0),
            glm::vec2(u_range.x, v_range.y)
        },
        {
            offset + glm::vec3(half_size.x, -half_size.y, half_size.z), glm::vec3(0.0, 0.0, 1.0),
            glm::vec2(u_range.y, v_range.y)
        },
        {
            offset + glm::vec3(half_size.x, half_size.y, half_size.z), glm::vec3(0.0, 0.0, 1.0),
            glm::vec2(u_range.y, v_range.x)
        },
        {
            offset + glm::vec3(-half_size.x, half_size.y, half_size.z), glm::vec3(0.0, 0.0, 1.0),
            glm::vec2(u_range.x, v_range.x)
        },
        {
            offset + glm::vec3(-half_size.x, -half_size.y, -half_size.z), glm::vec3(0.0, 0.0, -1.0),
            glm::vec2(u_range.y, v_range.y)
        },
        {
            offset + glm::vec3(-half_size.x, half_size.y, -half_size.z), glm::vec3(0.0, 0.0, -1.0),
            glm::vec2(u_range.y, v_range.x)
        },
        {
            offset + glm::vec3(half_size.x, half_size.y, -half_size.z), glm::vec3(0.0, 0.0, -1.0),
            glm::vec2(u_range.x, v_range.x)
        },
        {
            offset + glm::vec3(half_size.x, -half_size.y, -half_size.z), glm::vec3(0.0, 0.0, -1.0),
            glm::vec2(u_range.x, v_range.y)
        }
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        3, 2, 6, 6, 5, 3, // Top face
        0, 4, 7, 7, 1, 0, // Bottom face
        1, 7, 6, 6, 2, 1, // Right face
        0, 3, 5, 5, 4, 0  // Left face
    };

    /* temp logging of results
    CGX_INFO("Cubemap Vertices:")
    for (int i = 0 ; i < vertices.size() ; ++i) {
        auto& vert = vertices[i];
        CGX_INFO(
            "Vertex {}: [pos= {}, {}, {}] ; [norm={}, {}, {}], ; [uv={}, {}]",
            i,
            vert.position.x,
            vert.position.y,
            vert.position.z,
            vert.normal.x,
            vert.normal.y,
            vert.normal.z,
            vert.uv[0],
            vert.uv[1])
    }
    CGX_INFO("Vertex Indices:");
    for (int i = 0 ; i < 36 ; i+=6) {
        CGX_INFO(
            "face {}: {} {} {} {} {} {}",
            i,
            indices[i],
            indices[i+1],
            indices[i+2],
            indices[i+3],
            indices[i+4],
            indices[i+5]);
    }
    */

    return std::make_shared<asset::Mesh>("primitive_cube", "cgx://asset/mesh/primitive_cube", vertices, indices);
}


}
