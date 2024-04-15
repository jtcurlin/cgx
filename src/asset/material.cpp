// Copyright © 2024 Jacob Curlin

#include "asset/material.h"

#include "asset/asset.h"
#include "asset/shader.h"
#include "asset/texture.h"
#include "utility/error.h"

#define AMBIENT_MAP_BIT 1
#define DIFFUSE_MAP_BIT 2
#define SPECULAR_MAP_BIT 4
#define NORMAL_MAP_BIT 8

namespace cgx::asset
{
Material::Material(
    const std::string&              source_path,
    const std::string&              tag,
    const float                     shininess,
    const glm::vec3                 ambient_color,
    const glm::vec3                 diffuse_color,
    const glm::vec3                 specular_color,
    const std::shared_ptr<Texture>& ambient_map,
    const std::shared_ptr<Texture>& diffuse_map,
    const std::shared_ptr<Texture>& specular_map,
    const std::shared_ptr<Texture>& normal_map)
    : Asset(source_path, tag, AssetType::Material)
    , m_shininess(shininess)
    , m_ambient_color(ambient_color)
    , m_diffuse_color(diffuse_color)
    , m_specular_color(specular_color)
    , m_ambient_map(ambient_map)
    , m_diffuse_map(diffuse_map)
    , m_specular_map(specular_map)
    , m_normal_map(normal_map) {}

Material::~Material() = default;

void Material::set_ambient_map(const std::shared_ptr<Texture>& map)
{
    m_ambient_map = map;
}

void Material::set_diffuse_map(const std::shared_ptr<Texture>& map)
{
    m_diffuse_map = map;
}

void Material::set_specular_map(const std::shared_ptr<Texture>& map)
{
    m_specular_map = map;
}

void Material::set_normal_map(const std::shared_ptr<Texture>& map)
{
    m_normal_map = map;
}

void Material::bind(const Shader& shader) const
{
    shader.use();
    shader.set_vec3("material.ambient_color", m_ambient_color);
    CGX_CHECK_GL_ERROR;
    shader.set_vec3("material.diffuse_color", m_diffuse_color);
    CGX_CHECK_GL_ERROR;
    shader.set_vec3("material.specular_color", m_specular_color);
    CGX_CHECK_GL_ERROR;
    shader.set_float("material.shininess", m_shininess);
    CGX_CHECK_GL_ERROR;

    int map_bitset = 0;
    if (m_ambient_map != nullptr) {
        m_ambient_map->bind(0);
        shader.set_int("material.ambient_map", 0);
        CGX_CHECK_GL_ERROR;
        map_bitset |= AMBIENT_MAP_BIT;
    }
    if (m_diffuse_map != nullptr) {
        m_diffuse_map->bind(1);
        shader.set_int("material.diffuse_map", 1);
        CGX_CHECK_GL_ERROR;
        map_bitset |= DIFFUSE_MAP_BIT;
    }
    if (m_specular_map != nullptr) {
        m_specular_map->bind(2);
        shader.set_int("material.specular_map", 2);
        CGX_CHECK_GL_ERROR;
        map_bitset |= SPECULAR_MAP_BIT;
    }
    shader.set_int("material.map_bitset", map_bitset);
    CGX_CHECK_GL_ERROR;

    glActiveTexture(GL_TEXTURE0);
    CGX_CHECK_GL_ERROR;
}

void Material::log() const
{
    CGX_DEBUG(" > Ambient_color = ({}, {}, {})", m_ambient_color.x, m_ambient_color.y, m_ambient_color.z);
    CGX_DEBUG(" > Diffuse = ({}, {}, {})", m_diffuse_color.x, m_diffuse_color.y, m_diffuse_color.z);
    CGX_DEBUG(" > Specular = ({}, {}, {})", m_specular_color.x, m_specular_color.y, m_specular_color.z);
    CGX_DEBUG(" > Shininess = {}", m_shininess);

    if (m_ambient_map != nullptr) {
        CGX_DEBUG(" > Ambient Map = {}", m_ambient_map->get_tag());
    }
    else
        CGX_DEBUG(" > Ambient Map = [None]");

    if (m_diffuse_map != nullptr) {
        CGX_DEBUG(" > Diffuse Map = {}", m_diffuse_map->get_tag());
    }
    else
        CGX_DEBUG(" > Diffuse Map = [None]");

    if (m_specular_map != nullptr) {
        CGX_DEBUG(" > Specular Map = {}", m_specular_map->get_tag());
    }
    else
        CGX_DEBUG(" > Specular Map = [None]");

    if (m_normal_map != nullptr) {
        CGX_DEBUG(" > Normal Map = {}", m_normal_map->get_tag());
    }
    else
        CGX_DEBUG(" > Normal Map = [None]");
}
} // namespace cgx::resource
