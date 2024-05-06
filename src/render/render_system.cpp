// Copyright © 2024 Jacob Curlin

#define GLM_ENABLE_EXPERIMENTAL

#include "render/render_system.h"
#include "render/framebuffer.h"

#include "asset/model.h"
#include "asset/shader.h"

#include "core/event_handler.h"
#include "ecs/component_registry.h"
#include "scene/scene.h"
#include "utility/error.h"

#include "core/components/transform.h"
#include "core/components/render.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <filesystem>

#include "core/components/point_light.h"


namespace cgx::render
{
RenderSystem::RenderSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager) {}

RenderSystem::~RenderSystem() = default;

void RenderSystem::initialize()
{
    // initialize collider shader
    m_collider_shader = std::make_unique<asset::Shader>(
        "collider_shader",
        m_settings.collider_shader_path,
        asset::ShaderType::Unknown);

    m_geometry_shader = std::make_unique<asset::Shader>(
        "geometry_shader",
        m_settings.geometry_shader_path,
        asset::ShaderType::Unknown);

    m_lighting_shader = std::make_unique<asset::Shader>(
        "lighting_shader",
        m_settings.lighting_shader_path,
        asset::ShaderType::Unknown);

    // initialize collider shader
    m_light_mesh_shader = std::make_unique<asset::Shader>(
        "light_mesh_shader",
        m_settings.light_mesh_shader_path,
        asset::ShaderType::Unknown);

    // setup output framebuffer
    m_output_framebuffer = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_output_framebuffer->set_clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    m_output_framebuffer->add_color_attachment(asset::Texture::Format::RGBA, asset::Texture::DataType::UnsignedByte);
    m_output_framebuffer->add_depth_stencil_attachment(asset::Texture::Format::Depth24Stencil8);
    m_output_framebuffer->check_completeness();

    // setup g-buffer
    m_g_buffer = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_g_buffer->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // position
    m_g_buffer->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // normal
    m_g_buffer->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // albedo
    m_g_buffer->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float); // metallic
    m_g_buffer->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float); // roughness
    m_g_buffer->add_depth_stencil_attachment(asset::Texture::Format::Depth24Stencil8);
    m_g_buffer->check_completeness();

    // setup_test_triangle();

    glEnable(GL_DEPTH_TEST);
    CGX_CHECK_GL_ERROR;
}

void RenderSystem::begin_render()
{
    glDisable(GL_CULL_FACE);
    // CGX_CHECK_GL_ERROR;

    glViewport(0, 0, static_cast<GLsizei>(m_settings.render_width), static_cast<GLsizei>(m_settings.render_height));
    CGX_CHECK_GL_ERROR;
}

void RenderSystem::render()
{
    glViewport(0, 0, static_cast<GLsizei>(m_settings.render_width), static_cast<GLsizei>(m_settings.render_height));
    // begin_render();

    geometry_pass();
    lighting_pass();
    light_mesh_pass();

    if (m_settings.skybox_enabled) {
        draw_skybox();
    }

    m_output_framebuffer->unbind();

    // end_render();
}

