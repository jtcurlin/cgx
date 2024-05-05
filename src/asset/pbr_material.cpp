// Copyright © 2024 Jacob Curlin

#include "asset/pbr_material.h"
#include "asset/shader.h"
#include "asset/texture.h"
#include "utility/logging.h"

#define PBR_BASE_COLOR_MAP_BIT 1 << 0
#define PBR_METALLIC_ROUGHNESS_MAP_BIT 1 << 1
#define PBR_NORMAL_MAP_BIT 1 << 2
#define PBR_OCCLUSION_MAP_BIT 1 << 3
#define PBR_EMISSIVE_MAP_BIT 1 << 4

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
    if (shader == nullptr) shader = m_shader.get();
    CGX_VERIFY(shader);

    int map_bitset = 0;

    shader->use();

    shader->set_vec4("base_color_factor", m_base_color_factor);

    shader->set_float("metallic_factor", m_metallic_factor);

    shader->set_float("roughness_factor", m_roughness_factor);

    if (m_base_color_map != nullptr) {
        m_base_color_map->bind(0);
        shader->set_int("base_color_map", 0);
        map_bitset |= PBR_BASE_COLOR_MAP_BIT;
    }

    if (m_metallic_roughness_map != nullptr) {
        m_metallic_roughness_map->bind(1);
        shader->set_int("metallic_roughness_map", 1);
        map_bitset |= PBR_METALLIC_ROUGHNESS_MAP_BIT;
    }

    if (m_normal_map != nullptr) {
        m_normal_map->bind(2);
        shader->set_int("normal_map", 2);
        map_bitset |= PBR_NORMAL_MAP_BIT;
    }

    if (m_occlusion_map != nullptr) {
        m_occlusion_map->bind(3);
        shader->set_int("occlusion_map", 3);
        map_bitset |= PBR_OCCLUSION_MAP_BIT;
    }

    if (m_emissive_map != nullptr) {
        m_emissive_map->bind(4);
        shader->set_int("emissive_map", 4);
        map_bitset |= PBR_EMISSIVE_MAP_BIT;
    }

    shader->set_int("map_bitset", map_bitset);
    glActiveTexture(GL_TEXTURE0);
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
