// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"

#include <memory>
#include <string>
#include <vector>

namespace cgx::resource
{
    class Mesh;
    class Shader;

    class Model : public Resource
    {
    public:
        Model(const std::string& path, const std::string& name, const std::vector<std::shared_ptr<Mesh>>& meshes);
        Model(const std::string& name, const std::vector<std::shared_ptr<Mesh>>& meshes);

        ~Model() = default; // todo

        virtual std::string getTypeName() const override { return "Model"; }

        void Draw(Shader& shader) const; 

        void Log();

    private:
        std::vector<std::shared_ptr<Mesh>> m_meshes;

    }; // class Model

} // namespace cgx::resource