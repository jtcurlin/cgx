// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "geometry/common.h"
#include "utility/math.h"
#include "render/mesh.h"

namespace cgx::geometry
{

    enum class PrimitiveMeshType
    {
        plane
    };

    struct PrimitiveMeshInfo
    {
        PrimitiveMeshType   type;
        uint32_t            segments[3] = {1, 1, 1};
        glm::vec3           size{1.0f};
    };

    /*
    struct PrimitiveMesh{
        std::vector<Vertex>     vertices;
        std::vector<uint32_t>   indices;
    };
    */

    cgx::render::Mesh create_plane(const PrimitiveMeshInfo& info,
                                Axis horizontal_axis,
                                Axis vertical_axis,
                                WindingOrder winding,
                                glm::vec3 offset,
                                glm::vec2 u_range,
                                glm::vec2 v_range);

} // namespace cgx::geometry