// Copyright © 2024 Jacob Curlin

#include "asset/texture.h"
#include "stb/stb_image.h"
#include "utility/error.h"

namespace cgx::asset
{
Texture::Texture(
    const std::string& source_path,
    const std::string& tag,
    const uint32_t     width,
    const uint32_t     height,
    const uint32_t     num_channels,
    const GLenum       format,
    unsigned char*     pixels)
    : Asset(source_path, tag, AssetType::Texture)
    , m_width(width)
    , m_height(height)
    , m_num_channels(num_channels)
    , m_format(format)
    , m_pixels(pixels)
    , m_filter(TextureFilter::Linear)
{
    glGenTextures(1, &m_texture_id);
        CGX_CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        CGX_CHECK_GL_ERROR;
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            m_format,
            static_cast<GLsizei>(m_width),
            static_cast<GLsizei>(m_height),
            0,
            m_format,
            GL_UNSIGNED_BYTE,
            m_pixels);
    CGX_CHECK_GL_ERROR;

    glBindTexture(GL_TEXTURE_2D, 0);
    CGX_CHECK_GL_ERROR; // unbind texture

    set_texture_filter(TextureFilter::Linear);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture_id);
    CGX_CHECK_GL_ERROR;
    stbi_image_free(m_pixels);
}

void Texture::bind(const uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    CGX_CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
}

uint32_t Texture::get_width() const
{
    return m_width;
}

uint32_t Texture::get_height() const
{
    return m_height;
}

uint32_t Texture::get_num_channels() const
{
    return m_num_channels;
}

unsigned char* Texture::get_pixels() const
{
    return m_pixels;
}

GLenum Texture::get_format() const
{
    return m_format;
}

TextureFilter Texture::get_texture_filter() const
{
    return m_filter;
}

void Texture::set_texture_filter(const TextureFilter filter)
{
    m_filter = filter;

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
    switch (m_filter) {
        case TextureFilter::Linear: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            CGX_CHECK_GL_ERROR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            CGX_CHECK_GL_ERROR;
            break;
        case TextureFilter::Nearest: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            CGX_CHECK_GL_ERROR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            CGX_CHECK_GL_ERROR;
            break;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    CGX_CHECK_GL_ERROR; // unbind texture
}
}
