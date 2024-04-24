// Copyright © 2024 Jacob Curlin

#include "asset/model.h"

#include "core/common.h"
#include "asset/mesh.h"

#include <vector>

namespace cgx::asset
{
Model::Model(std::string tag, std::string source_path, const std::vector<std::shared_ptr<Mesh>>& meshes)
    : Asset(tag, get_path_prefix() + tag, std::move(source_path))
    , m_meshes(meshes) {}

Model::Model(std::string tag, std::string source_path, const std::shared_ptr<Mesh>& mesh)
    : Asset(tag, get_path_prefix() + tag, std::move(source_path))
{
    m_meshes.push_back(mesh);
}

Model::~Model() = default;

void Model::draw(Shader* shader) const
{
    for (auto& mesh : m_meshes) {
        mesh->draw(shader);
    }
}

std::string Model::get_path_prefix() const
{
    return Asset::get_path_prefix() + "/" + AssetType::get_lower_typename(AssetType::Model) + "/";
}

AssetType::Type Model::get_asset_type() const
{
    return AssetType::Model;
}
}
