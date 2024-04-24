// Copyright © 2024 Jacob Curlin

#include "asset/material.h"
#include <unordered_map>

namespace cgx::asset
{
std::string MaterialType::get_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> typeNames = {{PBR, "PBR"}, {Phong, "Phong"},};

    const auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "Unknown";
}

std::string MaterialType::get_lower_typename(const Type type)
{
     static const std::unordered_map<Type, std::string> typeNames = {{PBR, "pbr"}, {Phong, "phong"},};

    const auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "unknown";
}

Material::Material(std::string tag, std::string internal_path, std::string external_path, std::shared_ptr<Shader> shader)
    : Asset(tag, std::move(internal_path), std::move(external_path))
    , m_shader{std::move(shader)} {}

Material::~Material() = default;

std::shared_ptr<Shader> Material::get_shader() const
{
    return m_shader;
}

std::string Material::get_path_prefix() const
{
    return Asset::get_path_prefix() + "/" + AssetType::get_lower_typename(AssetType::Material);
}

AssetType::Type Material::get_asset_type() const
{
    return AssetType::Material;
}

std::string Material::get_material_typename() const
{
    return MaterialType::get_typename(get_material_type());
}

}
