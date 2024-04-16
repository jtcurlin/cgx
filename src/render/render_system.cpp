// Copyright © 2024 Jacob Curlin

#include "render/render_system.h"

#include "render/camera.h"
#include "render/framebuffer.h"
#include "asset/model.h"
#include "asset/shader.h"
#include "scene/scene.h"
#include "utility/error.h"

#include "ecs/component_registry.h"
#include "ecs/components/transform.h"
#include "ecs/components/render.h"
#include "event/event_handler.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace cgx::render
{
RenderSystem::RenderSystem(const std::shared_ptr<ecs::ComponentRegistry>& component_registry)
    : System(component_registry) {}

RenderSystem::~RenderSystem() = default;

void RenderSystem::initialize()
{
    glEnable(GL_DEPTH_TEST);
    CGX_CHECK_GL_ERROR;

    m_camera = std::make_unique<Camera>();

    m_framebuffer = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_framebuffer->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    init_msaa();

    setup_test_triangle();

    glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_framebuffer);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        CGX_ERROR("MSAA Framebuffer is incomplete: {}", status);
    }
}

void RenderSystem::update(const float dt)
{
    m_camera->Update(dt);
}

void RenderSystem::render()
{
    float r, g, b, a;
    m_framebuffer->getClearColor(r, g, b, a);
    glDisable(GL_CULL_FACE);
    CGX_CHECK_GL_ERROR;
    glClearColor(r, g, b, a);
    CGX_CHECK_GL_ERROR;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CGX_CHECK_GL_ERROR;

    if (m_settings.msaa_enabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_framebuffer);
        CGX_CHECK_GL_ERROR;
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer->getFBO());
        CGX_CHECK_GL_ERROR;
    }

    glViewport(0, 0, static_cast<GLsizei>(m_settings.render_width), static_cast<GLsizei>(m_settings.render_height));
    CGX_CHECK_GL_ERROR;
    glClearColor(r, g, b, a);
    CGX_CHECK_GL_ERROR;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CGX_CHECK_GL_ERROR;


    m_view_mat = m_camera->getViewMatrix();
    m_proj_mat = glm::perspective(
        glm::radians(m_camera->getZoom()),
        static_cast<float>(m_settings.render_width) / static_cast<float>(m_settings.render_height),
        0.1f,
        100.0f);

    // CGX_INFO("Rendering Models (Count={}", m_entities.size());
    for (auto& entity : m_entities) {
        glm::mat4 model_mat(1.0f);

        auto& render_c    = GetComponent<component::Render>(entity);
        auto& transform_c = GetComponent<component::Transform>(entity);

        if (!(render_c.model && render_c.shader)) {
            continue;
        }

        // apply rotations transformations around each axis
        model_mat = glm::rotate(
            model_mat,
            glm::radians(transform_c.local_rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis
        model_mat = glm::rotate(
            model_mat,
            glm::radians(transform_c.local_rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
        model_mat = glm::rotate(
            model_mat,
            glm::radians(transform_c.local_rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis

        model_mat = translate(model_mat, transform_c.local_position); // apply position transformation
        model_mat = scale(model_mat, transform_c.local_scale);        // apply scale transformation

        // activate shader program, set shader data, draw
        render_c.shader->use();

        // shader->setVec3("light.position", (m_ecsHandler->GetComponent<TransformComponent>(light))) /TODO
        render_c.shader->set_vec3("light.position", 1.0f, 1.0f, 1.0f);
        render_c.shader->set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
        render_c.shader->set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        render_c.shader->set_vec3("light.specular", 1.0f, 1.0f, 1.0f);

        render_c.shader->set_mat4("proj", m_proj_mat);
        render_c.shader->set_mat4("view", m_view_mat);
        render_c.shader->set_mat4("model", model_mat);

        render_c.model->draw(*(render_c.shader));
    }

    if (m_settings.m_render_test_enabled) {
        glUseProgram(m_settings.m_render_test_shader);
        glBindVertexArray(m_settings.m_render_test_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    // if (m_render_settings->skybox) { SkyboxRender(); }

    if (m_settings.msaa_enabled) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa_framebuffer);
        CGX_CHECK_GL_ERROR;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer->getFBO());
        CGX_CHECK_GL_ERROR;
        glBlitFramebuffer(
            0,
            0,
            static_cast<GLsizei>(m_settings.render_width),
            static_cast<GLsizei>(m_settings.render_height),
            0,
            0,
            static_cast<GLsizei>(m_settings.render_width),
            static_cast<GLsizei>(m_settings.render_height),
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
            GL_NEAREST);
        CGX_CHECK_GL_ERROR;
    }

    if (m_settings.skybox_enabled) {
        draw_skybox();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CGX_CHECK_GL_ERROR;

    glViewport(0, 0, static_cast<GLsizei>(m_settings.window_width), static_cast<GLsizei>(m_settings.window_height));
    CGX_CHECK_GL_ERROR;
}

void RenderSystem::draw_skybox() const
{
    if (m_skybox_cubemap) {
        // todo: check skybox model's material is cubemap
        glDepthFunc(GL_LEQUAL);
        const auto shader = m_skybox_cubemap->get_shader();
        shader->use();
        shader->set_mat4("view", glm::mat4(glm::mat3(m_view_mat)));
        shader->set_mat4("proj", m_proj_mat);
        m_skybox_cubemap->draw();
        glDepthFunc(GL_LESS);
    }
}

const std::shared_ptr<Framebuffer>& RenderSystem::getFramebuffer()
{
    return m_framebuffer;
}

const std::shared_ptr<asset::Cubemap>& RenderSystem::get_skybox_cubemap() const
{
    return m_skybox_cubemap;
}

void RenderSystem::set_skybox_cubemap(const std::shared_ptr<asset::Cubemap>& cubemap)
{
    m_skybox_cubemap = cubemap;
}

void RenderSystem::init_msaa()
{
    // setup MSAA frame buffer object
    glGenFramebuffers(1, &m_msaa_framebuffer);
    CGX_CHECK_GL_ERROR
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_framebuffer);
    CGX_CHECK_GL_ERROR;

    unsigned int msaa_tex_color_buf;
    glGenTextures(1, &msaa_tex_color_buf);
    CGX_CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_tex_color_buf);
    CGX_CHECK_GL_ERROR;
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        4,
        GL_RGBA,
        static_cast<GLsizei>(m_settings.render_width),
        static_cast<GLsizei>(m_settings.render_height),
        GL_TRUE);
    CGX_CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    CGX_CHECK_GL_ERROR;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaa_tex_color_buf, 0);
    CGX_CHECK_GL_ERROR;

    unsigned int msaa_rbo;
    glGenRenderbuffers(1, &msaa_rbo);
    CGX_CHECK_GL_ERROR;
    glBindRenderbuffer(GL_RENDERBUFFER, msaa_rbo);
    CGX_CHECK_GL_ERROR;
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        4,
        GL_DEPTH24_STENCIL8,
        static_cast<GLsizei>(m_settings.render_width),
        static_cast<GLsizei>(m_settings.render_height));
    CGX_CHECK_GL_ERROR;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CGX_CHECK_GL_ERROR;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaa_rbo);
    CGX_CHECK_GL_ERROR;

    CGX_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "MSAA Framebuffer not complete.");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CGX_CHECK_GL_ERROR;
}


