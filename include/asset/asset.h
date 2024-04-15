// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/item.h"
#include <functional>

namespace cgx::asset
{
class Asset;
class AssetManager;

using AssetID                  = std::size_t;
constexpr AssetID k_invalid_id = core::k_invalid_id;

enum class AssetType
{
    Mesh,
    Model,
    Material,
    Texture,
    Shader,
    Cubemap,
    Undefined
};

std::string translate_asset_typename (AssetType type);

class Asset : public core::Item
{
public:
    Asset (const std::string& source_path, const std::string& tag, AssetType asset_type);

    const std::string& get_source_path () const;
    const AssetType&   get_asset_type () const;
    std::string        get_asset_typename () const;

    std::string get_path_prefix () const override;

protected:
    const AssetType   m_asset_type;
    const std::string m_asset_typename;

    std::string m_source_path;

private:
    static std::size_t generate_id (const std::string& path);
    friend class AssetManager;
};
}

namespace std
{
template<>
struct hash<cgx::asset::AssetType>
{
    std::size_t operator() (const cgx::asset::AssetType& type) const noexcept
    {
        return std::hash<std::underlying_type<cgx::asset::AssetType>::type>()(
            static_cast<std::underlying_type<cgx::asset::AssetType>::type>(type));
    }
};
}
