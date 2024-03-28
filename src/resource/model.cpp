// Copyright © 2024 Jacob Curlin

#include "resource/model.h"

#include "core/common.h"
#include "resource/mesh.h"
#include "resource/shader.h"

#include <string>
#include <vector>

namespace cgx::resource
{
    Model::Model(const std::string& path, const std::string& name, const std::vector<std::shared_ptr<Mesh>>& meshes)
        : Resource(path, path, name), m_meshes(meshes)
    {}

    Model::Model(const std::string& name, const std::vector<std::shared_ptr<Mesh>>& meshes)
         : Resource(name) , m_meshes(meshes)
    {}
    
    void Model::Draw(Shader& shader) const
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
            CGX_DEBUG("[Model {}] : [Mesh {}]", getName(), i);
            auto& mesh = m_meshes[i];
        }

    }

} // namespace cgx::resource