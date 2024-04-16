// Copyright © 2024 Jacob Curlin

#pragma once

#include "glad/glad.h"

namespace cgx::render
{
class Framebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    [[nodiscard]] uint32_t getFBO() const;

    [[nodiscard]] uint32_t getTextureID() const;

    [[nodiscard]] uint32_t getRenderBufferID() const;

    void getSize(uint32_t& width, uint32_t& height) const;

    void setClearColor(float r, float g, float b, float a);

    void getClearColor(float& r, float& g, float& b, float& a) const;

private:
    GLuint m_FBO;
    GLuint m_texture_id;
    GLuint m_render_buffer_id;

    GLsizei m_width;
    GLsizei m_height;
    float   m_CCR, m_CCG, m_CCB, m_CCA;
};
}
