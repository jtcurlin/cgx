// Copyright © 2024 Jacob Curlin

#pragma once

#include <../../../external/glm/glm/glm.hpp>

namespace cgx::component
{

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    };

} // namespace cgx::component