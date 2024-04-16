// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include "asset/material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>

namespace cgx::gui
{
class PropertiesPanel;
}

namespace cgx::asset
{
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};
}

namespace std
{
template<>
struct hash<cgx::asset::Vertex>
{
    size_t operator()(cgx::asset::Vertex const& vertex) const noexcept
    {
        return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (
                   hash<glm::vec2>()(vertex.uv) << 1);
    }
};
}

namespace cgx::asset
{
class Shader;

class Mesh final : public Asset
{
public:
    Mesh(
        const std::string&               tag,
        const std::string&               source_path,
        const std::vector<Vertex>&       vertices,
        const std::vector<uint32_t>&     indices,
        const std::shared_ptr<Material>& material = nullptr);
    ~Mesh() override;

    void set_material(const std::shared_ptr<Material>& material);

    void draw(const Shader& shader) const;
    void log() const;

private:
    void initialize();

    std::vector<Vertex>       m_vertices{};
    std::vector<uint32_t>     m_indices{};
    std::shared_ptr<Material> m_material{};

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};

    friend class gui::PropertiesPanel;
};
}