void RenderSystem::setup_test_triangle()
{
    auto* test_vert_source = "#version 330 core\n" "layout (location = 0) in vec3 aPos;\n" "void main()\n" "{\n"
            "   gl_Position = vec4(aPos, 1.0);\n" "}\0";

    auto* test_frag_source = "#version 330 core\n" "out vec4 FragColor;\n" "void main()\n" "{\n"
            "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" "}\n\0";

    int success;

    const uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &test_vert_source, nullptr);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vert_shader, 512, nullptr, info_log);
        CGX_ERROR("Shader: test vertex shader compilation failed:\n {}", info_log)
    }

    const uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &test_frag_source, nullptr);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(frag_shader, 512, nullptr, info_log);
        CGX_ERROR("Shader: test fragment shader compilation failed:\n {}", info_log);
    }

    m_settings.m_render_test_shader = glCreateProgram();
    glAttachShader(m_settings.m_render_test_shader, vert_shader);
    glAttachShader(m_settings.m_render_test_shader, frag_shader);
    glLinkProgram(m_settings.m_render_test_shader);
    glGetProgramiv(m_settings.m_render_test_shader, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(m_settings.m_render_test_shader, 512, nullptr, info_log);
        CGX_ERROR("Shader: test shader linking failed:\n {}", info_log);
    }
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    constexpr float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

    uint32_t VBO;
    glGenVertexArrays(1, &m_settings.m_render_test_vao);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_settings.m_render_test_vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


RenderSettings& RenderSystem::get_render_settings()
{
    return m_settings;
}
}
