// Copyright © 2024 Jacob Curlin

#pragma once

#include "resource/resource.h"
#include "resource/material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>

namespace cgx::resource
{
    struct Vertex {
        glm::vec3 position{};   // position vector
        glm::vec3 normal{};     // normal vector
        glm::vec2 texCoord{};   // texture coordinate vector

        bool operator==(const Vertex& other) const 
        {
            return position == other.position   && 
                   normal == other.normal       && 
                   texCoord == other.texCoord;
        }
    };
}

namespace std 
{
    template<> struct hash<cgx::resource::Vertex>
    {
        size_t operator()(cgx::resource::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^
                    (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                    (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace cgx::resource
{
    class Shader;


    class Mesh : public Resource
    {
    public:

        Mesh(const std::string& path,
             const std::string& tag,
             const std::vector<Vertex>& vertices, 
             const std::vector<uint32_t>& indices, 
             const std::shared_ptr<Material>& material = nullptr);

        ~Mesh();

        void Draw(Shader &shader) const;

        void Log() const;

        void setMaterial(std::shared_ptr<Material> material) { m_material = material; }

        virtual ResourceType getType() const override { return ResourceType::Mesh; }

    private:
        void Setup();
        void Destroy();

        std::vector<Vertex>         m_vertices;
        std::vector<uint32_t>       m_indices;
        std::shared_ptr<Material>   m_material;          

        // gl render data id's
        uint32_t m_vao = 0;
        uint32_t m_vbo = 0;
        uint32_t m_ebo = 0;

    }; // class Mesh

} // namespace cgx::render