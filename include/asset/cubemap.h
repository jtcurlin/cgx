// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include "asset/mesh.h"
#include "texture.h"

#include <vector>

#define SKYBOX_VERT_SHADER_CODE R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    out vec3 TexCoords;

    uniform mat4 proj;
    uniform mat4 view;

    void main()
    {
        TexCoords = aPos;
        vec4 pos = proj * view * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
    }
)glsl"

#define SKYBOX_FRAG_SHADER_CODE R"glsl(
    #version 330 core
    out vec4 FragColor;

    in vec3 TexCoords;

    uniform samplerCube skybox;

    void main()
    {
    FragColor = texture(skybox, TexCoords);
    }
)glsl"

namespace cgx::asset
{
class Cubemap final : public Asset
{
    friend class gui::PropertiesPanel;

public:
    Cubemap(std::string tag, std::string path, const std::vector<std::string>& face_texture_paths);
    ~Cubemap() override;

    void draw() const;

    [[nodiscard]] uint32_t get_width() const;
    [[nodiscard]] uint32_t get_height() const;
    [[nodiscard]] GLenum   get_format() const;

    const std::shared_ptr<Mesh>&   get_mesh() const;
    const std::shared_ptr<Shader>& get_shader() const;

    void set_mesh(const std::shared_ptr<Mesh>& mesh);
    void set_shader(const std::shared_ptr<Shader>& shader);

    std::string get_path_prefix() const override;
    AssetType::Type get_asset_type() const override;

private:
    uint32_t m_texture_id{0};
    uint32_t m_width{0};
    uint32_t m_height{0};
    GLenum   m_format{0};

    std::shared_ptr<Mesh>   m_mesh;
    std::shared_ptr<Shader> m_shader;
};
}
