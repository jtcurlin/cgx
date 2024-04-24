// Copyright © 2024 Jacob Curlin

#include "asset/mesh.h"
#include "asset/material.h"
#include "utility/error.h"

#include <glad/glad.h>

namespace cgx::asset
{
Mesh::Mesh(
    std::string                      tag,
    std::string                      source_path,
    const std::vector<Vertex>&       vertices,
    const std::vector<uint32_t>&     indices,
    const std::shared_ptr<Material>& material)
    : Asset(tag, get_path_prefix() + tag, std::move(source_path))
    , m_vertices(vertices)
    , m_indices(indices)
    , m_material(material)
{
    initialize();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Mesh::set_material(const std::shared_ptr<Material>& material)
{
    m_material = material;
}

void Mesh::draw(Shader* shader) const
{
    if (m_material != nullptr) {
        m_material->bind(shader);
    }

    glBindVertexArray(m_vao);
    CGX_CHECK_GL_ERROR;
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    CGX_CHECK_GL_ERROR;
    glBindVertexArray(0);
    CGX_CHECK_GL_ERROR;
}

std::string Mesh::get_path_prefix() const
{
    return Asset::get_path_prefix() + "/" + AssetType::get_lower_typename(AssetType::Mesh) + "/";
}

AssetType::Type Mesh::get_asset_type() const { return AssetType::Mesh; }

void Mesh::initialize()
{
    // CGX_ASSERT(true, "todo");  need to add assertions to check vertices, indices ready for gl init

    glGenVertexArrays(1, &m_vao);
    CGX_CHECK_GL_ERROR;
    glGenBuffers(1, &m_vbo);
    CGX_CHECK_GL_ERROR;
    glGenBuffers(1, &m_ebo);
    CGX_CHECK_GL_ERROR;

    glBindVertexArray(m_vao);
    CGX_CHECK_GL_ERROR;

    // setup vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CGX_CHECK_GL_ERROR;
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)),
        &m_vertices[0],
        GL_STATIC_DRAW);
    CGX_CHECK_GL_ERROR;

    // setup element array object (indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    CGX_CHECK_GL_ERROR;
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
        &m_indices[0],
        GL_STATIC_DRAW);
    CGX_CHECK_GL_ERROR;

    // vertex data : position vectors
    glEnableVertexAttribArray(0);
    CGX_CHECK_GL_ERROR;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    CGX_CHECK_GL_ERROR;

    // vertex data : normal vectors
    glEnableVertexAttribArray(1);
    CGX_CHECK_GL_ERROR;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    CGX_CHECK_GL_ERROR;

    // vertex data: texture coordinates
    glEnableVertexAttribArray(2);
    CGX_CHECK_GL_ERROR;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
    CGX_CHECK_GL_ERROR;

    glBindVertexArray(0); // unbind any bound vertex arrays
}
}
