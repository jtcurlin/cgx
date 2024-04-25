// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include <memory>

namespace cgx::asset
{
class Texture;
class Shader;

struct MaterialType
{
    enum Type
    {
        Phong,
        PBR
    };

    static std::string get_typename(Type type);
    static std::string get_lower_typename(Type type);
};

class Material : public Asset
{
public:
    Material(std::string tag, std::string internal_path, std::string external_path, std::shared_ptr<Shader> shader=nullptr);
    ~Material() override;

    virtual void bind(Shader* shader) const = 0;

    std::shared_ptr<Shader> get_shader() const;

    std::string get_path_prefix() const override;
    AssetType::Type            get_asset_type() const override;
    virtual MaterialType::Type get_material_type() const = 0;
    virtual std::string        get_material_typename() const;

protected:
    std::shared_ptr<Shader> m_shader;

};
}
