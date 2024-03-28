// Copyright © 2024 Jacob Curlin

#include "render/mesh.h"
#include "utility/error.h"

namespace cgx::render
{

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> material)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)), m_material(std::move(material))
    {
        if (material == nullptr)
        {
            m_material = std::make_shared<Material>();
        }

        glGenVertexArrays(1, &m_vao); CGX_CHECK_GL_ERROR;
        glGenBuffers(1, &m_vbo); CGX_CHECK_GL_ERROR;
        glGenBuffers(1, &m_ebo); CGX_CHECK_GL_ERROR;

        glBindVertexArray(m_vao); CGX_CHECK_GL_ERROR;

        // setup vertex buffer object
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo); CGX_CHECK_GL_ERROR;
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), &m_vertices[0], GL_STATIC_DRAW); CGX_CHECK_GL_ERROR;

        // setup element array object (indices)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); CGX_CHECK_GL_ERROR;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)), &m_indices[0], GL_STATIC_DRAW); CGX_CHECK_GL_ERROR;

        // vertex data : position vectors
        glEnableVertexAttribArray(0); CGX_CHECK_GL_ERROR;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position)); CGX_CHECK_GL_ERROR;

        // vertex data : normal vectors
        glEnableVertexAttribArray(1); CGX_CHECK_GL_ERROR;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal)); CGX_CHECK_GL_ERROR;

        // vertex data: texture coordinates
        glEnableVertexAttribArray(2); CGX_CHECK_GL_ERROR;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoord)); CGX_CHECK_GL_ERROR;

        glBindVertexArray(0);   // unbind any bound vertex arrays
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &m_vao);    // free VAO
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void Mesh::Log() const
    {   

        CGX_DEBUG(" >> Vertex Data (count = {})", m_vertices.size());
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            const auto& v = m_vertices[i];
            CGX_DEBUG("  [Vertex {}] Pos({:.2f}, {:.2f}, {:.2f}), Norm({:.2f}, {:.2f}, {:.2f}), Tex({:.2f}, {:.2f})",
                    i, v.position.x, v.position.y, v.position.z,
                    v.normal.x, v.normal.y, v.normal.z,
                    v.texCoord.x, v.texCoord.y);
        }

        CGX_DEBUG(" >> Index Data (count = {})", m_indices.size());
        std::stringstream ss;
        for (size_t i = 0; i < m_indices.size(); ++i) {
            ss << m_indices[i];
            if ((i+1) % 3 != 0)
            {
                ss << ", ";
            } 
            else
            {
                CGX_DEBUG("  [Triangle {}] ({})", i / 3, ss.str());
                ss.str("");
            }
            if (i == m_indices.size() - 1 && (i + 1) % 3 != 0)
            {
                CGX_DEBUG("  [Partial Triangle {}] : ({})", i / 3, ss.str());
            }
        }

        CGX_DEBUG(" >> Material Data");
        if (m_material != nullptr) 
        {
            m_material->Log();
        }
        else
        {
            CGX_DEBUG("  [No material assigned]")
        }
        CGX_DEBUG("m_vao = {}", m_vao);
    }

    void Mesh::Draw(Shader& shader) const
    {
        if (m_material != nullptr)
        {
            m_material->bind(shader);    
        }

        glBindVertexArray(m_vao); CGX_CHECK_GL_ERROR;
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr); CGX_CHECK_GL_ERROR;
        glBindVertexArray(0); CGX_CHECK_GL_ERROR;
    }

}
