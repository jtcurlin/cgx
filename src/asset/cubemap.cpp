// Copyright © 2024 Jacob Curlin

#include "asset/cubemap.h"
#include "asset/shader.h"

#include <stb_image.h>
#include <utility/paths.h>

#include "utility/primitive_mesh.h"
#include "utility/error.h"

namespace cgx::asset
{
Cubemap::Cubemap(std::string tag, std::string path, const std::vector<std::string>& face_texture_paths)
    : Asset(tag, get_path_prefix() + tag, std::move(path))
    , m_mesh(geometry::create_cube())
    , m_shader(
        std::make_shared<Shader>(
            "skybox",
            std::string(SKYBOX_VERT_SHADER_CODE),
            std::string(SKYBOX_FRAG_SHADER_CODE),
            ShaderType::Cubemap))
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

Cubemap::Cubemap(std::string tag, std::string hdr_path)
    : Asset(tag, get_path_prefix() + tag, std::move(hdr_path))
    , m_mesh(geometry::create_cube())
    , m_shader(
        std::make_shared<Shader>(
            "skybox",
            std::string(SKYBOX_VERT_SHADER_CODE),
            std::string(SKYBOX_FRAG_SHADER_CODE),
            ShaderType::Cubemap))
{
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true);
    int          width, height, num_components;
    float*       data = stbi_loadf(m_external_path.c_str(), &width, &height, &num_components, 0);
    unsigned int hdr_texture;
    if (data) {
        glGenTextures(1, &hdr_texture);
        glBindTexture(GL_TEXTURE_2D, hdr_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        CGX_ERROR("Cubemap::Cubemap() : failed to load HDR image at {}", hdr_path);
    }

    auto equirectangular_shader = Shader("equirectangular_shader", std::string(DATA_DIRECTORY) + "/shaders/equirectangular_cubemap", ShaderType::Unknown);
    auto irradiance_shader = Shader("irradiance_shader", std::string(DATA_DIRECTORY) + "/shaders/irradiance", ShaderType::Unknown);

    unsigned int capture_fbo, capture_rbo;
    glGenFramebuffers(1, &capture_fbo);
    glGenRenderbuffers(1, &capture_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo);

    unsigned int env_cubemap;
    glGenTextures(1, &env_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap);
    for (unsigned int i = 0 ; i < 6 ; ++i) {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 capture_proj    = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    auto cube_mesh = geometry::create_cube();

    equirectangular_shader.use();
    equirectangular_shader.set_int("rectangular_map", 0);
    equirectangular_shader.set_mat4("proj", capture_proj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    for (size_t i = 0; i < 6; ++i) {
        equirectangular_shader.set_mat4("view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_mesh->draw(&equirectangular_shader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int irradiance_map;
    glGenTextures(1, &irradiance_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
    for (size_t i=0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradiance_shader.use();
    irradiance_shader.set_int("environment_map", 0);
    irradiance_shader.set_mat4("proj", capture_proj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradiance_shader.set_mat4("view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_mesh->draw(&irradiance_shader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_texture_id = irradiance_map;
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

uint32_t Cubemap::get_texture_id() const
{
    return m_texture_id;
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

AssetType::Type Cubemap::get_asset_type() const
{
    return AssetType::Cubemap;
}

}