void RenderSystem::end_render()
{
    if (m_settings.m_render_test_enabled) {
        glUseProgram(m_settings.m_render_test_shader);
        glBindVertexArray(m_settings.m_render_test_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
}

void RenderSystem::geometry_pass()
{
    m_g_buffer->bind();

    m_g_buffer->clear(true, true, true);

    static auto default_proj = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(m_settings.render_width) / static_cast<float>(m_settings.render_height),
        0.1f,
        100.0f);

    if (m_camera != ecs::MAX_ENTITIES) {
        auto& camera_c = get_component<component::Camera>(m_camera);
        m_view_mat     = camera_c.view_matrix;
        m_proj_mat     = camera_c.proj_matrix;
    }
    else {
        m_view_mat = glm::mat4(1.0f);
        m_proj_mat = default_proj;
    }

    for (auto& entity : m_entities) {
        if (!m_ecs_manager->has_component<component::Render>(entity)) {
            continue;
        }

        auto& render_c    = get_component<component::Render>(entity);
        auto& transform_c = get_component<component::Transform>(entity);

        auto* model = render_c.model.get();
        // auto* shader = render_c.shader.get()

        if (model == nullptr) {
            continue;
        }

        m_geometry_shader->use();

        m_geometry_shader->set_mat4("proj", m_proj_mat);
        m_geometry_shader->set_mat4("view", m_view_mat);
        m_geometry_shader->set_mat4("model", transform_c.world_matrix);

        model->draw(m_geometry_shader.get());
    }

    m_g_buffer->unbind();
}

void RenderSystem::lighting_pass()
{
    m_output_framebuffer->bind();
    m_output_framebuffer->clear(true, true, true);

    // bind color attachment textures
    m_g_buffer->get_texture(GL_COLOR_ATTACHMENT0)->bind(0); // position
    m_g_buffer->get_texture(GL_COLOR_ATTACHMENT1)->bind(1); // normal
    m_g_buffer->get_texture(GL_COLOR_ATTACHMENT2)->bind(2); // albedo
    m_g_buffer->get_texture(GL_COLOR_ATTACHMENT3)->bind(3); // roughness
    m_g_buffer->get_texture(GL_COLOR_ATTACHMENT4)->bind(4); // metallic

    m_lighting_shader->use();

    m_lighting_shader->set_int("g_position", 0);
    m_lighting_shader->set_int("g_normal", 1);
    m_lighting_shader->set_int("g_albedo", 2);
    m_lighting_shader->set_int("g_metallic", 3);
    m_lighting_shader->set_int("g_roughness", 4);

    int light_index = 0;
    m_curr_lights.clear();
    for (auto& entity : m_entities) {
        if (!m_ecs_manager->has_component<component::PointLight>(entity)) {
            continue;
        }

        const auto& lc = m_ecs_manager->get_component<component::PointLight>(entity);
        const auto& tc = m_ecs_manager->get_component<component::Transform>(entity);



        m_lighting_shader->set_vec3("lights[" + std::to_string(light_index) + "].position", glm::vec3(tc.world_matrix[3]));
        m_lighting_shader->set_vec3("lights[" + std::to_string(light_index) + "].color", lc.color);
        m_lighting_shader->set_float("lights[" + std::to_string(light_index) + "].intensity", lc.intensity);
        m_lighting_shader->set_float("lights[" + std::to_string(light_index) + "].range", lc.range);

        light_index++;
        m_curr_lights.push_back(entity);
    }

    m_lighting_shader->set_int("num_point_lights", light_index);
    m_lighting_shader->set_vec3("view_pos", m_ecs_manager->get_component<component::Transform>(m_camera).translation);

    render_quad();
    m_output_framebuffer->unbind();
}

void RenderSystem::light_mesh_pass()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_g_buffer->get_fbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_output_framebuffer->get_fbo());

    glBlitFramebuffer(
        0,
        0,
        m_settings.render_width,
        m_settings.render_height,
        0,
        0,
        m_settings.render_width,
        m_settings.render_height,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST);

    m_output_framebuffer->bind();

    m_light_mesh_shader->use();
    m_light_mesh_shader->set_mat4("proj", m_proj_mat);
    m_light_mesh_shader->set_mat4("view", m_view_mat);

    for (auto& entity : m_curr_lights) {
        auto lc = m_ecs_manager->get_component<component::PointLight>(entity);
        auto tc = m_ecs_manager->get_component<component::Transform>(entity);

        m_light_mesh_shader->set_mat4("model", tc.world_matrix);
        m_light_mesh_shader->set_vec3("light_color", lc.color);
        render_cube();
    }


}

