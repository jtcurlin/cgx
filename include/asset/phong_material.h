// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/material.h"
#include <glm/glm.hpp>

namespace cgx::asset
{
class PhongMaterial final : public Material
{
friend class gui::PropertiesPanel;
public:
    PhongMaterial(
        std::string                     tag,
        std::string                     source_path,
        float                           shininess      = 32.0f,
        glm::vec3                       ambient_color  = glm::vec3(0.2f, 0.1f, 0.1f),
        glm::vec3                       diffuse_color  = glm::vec3(1.0f, 0.75f, 0.8f),
        glm::vec3                       specular_color = glm::vec3(1.0f, 0.9f, 0.9f),
        const std::shared_ptr<Texture>& ambient_map    = nullptr,
        const std::shared_ptr<Texture>& diffuse_map    = nullptr,
        const std::shared_ptr<Texture>& specular_map   = nullptr,
        const std::shared_ptr<Texture>& normal_map     = nullptr,
        std::shared_ptr<Shader>         shader         = nullptr);
    ~PhongMaterial() override;

    void bind(Shader* shader) const override;

    std::string get_path_prefix() const override;
    MaterialType::Type get_material_type() const override;

private:
    glm::vec3                m_ambient_color;
    glm::vec3                m_diffuse_color;
    glm::vec3                m_specular_color;
    float                    m_shininess{32.0f};
    std::shared_ptr<Texture> m_ambient_map{nullptr};
    std::shared_ptr<Texture> m_diffuse_map{nullptr};
    std::shared_ptr<Texture> m_specular_map{nullptr};
    std::shared_ptr<Texture> m_normal_map{nullptr};

};
}
