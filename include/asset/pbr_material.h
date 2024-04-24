// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/material.h"
#include "glm/glm.hpp"

namespace cgx::asset
{
class PBRMaterial final : public Material
{
public:
    PBRMaterial(
        std::string                     tag,
        std::string                     source_path,
        glm::vec4                       base_color_factor      = glm::vec4(1.0f),
        float                           metallic_factor        = 0.0f,
        float                           roughness_factor       = 1.0f,
        const std::shared_ptr<Texture>& base_color_map         = nullptr,
        const std::shared_ptr<Texture>& metallic_roughness_map = nullptr,
        const std::shared_ptr<Texture>& normal_map             = nullptr,
        const std::shared_ptr<Texture>& occlusion_map          = nullptr,
        const std::shared_ptr<Texture>& emissive_map           = nullptr,
        std::shared_ptr<Shader>         shader                 = nullptr);
    ~PBRMaterial() override;

    void bind(Shader* shader) const override;

    std::string        get_path_prefix() const override;
    MaterialType::Type get_material_type() const override;

private:
    glm::vec4 m_base_color_factor = glm::vec4(1.0f);
    float     m_metallic_factor{0.0f};
    float     m_roughness_factor{1.0f};

    std::shared_ptr<Texture> m_base_color_map{nullptr};
    std::shared_ptr<Texture> m_metallic_roughness_map{nullptr};
    std::shared_ptr<Texture> m_normal_map{nullptr};
    std::shared_ptr<Texture> m_occlusion_map{nullptr};
    std::shared_ptr<Texture> m_emissive_map{nullptr};

    friend class PropertiesPanel;
};
}
