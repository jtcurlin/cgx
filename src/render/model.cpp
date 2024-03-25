// Copyright © 2024 Jacob Curlin

#include "render/model.h"

namespace cgx::render
{
    Model::Model(std::string name, std::vector<std::shared_ptr<Mesh>> meshes)
        : m_name(std::move(name))
        , m_meshes(std::move(meshes)) {}

    void Model::Draw(Shader& shader)
    {
        for (auto& mesh : m_meshes)
        {
            mesh->Draw(shader);
        }

    }

    void Model::Log()
    {
        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
            CGX_DEBUG("[Model {}] : [Mesh {}]", m_name, i);
            auto& mesh = m_meshes[i];
            mesh->Log();
        }
    }
}