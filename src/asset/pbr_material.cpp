// Copyright © 2024 Jacob Curlin

#include "asset/pbr_material.h"
#include "asset/shader.h"
#include "asset/texture.h"

namespace cgx::asset
{
PBRMaterial::PBRMaterial(
    std::string                     tag,
    std::string                     source_path,
    const glm::vec4                 base_color_factor,
    const float                     metallic_factor,
    const float                     roughness_factor,
    const std::shared_ptr<Texture>& base_color_map,
    const std::shared_ptr<Texture>& metallic_roughness_map,
    const std::shared_ptr<Texture>& normal_map,
    const std::shared_ptr<Texture>& occlusion_map,
    const std::shared_ptr<Texture>& emissive_map,
    std::shared_ptr<Shader>         shader)
    : Material(tag, get_path_prefix() + tag, std::move(source_path), std::move(shader))
    , m_base_color_factor{base_color_factor}
    , m_metallic_factor{metallic_factor}
    , m_roughness_factor{roughness_factor}
    , m_base_color_map{base_color_map}
    , m_metallic_roughness_map{metallic_roughness_map}
    , m_normal_map{normal_map}
    , m_occlusion_map{occlusion_map}
    , m_emissive_map{emissive_map} {}

PBRMaterial::~PBRMaterial() = default;

void PBRMaterial::bind(Shader* shader) const
{
    if (shader == nullptr)
        shader = m_shader.get();

    shader->use();

    // Set base color factor
    shader->set_vec4("baseColorFactor", m_base_color_factor);

    // Set metallic factor
    shader->set_float("metallicFactor", m_metallic_factor);

    // Set roughness factor
    shader->set_float("roughnessFactor", m_roughness_factor);

    // Bind base color map
    if (m_base_color_map != nullptr)
    {
        m_base_color_map->bind(0);
        shader->set_int("baseColorMap", 0);
    }

    // Bind metallic-roughness map
    if (m_metallic_roughness_map != nullptr)
    {
        m_metallic_roughness_map->bind(1);
        shader->set_int("metallicRoughnessMap", 1);
    }

    // Bind normal map
    if (m_normal_map != nullptr)
    {
        m_normal_map->bind(2);
        shader->set_int("normalMap", 2);
    }

    // Bind occlusion map
    if (m_occlusion_map != nullptr)
    {
        m_occlusion_map->bind(3);
        shader->set_int("occlusionMap", 3);
    }

    // Bind emissive map
    if (m_emissive_map != nullptr)
    {
        m_emissive_map->bind(4);
        shader->set_int("emissiveMap", 4);
    }
}

std::string PBRMaterial::get_path_prefix() const
{
    return Material::get_path_prefix() + "-" + MaterialType::get_lower_typename(MaterialType::PBR) + "/";
}

MaterialType::Type PBRMaterial::get_material_type() const
{
    return MaterialType::PBR;
}
}
