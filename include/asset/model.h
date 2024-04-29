// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include <vector>

#include "asset/material.h"

namespace cgx::asset
{
class Mesh;
class Shader;

class Model final : public Asset
{
    friend class gui::PropertiesPanel;

public:
    Model(std::string tag, std::string source_path, const std::vector<std::shared_ptr<Mesh>>& meshes);
    Model(std::string tag, std::string source_path, const std::shared_ptr<Mesh>& mesh);
    ~Model() override;

    void draw(Shader* shader) const;

    std::string get_path_prefix() const override;
    AssetType::Type get_asset_type() const override;

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
};
}
