// Copyright © 2024 Jacob Curlin

#include "asset/model.h"

#include "core/common.h"
#include "asset/mesh.h"

#include <vector>

namespace cgx::asset
{
Model::Model (const std::string& source_path, const std::string& tag, const std::vector<std::shared_ptr<Mesh>>& meshes)
    : Asset(source_path, tag, AssetType::Model)
    , m_meshes(meshes) {}

Model::Model(const std::string &source_path, const std::string &tag, const std::shared_ptr<Mesh>& mesh)
    : Asset(source_path, tag, AssetType::Model)
{
    m_meshes.push_back(mesh);
}

Model::~Model () = default;

void Model::draw (const Shader& shader) const
{
    for (auto& mesh : m_meshes) {
        mesh->draw(shader);
    }
}

void Model::log () const
{
    for (size_t i = 0 ; i < m_meshes.size() ; ++i) {
        CGX_DEBUG("[Model {}] : [Mesh {}]", get_tag(), i);
        auto& mesh = m_meshes[i];
    }
}

}
