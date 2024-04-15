// Copyright © 2024 Jacob Curlin

#include "asset/asset.h"

namespace cgx::asset
{
Asset::Asset (const std::string& source_path, const std::string& tag, const AssetType asset_type)
    : Item(core::ItemType::Asset, tag, Asset::get_path_prefix() + tag)
    , m_asset_type(asset_type)
    , m_asset_typename(translate_asset_typename(asset_type))
    , m_source_path(source_path)
{
    set_path(source_path);
}

const std::string& Asset::get_source_path () const
{
    return m_source_path;
}

const AssetType& Asset::get_asset_type () const
{
    return m_asset_type;
}

std::string Asset::get_asset_typename () const
{
    return m_asset_typename;
}

std::string Asset::get_path_prefix () const
{
    return Item::get_path_prefix() + get_asset_typename() + "/";
}

std::size_t Asset::generate_id (const std::string& path)
{
    return std::hash<std::string>()(path);
}

std::string translate_asset_typename (const AssetType type)
{
    switch (type) {
    case AssetType::Model: return "model";
    case AssetType::Mesh: return "mesh";
    case AssetType::Material: return "material";
    case AssetType::Texture: return "texture";
    case AssetType::Shader: return "shader";
    default:
        return "unknown_asset_type";
    }
}
}
