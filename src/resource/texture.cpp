// Copyright © 2024 Jacob Curlin

#include "resource/texture.h"
#include "glad/glad.h"
#include "stb/stb_image.h"
#include "utility/error.h"

#include <iomanip>

namespace cgx::resource
{
    Texture::Texture(const std::string& path, uint32_t width, uint32_t height, 
                     uint32_t num_channels, GLenum format, unsigned char* pixels)
        : Resource(path, path)
        , m_width(width)
        , m_height(height)
        , m_num_channels(num_channels)
        , m_format(format) 
        , m_pixels(pixels)
    {
        Setup();
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_texture_id); CGX_CHECK_GL_ERROR;
        stbi_image_free(m_pixels);
    }

    void Texture::Setup()
    {
        glGenTextures(1, &m_texture_id); CGX_CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, m_texture_id); CGX_CHECK_GL_ERROR;
        glTexImage2D(GL_TEXTURE_2D, 0, m_format, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height), 0, m_format, GL_UNSIGNED_BYTE, m_pixels); CGX_CHECK_GL_ERROR;

        glBindTexture(GL_TEXTURE_2D, 0); CGX_CHECK_GL_ERROR;   // unbind texture

        SetTextureFilter(TextureFilter::Linear); 
    }

    void Texture::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot); CGX_CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, m_texture_id); CGX_CHECK_GL_ERROR;
    }
   
    void Texture::SetTextureFilter(TextureFilter filter)
    {
        m_filter = filter;

        glBindTexture(GL_TEXTURE_2D, m_texture_id); CGX_CHECK_GL_ERROR;
        switch(m_filter)
        {
            case TextureFilter::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CGX_CHECK_GL_ERROR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CGX_CHECK_GL_ERROR;
                break;
            case TextureFilter::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); CGX_CHECK_GL_ERROR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); CGX_CHECK_GL_ERROR;
                break;
        }
        glBindTexture(GL_TEXTURE_2D, 0); CGX_CHECK_GL_ERROR;   // unbind texture
    }

} // namespace cgx::resource