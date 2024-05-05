// Copyright © 2024 Jacob Curlin

#include "core/common.h"
#include "render/framebuffer.h"

#include "utility/error.h"

namespace cgx::render
{
Framebuffer::Framebuffer(const uint32_t width, const uint32_t height)
    : m_fbo(0)
    , m_dimensions{width, height}
    , m_clear_color{0.0f, 0.0f, 0.0f, 0.0f}
{
    glGenFramebuffers(1, &m_fbo);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::add_color_attachment(asset::Texture::Format format, asset::Texture::DataType data_type)
{
    const GLenum attachment_point = GL_COLOR_ATTACHMENT0 + m_color_attachment_count;

    GLsizei num_channels;

    switch(format) {
        case (asset::Texture::Format::Red):
        case (asset::Texture::Format::R16F):
        case (asset::Texture::Format::R32F): {
            num_channels = 1;
            break;
        }
        case (asset::Texture::Format::RG):
        case (asset::Texture::Format::RG16F):
        case (asset::Texture::Format::RG32F): {
            num_channels = 2;
            break;
        }
        case (asset::Texture::Format::RGB):
        case (asset::Texture::Format::RGB16F):
        case (asset::Texture::Format::RGB32F): {
            num_channels = 3;
            break;
        }
        case (asset::Texture::Format::RGBA):
        case (asset::Texture::Format::RGBA16F):
        case (asset::Texture::Format::RGBA32F): {
            num_channels = 4;
            break;
        }
    }

    m_textures[attachment_point] = std::make_unique<asset::Texture>(
        "Color Texture",
        "",
        m_dimensions[0],
        m_dimensions[1],
        4,
        format,
        data_type,
        nullptr);

    const auto* texture = m_textures[attachment_point].get();

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); CGX_CHECK_GL_ERROR;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_point, GL_TEXTURE_2D, texture->get_texture_id(), 0); CGX_CHECK_GL_ERROR;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); CGX_CHECK_GL_ERROR;


    m_attachments[attachment_point] = {
        AttachmentType::Color, static_cast<GLenum>(texture->get_format()), texture->get_texture_id()
    };

    m_color_attachment_count++;
}

void Framebuffer::add_depth_attachment(asset::Texture::Format format)
{
    CGX_ASSERT(m_attachments.find(GL_DEPTH_ATTACHMENT) == m_attachments.end(), "attempt to add >1 depth attachments to framebuffer");

    constexpr GLenum attachment_point = GL_DEPTH_ATTACHMENT;

    GLuint renderbuffer_id;
    glGenRenderbuffers(1, &renderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        static_cast<GLenum>(format),
        static_cast<GLsizei>(m_dimensions[0]),
        static_cast<GLsizei>(m_dimensions[1]));

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_point, GL_RENDERBUFFER, renderbuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_attachments[attachment_point] = {AttachmentType::Depth, static_cast<GLenum>(format), renderbuffer_id};
}

void Framebuffer::add_stencil_attachment(asset::Texture::Format format)
{
    CGX_ASSERT(m_attachments.find(GL_STENCIL_ATTACHMENT) == m_attachments.end(), "attempt to add >1 stencil attachments to framebuffer");

    constexpr GLenum attachment_point = GL_STENCIL_ATTACHMENT;

    GLuint renderbuffer_id;
    glGenRenderbuffers(1, &renderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        static_cast<GLenum>(format),
        static_cast<GLsizei>(m_dimensions[0]),
        static_cast<GLsizei>(m_dimensions[1]));

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_point, GL_RENDERBUFFER, renderbuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_attachments[attachment_point] = {AttachmentType::Stencil, static_cast<GLenum>(format), renderbuffer_id};
}

