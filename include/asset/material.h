// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include <glm/glm.hpp>
// #include "gui/panels/asset_panel.h"

namespace cgx::gui
{
class PropertiesPanel;
}

namespace cgx::asset
{
class Texture;
class Shader;

class Material final : public Asset
{
public:
    Material(
        const std::string &             source_path,
        const std::string &             tag,
        float                           shininess = 32.0f,
        glm::vec3                       ambient_color = glm::vec3(0.2f, 0.1f, 0.1f),
        glm::vec3                       diffuse_color = glm::vec3(1.0f, 0.75f, 0.8f),
        glm::vec3                       specular_color = glm::vec3(1.0f, 0.9f, 0.9f),
        const std::shared_ptr<Texture> &ambient_map = nullptr,
        const std::shared_ptr<Texture> &diffuse_map = nullptr,
        const std::shared_ptr<Texture> &specular_map = nullptr,
        const std::shared_ptr<Texture> &normal_map = nullptr
    );
    ~Material() override;

    void set_ambient_map(const std::shared_ptr<Texture> &map);
    void set_diffuse_map(const std::shared_ptr<Texture> &map);
    void set_specular_map(const std::shared_ptr<Texture> &map);
    void set_normal_map(const std::shared_ptr<Texture> &map);

    void bind(const Shader &shader) const;

    void log() const;

private:
    glm::vec3 m_ambient_color;
    glm::vec3 m_diffuse_color;
    glm::vec3 m_specular_color;

    float m_shininess;

    std::shared_ptr<Texture> m_ambient_map;
    std::shared_ptr<Texture> m_diffuse_map;
    std::shared_ptr<Texture> m_specular_map;
    std::shared_ptr<Texture> m_normal_map;

    friend class gui::PropertiesPanel;
};
}
