// Copyright © 2024 Jacob Curlin

#include <glm/glm.hpp>

namespace cgx::component
{
struct RigidBody
{
    glm::vec3 velocity     = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    glm::vec3 angular_velocity = glm::vec3(0.0f);
    glm::vec3 scale_rate = glm::vec3(0.0f);

    float mass{1.0f};
};
}
