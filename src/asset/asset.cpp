// Copyright © 2024 Jacob Curlin

#include "asset/asset.h"
#include <unordered_map>

namespace cgx::asset
{

std::string AssetType::get_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Mesh, "Mesh"}, {Model, "Model"}, {Material, "Material"}, {Texture, "Texture"}, {Shader, "Shader"},
        {Cubemap, "Cubemap"},
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "Unknown";
}

std::string AssetType::get_lower_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Mesh, "mesh"}, {Model, "model"}, {Material, "material"}, {Texture, "texture"}, {Shader, "shader"},
        {Cubemap, "cubemap"},
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "unknown";
}

Asset::Asset(std::string tag, std::string internal_path, std::string external_path)
    : Item(std::move(tag), std::move(internal_path), std::move(external_path)) {}

std::string Asset::get_path_prefix() const
{
    return Item::get_path_prefix() + "/" + core::ItemType::get_lower_typename(core::ItemType::Asset);
}

std::size_t Asset::generate_id(const std::string& path)
{
    return std::hash<std::string>()(path);
}

std::string Asset::get_asset_typename() const
{
    return AssetType::get_typename(get_asset_type());
}

core::ItemType::Type Asset::get_item_type() const
{
    return core::ItemType::Asset;
}
}
