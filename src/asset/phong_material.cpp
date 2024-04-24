// Copyright © 2024 Jacob Curlin

#include "asset/phong_material.h"

#include "asset/shader.h"
#include "asset/texture.h"
#include "utility/error.h"

#define AMBIENT_MAP_BIT 1
#define DIFFUSE_MAP_BIT 2
#define SPECULAR_MAP_BIT 4
#define NORMAL_MAP_BIT 8

namespace cgx::asset
{
PhongMaterial::PhongMaterial(
    std::string                     tag,
    std::string                     source_path,
    const float                     shininess,
    const glm::vec3                 ambient_color,
    const glm::vec3                 diffuse_color,
    const glm::vec3                 specular_color,
    const std::shared_ptr<Texture>& ambient_map,
    const std::shared_ptr<Texture>& diffuse_map,
    const std::shared_ptr<Texture>& specular_map,
    const std::shared_ptr<Texture>& normal_map,
    std::shared_ptr<Shader>         shader)
    : Material(tag, get_path_prefix() + tag, std::move(source_path), std::move(shader))
    , m_shininess(shininess)
    , m_ambient_color(ambient_color)
    , m_diffuse_color(diffuse_color)
    , m_specular_color(specular_color)
    , m_ambient_map(ambient_map)
    , m_diffuse_map(diffuse_map)
    , m_specular_map(specular_map)
    , m_normal_map(normal_map) {}

PhongMaterial::~PhongMaterial() = default;

void PhongMaterial::bind(Shader* shader) const
{
    Shader* s = shader;
    if (s == nullptr) {
        s = m_shader.get();
    }
    CGX_ASSERT(s, "no valid shader provided for binding");


    s->use();
    s->set_vec3("material.ambient_color", m_ambient_color);
    CGX_CHECK_GL_ERROR;
    s->set_vec3("material.diffuse_color", m_diffuse_color);
    CGX_CHECK_GL_ERROR;
    s->set_vec3("material.specular_color", m_specular_color);
    CGX_CHECK_GL_ERROR;
    s->set_float("material.shininess", m_shininess);
    CGX_CHECK_GL_ERROR;

    int map_bitset = 0;
    if (m_ambient_map != nullptr) {
        m_ambient_map->bind(0);
        s->set_int("material.ambient_map", 0);
        CGX_CHECK_GL_ERROR;
        map_bitset |= AMBIENT_MAP_BIT;
    }
    if (m_diffuse_map != nullptr) {
        m_diffuse_map->bind(1);
        s->set_int("material.diffuse_map", 1);
        CGX_CHECK_GL_ERROR;
        map_bitset |= DIFFUSE_MAP_BIT;
    }
    if (m_specular_map != nullptr) {
        m_specular_map->bind(2);
        s->set_int("material.specular_map", 2);
        CGX_CHECK_GL_ERROR;
        map_bitset |= SPECULAR_MAP_BIT;
    }
    s->set_int("material.map_bitset", map_bitset);
    CGX_CHECK_GL_ERROR;

    glActiveTexture(GL_TEXTURE0);
    CGX_CHECK_GL_ERROR;
}

std::string PhongMaterial::get_path_prefix() const
{
    return Material::get_path_prefix() + "-" + MaterialType::get_lower_typename(MaterialType::Phong) + "/";
}

MaterialType::Type PhongMaterial::get_material_type() const
{
    return MaterialType::Phong;
}
}
