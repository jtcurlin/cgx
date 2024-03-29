// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"
#include <glm/glm.hpp>

#include <string>
#include <memory>


namespace cgx::resource
{
    class Texture;
    class Shader;

    class Material : public Resource
    {
    public:
        Material(
            const std::string& path, const std::string& tag,
            float shininess = 32.0f, 
            glm::vec3 ambient_color = glm::vec3(0.2f, 0.1f, 0.1f), 
            glm::vec3 diffuse_color = glm::vec3(1.0f, 0.75f, 0.8f), 
            glm::vec3 specular_color = glm::vec3(1.0f, 0.9f, 0.9f), 
            const std::shared_ptr<Texture>& ambient_map = nullptr,
            const std::shared_ptr<Texture>& diffuse_map = nullptr,
            const std::shared_ptr<Texture>& specular_map = nullptr,
            const std::shared_ptr<Texture>& normal_map = nullptr
        );
        ~Material() = default;

        virtual ResourceType getType() const override { return ResourceType::Material; }

        void setAmbientMap(const std::shared_ptr<Texture>& map)     { m_ambient_map = map;  }
        void setDiffuseMap(const std::shared_ptr<Texture>& map)     { m_diffuse_map = map;  }
        void setSpecularMap(const std::shared_ptr<Texture>& map)    { m_specular_map = map; }
        void setNormalMap(const std::shared_ptr<Texture>& map)      { m_normal_map = map;   }

        void Bind(Shader& shader);

        void Log();

    private:
        glm::vec3 m_ambient_color;
        glm::vec3 m_diffuse_color;
        glm::vec3 m_specular_color;

        float m_shininess;

        std::shared_ptr<Texture> m_ambient_map;
        std::shared_ptr<Texture> m_diffuse_map;
        std::shared_ptr<Texture> m_specular_map;
        std::shared_ptr<Texture> m_normal_map;

    }; // class Material

} // namespace cgx::resource