void Framebuffer::add_depth_stencil_attachment(asset::Texture::Format format)
{
    CGX_ASSERT(m_attachments.find(GL_DEPTH_STENCIL_ATTACHMENT) == m_attachments.end(), "attempt to add >1 depth/stencil attachments to framebuffer");
    CGX_ASSERT(m_attachments.find(GL_DEPTH_ATTACHMENT) == m_attachments.end(), "attempt to add >1 depth attachments to framebuffer");
    CGX_ASSERT(m_attachments.find(GL_STENCIL_ATTACHMENT) == m_attachments.end(), "attempt to add >1 stencil attachments to framebuffer");

    constexpr GLenum attachment_point = GL_DEPTH_STENCIL_ATTACHMENT;

    GLuint renderbuffer_id;
    glGenRenderbuffers(1, &renderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        static_cast<GLenum>(format),
        static_cast<GLsizei>(m_dimensions[0]),
        static_cast<GLsizei>(m_dimensions[1]));

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_point, GL_RENDERBUFFER, renderbuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_attachments[attachment_point] = {AttachmentType::DepthStencil, static_cast<GLenum>(format), renderbuffer_id};
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    if (m_color_attachment_count == 0) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    else {
        std::vector<GLenum> draw_buffers(m_color_attachment_count);
        for (int i = 0 ; i < m_color_attachment_count ; ++i) {
            draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(m_color_attachment_count, draw_buffers.data());
    }
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(const size_t width, const size_t height)
{
    m_dimensions[0] = width;
    m_dimensions[1] = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    for (const auto& [attachment_point, attachment_info] : m_attachments) {
        if (attachment_info.type != AttachmentType::Color) {
            glBindRenderbuffer(GL_RENDERBUFFER, attachment_info.id);
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                attachment_info.format,
                static_cast<GLsizei>(m_dimensions[0]),
                static_cast<GLsizei>(m_dimensions[1]));
        }
        else {
            m_textures[attachment_point]->resize(width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_point, GL_TEXTURE_2D, attachment_info.id, 0);
        }
    }

    check_completeness();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::clear(const bool clear_color, const bool clear_depth, const bool clear_stencil)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    GLbitfield clear_mask = 0;
    if (clear_color && m_color_attachment_count > 0) {
        clear_mask |= GL_COLOR_BUFFER_BIT;
        glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    }
    if (m_attachments.find(GL_DEPTH_STENCIL_ATTACHMENT) != m_attachments.end()) {
        if (clear_depth && clear_stencil) {
            glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
        }
        else if (clear_depth) {
            float depth_clear_value = 1.0f;
            glClearBufferfv(GL_DEPTH, 0, &depth_clear_value);
        }
        else if (clear_stencil) {
            GLint stencil_clear_value = 0;
            glClearBufferiv(GL_STENCIL, 0, &stencil_clear_value);
        }
    }
    else {
        if (clear_depth && m_attachments.find(GL_DEPTH_ATTACHMENT) != m_attachments.end()) {
            clear_mask |= GL_DEPTH_BUFFER_BIT;
            glClearDepth(1.0f);
        }
        if (clear_stencil && m_attachments.find(GL_STENCIL_ATTACHMENT) != m_attachments.end()) {
            clear_mask |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(0);
        }
    }
    if (clear_mask != 0) {
        glClear(clear_mask);
    }
}

const Framebuffer::AttachmentInfo& Framebuffer::get_attachment_info(const GLenum attachment_point) const
{
    const auto it = m_attachments.find(attachment_point);
    if (it == m_attachments.end()) // todo
    {
        CGX_ERROR("Attachment not found in framebuffer");
    }
    return it->second;
}

const asset::Texture* Framebuffer::get_texture(GLenum attachment_point) const
{
    const auto it = m_textures.find(attachment_point);
    if (it == m_textures.end()) // todo
    {
        CGX_ERROR("Attachment not found in framebuffer");
    }
    return it->second.get();
}

uint32_t Framebuffer::get_fbo() const
{
    return m_fbo;
}

void Framebuffer::get_dimensions(uint32_t& width, uint32_t& height) const
{
    width  = m_dimensions[0];
    height = m_dimensions[1];
}

void Framebuffer::set_clear_color(const float r, const float g, const float b, const float a)
{
    m_clear_color[0] = r;
    m_clear_color[1] = g;
    m_clear_color[2] = b;
    m_clear_color[3] = a;
}

void Framebuffer::get_clear_color(float& r, float& g, float& b, float& a) const
{
    r = m_clear_color[0];
    g = m_clear_color[1];
    b = m_clear_color[2];
    a = m_clear_color[3];
}

void Framebuffer::check_completeness() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    CGX_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "framebuffer is not complete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}
