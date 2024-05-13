// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>

#include "asset/material.h"

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
    friend class gui::PropertiesPanel;

public:
    Mesh(
        std::string                      tag,
        std::string                      source_path,
        const std::vector<Vertex>&       vertices,
        const std::vector<uint32_t>&     indices,
        const std::shared_ptr<Material>& material = nullptr);
    ~Mesh() override;

    void set_material(const std::shared_ptr<Material>& material);

    void draw(Shader* shader) const;

    std::string     get_path_prefix() const override;
    AssetType::Type get_asset_type() const override;

    const glm::vec3& get_min_bounds() const;
    const glm::vec3& get_max_bounds() const;

    size_t get_vertex_count() const;
    size_t get_index_count() const;

    // const std::vector<Vertex>&   get_vertices();
    // const std::vector<uint32_t>& get_indices();

private:
    void initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    // std::vector<Vertex>       m_vertices{};
    // std::vector<uint32_t>     m_indices{};
    std::shared_ptr<Material> m_material{};

    size_t m_vertex_count{0};
    size_t m_index_count{0};

    glm::vec3 m_min_bounds;
    glm::vec3 m_max_bounds;

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};
};
}
