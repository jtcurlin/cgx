// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include <vector>

namespace cgx::asset
{
class Mesh;
class Shader;

class Model : public Asset
{
public:
    Model(const std::string &source_path, const std::string &tag, const std::vector<std::shared_ptr<Mesh>> &meshes);
    ~Model() override;

    void draw(const Shader &shader) const;
    void log() const;

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
};
}
