// Copyright © 2024 Jacob Curlin

#include <glm/glm.hpp>

namespace cgx::component
{

    struct Transform
    {
        glm::vec3 local_position    = glm::vec3(0.0f);
        glm::vec3 local_rotation    = glm::vec3(0.0f);
        glm::vec3 local_scale       = glm::vec3(1.0f);

        Transform() {}

        Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
            : local_position(pos), local_rotation(rot), local_scale(scale) {}

        /*
        glm::vec3 world_position    = glm::vec3(0.0f);
        glm::vec3 world_rotation    = glm::vec3(0.0f);
        glm::vec3 world_scale       = glm::vec3(1.0f);

        glm::mat4 world_transform   = glm::mat4(1.0f);
        bool dirty                  = true; 

        Transform() {}

        Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
            : local_position(pos), local_rotation(rot), local_scale(scale),
              world_position(pos), world_rotation(rot), world_scale(scale),
              world_transform(glm::mat4(1.0f)), dirty(true) {}
        */

    }; // struct Transform

} // namespace cgx::component