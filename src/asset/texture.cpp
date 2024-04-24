// Copyright © 2024 Jacob Curlin

#include "asset/texture.h"
#include "stb_image.h"
#include "utility/error.h"

namespace cgx::asset
{
Texture::Texture(
    std::string tag,
    std::string source_path,
    const uint32_t     width,
    const uint32_t     height,
    const uint32_t     num_channels,
    const GLenum       format,
    unsigned char*     pixels)
    : Asset(tag, get_path_prefix() + tag, std::move(source_path))
    , m_width(width)
    , m_height(height)
    , m_num_channels(num_channels)
    , m_format(format)
    , m_pixels(pixels)
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
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    CGX_CHECK_GL_ERROR; // unbind texture

    set_min_filter(m_min_filter);
    set_mag_filter(m_mag_filter);
    set_wrap_s(m_wrap_s);
    set_wrap_t(m_wrap_t);
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

[[nodiscard]] FilterMode Texture::get_min_filter() const
{
    return m_min_filter;
}

[[nodiscard]] FilterMode Texture::get_mag_filter() const
{
    return m_mag_filter;
}

[[nodiscard]] WrapMode Texture::get_wrap_s() const
{
    return m_wrap_s;
}

[[nodiscard]] WrapMode Texture::get_wrap_t() const
{
    return m_wrap_t;
}

void Texture::set_min_filter(FilterMode filter)
{
    m_min_filter = filter;
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    CGX_CHECK_GL_ERROR;
}

void Texture::set_mag_filter(FilterMode filter)
{
    m_mag_filter = filter;
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    CGX_CHECK_GL_ERROR;
}

void Texture::set_wrap_s(WrapMode wrap)
{
    m_wrap_s = wrap;
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
    CGX_CHECK_GL_ERROR;
}

void Texture::set_wrap_t(WrapMode wrap)
{
    m_wrap_t = wrap;
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    CGX_CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
    CGX_CHECK_GL_ERROR;
}

std::string Texture::get_path_prefix() const
{
    return Asset::get_path_prefix() + "/" + AssetType::get_lower_typename(AssetType::Texture) + "/";
}

AssetType::Type Texture::get_asset_type() const { return AssetType::Texture; }
}
