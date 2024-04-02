// Copyright © 2024 Jacob Curlin

#include <glm/glm.hpp>

namespace cgx::component
{

struct RigidBody
{
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

} // namespace cgx::component