// Copyright © 2024 Jacob Curlin

#include <glm/glm.hpp>

namespace cgx::component
{
struct Transform
{
    glm::vec3 local_position = glm::vec3(0.0f);
    glm::vec3 local_rotation = glm::vec3(0.0f);
    glm::vec3 local_scale    = glm::vec3(1.0f);

};
}
