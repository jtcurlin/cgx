// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/item.h"

namespace cgx::gui
{
class PropertiesPanel;
}

namespace cgx::asset
{
class Asset;
class AssetManager;
using AssetID                  = std::size_t;
constexpr AssetID k_invalid_id = core::k_invalid_id;

struct AssetType
{
    enum Type
    {
        Mesh,
        Model,
        Material,
        Texture,
        Shader,
        Cubemap,
    };

    static std::string get_typename(Type type);
    static std::string get_lower_typename(Type type);
};

class Asset : public core::Item
{
public:
    Asset(std::string tag, std::string internal_path, std::string external_path);

    std::string get_path_prefix() const override;
    virtual AssetType::Type get_asset_type() const = 0;
    virtual std::string     get_asset_typename() const;
    core::ItemType::Type    get_item_type() const override;

private:
    static std::size_t generate_id(const std::string& path);

    friend class AssetManager;
};
}
