// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/texture.h"
#include "glad/glad.h"

namespace cgx::render
{
class Framebuffer
{
public:
    enum class AttachmentType
    {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    struct AttachmentInfo
    {
        AttachmentType type;
        GLenum         format;
        GLuint         id;
    };

    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    void add_color_attachment(asset::Texture::Format format, asset::Texture::DataType data_type);
    void add_depth_attachment(asset::Texture::Format format);
    void add_stencil_attachment(asset::Texture::Format format);
    void add_depth_stencil_attachment(asset::Texture::Format format);

    void bind() const;
    void unbind() const;
    void resize(size_t width, size_t height);
    void clear(bool clear_color, bool clear_depth, bool clear_stencil);

    [[nodiscard]] const AttachmentInfo& get_attachment_info(GLenum attachment_point) const;
    [[nodiscard]] const asset::Texture* get_texture(GLenum attachment_point) const;

    [[nodiscard]] uint32_t              get_fbo() const;
    void                                get_dimensions(uint32_t& width, uint32_t& height) const;
    void                                set_clear_color(float r, float g, float b, float a);
    void                                get_clear_color(float& r, float& g, float& b, float& a) const;

    void                            check_completeness() const;

private:
    GLuint m_fbo;

    std::unordered_map<GLenum, AttachmentInfo>                  m_attachments;
    std::unordered_map<GLenum, std::unique_ptr<asset::Texture>> m_textures;

    int  m_color_attachment_count{0};

    uint32_t m_dimensions[2];
    float  m_clear_color[4];
};
}