void RenderSystem::render_quad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO = 0;

    if (quadVAO == 0) {
        constexpr float quadVertices[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

const std::shared_ptr<Framebuffer>& RenderSystem::get_output_buffer()
{
    return m_output_framebuffer;
}

const std::shared_ptr<Framebuffer>& RenderSystem::get_g_buffer()
{
    return m_g_buffer;
}

const std::shared_ptr<asset::Cubemap>& RenderSystem::get_skybox_cubemap() const
{
    return m_skybox_cubemap;
}

void RenderSystem::set_skybox_cubemap(const std::shared_ptr<asset::Cubemap>& cubemap)
{
    m_skybox_cubemap          = cubemap;
    m_settings.skybox_enabled = true;
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

void RenderSystem::draw_cube(const glm::vec3& size)
{
    static GLuint vao = 0;
    static GLuint vbo = 0;
    static GLuint ebo = 0;

    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> vertices = {
            // Front face
            {-size.x / 2, -size.y / 2, size.z / 2}, {size.x / 2, -size.y / 2, size.z / 2},
            {size.x / 2, size.y / 2, size.z / 2}, {-size.x / 2, size.y / 2, size.z / 2},

            // Back face
            {-size.x / 2, -size.y / 2, -size.z / 2}, {size.x / 2, -size.y / 2, -size.z / 2},
            {size.x / 2, size.y / 2, -size.z / 2}, {-size.x / 2, size.y / 2, -size.z / 2}
        };

        std::vector<GLuint> indices = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front face
            4, 5, 5, 6, 6, 7, 7, 4, // Back face
            0, 4, 1, 5, 2, 6, 3, 7  // Connecting lines
        };

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RenderSystem::draw_sphere(float radius)
{
    static GLuint vao = 0;
    static GLuint vbo = 0;

    const int segments = 16;
    const int rings    = 16;

    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);


        std::vector<glm::vec3> vertices;
        std::vector<GLuint>    indices;

        for (int i = 0 ; i <= rings ; ++i) {
            float theta = glm::pi<float>() * i / rings;
            for (int j = 0 ; j <= segments ; ++j) {
                float phi = 2.0f * glm::pi<float>() * j / segments;
                float x   = radius * std::sin(theta) * std::cos(phi);
                float y   = radius * std::cos(theta);
                float z   = radius * std::sin(theta) * std::sin(phi);
                vertices.emplace_back(x, y, z);
            }
        }

        for (int i = 0 ; i < rings ; ++i) {
            for (int j = 0 ; j < segments ; ++j) {
                indices.push_back(i * (segments + 1) + j);
                indices.push_back(i * (segments + 1) + j + 1);
                indices.push_back((i + 1) * (segments + 1) + j);

                indices.push_back(i * (segments + 1) + j + 1);
                indices.push_back((i + 1) * (segments + 1) + j + 1);
                indices.push_back((i + 1) * (segments + 1) + j);
            }
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, segments * rings * 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RenderSystem::render_cube()
{
    // initialize (if necessary)
    if (m_light_cube_vao == 0)
    {
        constexpr float vertices[] = {
            // back face
            -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.0f, 0.0f, // bottom-left
             0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.1f, 0.1f, // top-right
             0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.1f, 0.0f, // bottom-right
             0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.1f, 0.1f, // top-right
            -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.0f, 0.0f, // bottom-left
            -0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -0.1f, 0.0f, 0.1f, // top-left
            // front face
            -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.0f, 0.0f, // bottom-left
             0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.1f, 0.0f, // bottom-right
             0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.1f, 0.1f, // top-right
             0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.1f, 0.1f, // top-right
            -0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.0f, 0.1f, // top-left
            -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  0.1f, 0.0f, 0.0f, // bottom-left
            // left face
            -0.1f,  0.1f,  0.1f, -0.1f,  0.0f,  0.0f, 0.1f, 0.0f, // top-right
            -0.1f,  0.1f, -0.1f, -0.1f,  0.0f,  0.0f, 0.1f, 0.1f, // top-left
            -0.1f, -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, 0.0f, 0.1f, // bottom-left
            -0.1f, -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, 0.0f, 0.1f, // bottom-left
            -0.1f, -0.1f,  0.1f, -0.1f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -0.1f,  0.1f,  0.1f, -0.1f,  0.0f,  0.0f, 0.1f, 0.0f, // top-right
            // right face
             0.1f,  0.1f,  0.1f,  0.1f,  0.0f,  0.0f, 0.1f, 0.0f, // top-left
             0.1f, -0.1f, -0.1f,  0.1f,  0.0f,  0.0f, 0.0f, 0.1f, // bottom-right
             0.1f,  0.1f, -0.1f,  0.1f,  0.0f,  0.0f, 0.1f, 0.1f, // top-right
             0.1f, -0.1f, -0.1f,  0.1f,  0.0f,  0.0f, 0.0f, 0.1f, // bottom-right
             0.1f,  0.1f,  0.1f,  0.1f,  0.0f,  0.0f, 0.1f, 0.0f, // top-left
             0.1f, -0.1f,  0.1f,  0.1f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -0.1f, -0.1f, -0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 0.1f, // top-right
             0.1f, -0.1f, -0.1f,  0.0f, -0.1f,  0.0f, 0.1f, 0.1f, // top-left
             0.1f, -0.1f,  0.1f,  0.0f, -0.1f,  0.0f, 0.1f, 0.0f, // bottom-left
             0.1f, -0.1f,  0.1f,  0.0f, -0.1f,  0.0f, 0.1f, 0.0f, // bottom-left
            -0.1f, -0.1f,  0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 0.0f, // bottom-right
            -0.1f, -0.1f, -0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 0.1f, // top-right
            // top face
            -0.1f,  0.1f, -0.1f,  0.0f,  0.1f,  0.0f, 0.0f, 0.1f, // top-left
             0.1f,  0.1f , 0.1f,  0.0f,  0.1f,  0.0f, 0.1f, 0.0f, // bottom-right
             0.1f,  0.1f, -0.1f,  0.0f,  0.1f,  0.0f, 0.1f, 0.1f, // top-right
             0.1f,  0.1f,  0.1f,  0.0f,  0.1f,  0.0f, 0.1f, 0.0f, // bottom-right
            -0.1f,  0.1f, -0.1f,  0.0f,  0.1f,  0.0f, 0.0f, 0.1f, // top-left
            -0.1f,  0.1f,  0.1f,  0.0f,  0.1f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &m_light_cube_vao);
        glGenBuffers(1, &m_light_cube_vbo);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_light_cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(m_light_cube_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(m_light_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


RenderSettings& RenderSystem::get_render_settings()
{
    return m_settings;
}

ecs::Entity RenderSystem::get_camera() const
{
    return m_camera;
}

void RenderSystem::set_camera(ecs::Entity camera_entity)
{
    m_camera = camera_entity;
}
}
