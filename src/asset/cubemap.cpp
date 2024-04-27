// Copyright © 2024 Jacob Curlin

#include "asset/cubemap.h"
#include "asset/shader.h"

#include <stb_image.h>
#include "../../include/utility/primitive_mesh.h"
#include "utility/error.h"

namespace cgx::asset
{
Cubemap::Cubemap(std::string tag, std::string path, const std::vector<std::string>& face_texture_paths)
    : Asset(tag, get_path_prefix() + tag, std::move(path))
    , m_mesh(geometry::create_cube())
    , m_shader(
        std::make_shared<Shader>("skybox", std::string(SKYBOX_VERT_SHADER_CODE), std::string(SKYBOX_FRAG_SHADER_CODE)))
{
    CGX_ASSERT(face_texture_paths.size() == 6, "cubemap constructed w/ num texture image paths != 6");

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

    int    width, height, num_channels;
    GLenum format = 0;
    for (size_t i = 0 ; i < 6 ; ++i) {
        stbi_uc* data = nullptr;

        stbi_set_flip_vertically_on_load(0);
        data = stbi_load(face_texture_paths[i].c_str(), &width, &height, &num_channels, 0);

        if (data) {
            if (num_channels == 1) {
                format = GL_RED;
            }
            else if (num_channels == 3) {
                format = GL_RGB;
            }
            else if (num_channels == 4) {
                format = GL_RGBA;
            }

            if (format == 0) {
                CGX_ERROR(
                    "CubeMap: During construciton, failed to determine valid texture data "
                    "format for specified path. [{}]",
                    face_texture_paths[i]);
            }
        }
        if (!data) {
            CGX_ERROR(
                "Cubemap: During construction, failed to load texture resource at path {}",
                face_texture_paths[i]);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    m_format = format;
    m_width  = width;
    m_height = height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap() = default;

void Cubemap::draw() const
{
    m_shader->use();
    glActiveTexture(GL_TEXTURE0);
    CGX_CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);
    CGX_CHECK_GL_ERROR;
    m_mesh->draw(m_shader.get());
}

uint32_t Cubemap::get_width() const
{
    return m_width;
}

uint32_t Cubemap::get_height() const
{
    return m_height;
}


GLenum Cubemap::get_format() const
{
    return m_format;
}

const std::shared_ptr<Mesh>& Cubemap::get_mesh() const
{
    return m_mesh;
}

const std::shared_ptr<Shader>& Cubemap::get_shader() const
{
    return m_shader;
}

void Cubemap::set_mesh(const std::shared_ptr<Mesh>& mesh)
{
    m_mesh = mesh;
}

void Cubemap::set_shader(const std::shared_ptr<Shader>& shader)
{
    m_shader = shader;
}

std::string Cubemap::get_path_prefix() const
{
    return Asset::get_path_prefix() + "/" + AssetType::get_typename(AssetType::Cubemap) + "/";
}

AssetType::Type Cubemap::get_asset_type() const {
    return AssetType::Cubemap;
}


}
