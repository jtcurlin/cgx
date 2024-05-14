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
#include <random>
#include <core/components/collider.h>
#include <utility/math.h>
#include <utility/primitive_mesh.h>

#include "core/components/point_light.h"

namespace cgx::render
{
RenderSystem::RenderSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager) {}

RenderSystem::~RenderSystem() = default;

void RenderSystem::initialize()
{
    m_geometry_shader = std::make_unique<asset::Shader>(
        "geometry_shader",
        m_settings.geometry_shader_path,
        asset::ShaderType::Unknown);

    m_lighting_shader = std::make_unique<asset::Shader>(
        "lighting_shader",
        m_settings.lighting_shader_path,
        asset::ShaderType::Unknown);

    m_light_mesh_shader = std::make_unique<asset::Shader>(
        "light_mesh_shader",
        m_settings.light_mesh_shader_path,
        asset::ShaderType::Unknown);

    m_collider_config.box_mesh    = geometry::create_cube();
    m_collider_config.sphere_mesh = geometry::create_sphere();
    m_collider_config.shader      = std::make_unique<asset::Shader>(
        "collider_shader",
        m_collider_config.shader_path,
        asset::ShaderType::Unknown);

    m_env_map = std::make_shared<asset::Cubemap>("env_map", std::string(DATA_DIRECTORY) + "/assets/misc/metro_noord_8k.hdr");

    // setup output framebuffer
    m_output_fb = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_output_fb->set_clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    m_output_fb->add_color_attachment(asset::Texture::Format::RGBA, asset::Texture::DataType::UnsignedByte);
    m_output_fb->add_depth_stencil_attachment(asset::Texture::Format::Depth24Stencil8);
    m_output_fb->check_completeness();

    // setup g-buffer
    m_gbuffer_fb = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_gbuffer_fb->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // position
    m_gbuffer_fb->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // normal
    m_gbuffer_fb->add_color_attachment(asset::Texture::Format::RGB, asset::Texture::DataType::Float); // albedo
    m_gbuffer_fb->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float); // metallic
    m_gbuffer_fb->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float); // roughness
    m_gbuffer_fb->add_depth_stencil_attachment(asset::Texture::Format::Depth24Stencil8);
    m_gbuffer_fb->check_completeness();

    init_ssao();

    glEnable(GL_DEPTH_TEST);
    CGX_CHECK_GL_ERROR;
}

void RenderSystem::init_ssao()
{
    m_ssao_config.main_shader = std::make_unique<asset::Shader>(
        "ssao_shader",
        m_ssao_config.ssao_shader_path,
        asset::ShaderType::Unknown);

    m_ssao_config.blur_shader = std::make_unique<asset::Shader>(
        "ssao_blur_shader",
        m_ssao_config.ssao_blur_shader_path,
        asset::ShaderType::Unknown);

    // setup ssao framebuffer
    m_ssao_config.main_fb = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_output_fb->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    m_ssao_config.main_fb->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float);
    m_ssao_config.main_fb->check_completeness();

    // setup ssaa-blur framebuffer
    m_ssao_config.blur_fb = std::make_shared<Framebuffer>(m_settings.render_width, m_settings.render_height);
    m_output_fb->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    m_ssao_config.blur_fb->add_color_attachment(asset::Texture::Format::Red, asset::Texture::DataType::Float);
    m_ssao_config.blur_fb->check_completeness();

    // red->white texture backgrounds for debug buffer view
    constexpr GLint swizzle[4] = {GL_RED, GL_RED, GL_RED, GL_ONE};
    m_ssao_config.main_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    m_ssao_config.blur_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(0);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

    std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0);
    std::default_random_engine              generator;
    for (uint32_t i = 0 ; i < 64 ; ++i) {
        glm::vec3 sample(
            random_floats(generator) * 2.0 - 1.0,
            random_floats(generator) * 2.0 - 1.0,
            random_floats(generator));
        sample = glm::normalize(sample);
        sample *= random_floats(generator);
        float scale = static_cast<float>(i) / 64.0f;
        scale       = math::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssao_config.kernel.push_back(sample);
    }

    std::vector<glm::vec3> ssao_noise_data;
    for (uint32_t i = 0 ; i < 16 ; i++) {
        glm::vec3 noise(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, 0.0f);
        ssao_noise_data.push_back(noise);
    }

    m_ssao_config.noise_tex = std::make_shared<asset::Texture>(
        "ssao_noise_texture",
        "",
        4,
        4,
        3,
        asset::Texture::Format::RGB,
        asset::Texture::DataType::Float,
        &ssao_noise_data[0]);

    m_ssao_config.main_shader->use();
    m_ssao_config.main_shader->set_int("g_position", 0);
    m_ssao_config.main_shader->set_int("g_normal", 1);
    m_ssao_config.main_shader->set_int("noise_tex", 2);

    m_ssao_config.blur_shader->use();
    m_ssao_config.blur_shader->set_int("ssao_input_tex", 0);

    m_lighting_shader->use();
    m_lighting_shader->set_int("ssao", 5);
}

void RenderSystem::render()
{
    glViewport(0, 0, static_cast<GLsizei>(m_settings.render_width), static_cast<GLsizei>(m_settings.render_height));

    geometry_pass();
    if (m_ssao_config.enabled) {
        ssao_pass();
    }
    lighting_pass();
    light_mesh_pass();

    if (m_collider_config.enabled) {
        CGX_TRACE("Drawing Colliders Enabled");
        collider_pass();
    }

    if (m_settings.skybox_enabled) {
        draw_skybox();
    }

    m_output_fb->unbind();
}

void RenderSystem::geometry_pass()
{
    m_gbuffer_fb->bind();

    m_gbuffer_fb->clear(true, true, true);

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

        if (model == nullptr) {
            continue;
        }

        m_geometry_shader->use();

        m_geometry_shader->set_mat4("proj", m_proj_mat);
        m_geometry_shader->set_mat4("view", m_view_mat);
        m_geometry_shader->set_mat4("model", transform_c.world_matrix);

        model->draw(m_geometry_shader.get());
    }

    m_gbuffer_fb->unbind();
}

void RenderSystem::lighting_pass()
{
    m_output_fb->bind();
    m_output_fb->clear(true, true, true);

    // bind color attachment textures
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(0);          // position
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT1)->bind(1);          // normal
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT2)->bind(2);          // albedo
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT3)->bind(3);          // roughness
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT4)->bind(4);          // metallic
    m_ssao_config.blur_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(5); // ssao

    auto env_map_texture = m_env_map->get_texture_id();
    glActiveTexture(GL_TEXTURE6); CGX_CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_map_texture); CGX_CHECK_GL_ERROR;

    m_lighting_shader->use();

    m_lighting_shader->set_int("g_position", 0);
    m_lighting_shader->set_int("g_normal", 1);
    m_lighting_shader->set_int("g_albedo", 2);
    m_lighting_shader->set_int("g_metallic", 3);
    m_lighting_shader->set_int("g_roughness", 4);

    if (m_ssao_config.enabled) {
        m_lighting_shader->set_bool("ssao_enabled", true);
        m_lighting_shader->set_int("ssao", 5);
    }
    else {
        m_lighting_shader->set_bool("ssao_enabled", false);
    }

    m_lighting_shader->set_int("irradiance_map", 6);

    int light_index = 0;
    m_curr_lights.clear();
    for (auto& entity : m_entities) {
        if (!m_ecs_manager->has_component<component::PointLight>(entity)) {
            continue;
        }

        const auto& lc = m_ecs_manager->get_component<component::PointLight>(entity);
        const auto& tc = m_ecs_manager->get_component<component::Transform>(entity);

        m_lighting_shader->set_vec3(
            "lights[" + std::to_string(light_index) + "].position",
            glm::vec3(tc.world_matrix[3]));
        m_lighting_shader->set_vec3("lights[" + std::to_string(light_index) + "].color", lc.color);
        m_lighting_shader->set_float("lights[" + std::to_string(light_index) + "].intensity", lc.intensity);
        m_lighting_shader->set_float("lights[" + std::to_string(light_index) + "].range", lc.range);
        m_lighting_shader->set_float("lights[" + std::to_string(light_index) + "].cutoff", lc.cutoff);

        light_index++;
        m_curr_lights.push_back(entity);
    }

    m_lighting_shader->set_int("num_point_lights", light_index);
    m_lighting_shader->set_vec3("view_pos", m_ecs_manager->get_component<component::Transform>(m_camera).translation);

    render_quad();
    m_output_fb->unbind();
}

void RenderSystem::light_mesh_pass()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer_fb->get_fbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_output_fb->get_fbo());

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

    m_output_fb->bind();

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

void RenderSystem::ssao_pass()
{
    m_ssao_config.main_fb->bind();
    m_ssao_config.main_fb->clear(true, false, false);

    m_ssao_config.main_shader->use();

    for (uint32_t i = 0 ; i < 64 ; ++i) {
        m_ssao_config.main_shader->set_vec3("samples[" + std::to_string(i) + "]", m_ssao_config.kernel[i]);
    }
    m_ssao_config.main_shader->set_mat4("proj", m_proj_mat);
    m_ssao_config.main_shader->set_mat4("view", m_view_mat);

    m_ssao_config.main_shader->set_float("power", m_ssao_config.power);
    m_ssao_config.main_shader->set_int("kernel_size", m_ssao_config.kernel_size);
    m_ssao_config.main_shader->set_float("radius", m_ssao_config.radius);
    m_ssao_config.main_shader->set_float("bias", m_ssao_config.bias);

    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(0); // position
    m_gbuffer_fb->get_texture(GL_COLOR_ATTACHMENT1)->bind(1); // normal
    m_ssao_config.noise_tex->bind(2);

    render_quad();

    m_ssao_config.main_fb->unbind();

    // blur
    m_ssao_config.blur_fb->bind();
    m_ssao_config.blur_fb->clear(true, false, false);

    m_ssao_config.blur_shader->use();
    m_ssao_config.main_fb->get_texture(GL_COLOR_ATTACHMENT0)->bind(0); // ssao color buffer

    glDisable(GL_BLEND);
    render_quad();
    glEnable(GL_BLEND);

    m_ssao_config.blur_fb->unbind();
}

void RenderSystem::collider_pass()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_output_fb->bind();
    for (auto& entity : m_entities) {
        if (!m_ecs_manager->has_component<component::Collider>(entity)) {
            continue;
        }

        auto& transform_c = m_ecs_manager->get_component<component::Transform>(entity);
        auto& collider_c  = m_ecs_manager->get_component<component::Collider>(entity);

        glm::mat4 scaled_mesh = glm::scale(transform_c.world_matrix, collider_c.size);

        m_collider_config.shader->use();
        m_collider_config.shader->set_mat4("proj", m_proj_mat);
        m_collider_config.shader->set_mat4("view", m_view_mat);
        m_collider_config.shader->set_mat4("model", scaled_mesh);
        m_collider_config.shader->set_vec4("color", m_collider_config.color);

        if (collider_c.type == component::Collider::Type::AABB) {
            m_collider_config.box_mesh->draw(m_collider_config.shader.get());
        }
        else if (collider_c.type == component::Collider::Type::Sphere) {
            m_collider_config.sphere_mesh->draw(m_collider_config.shader.get());
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glDisable(GL_BLEND);
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

const std::shared_ptr<Framebuffer>& RenderSystem::get_output_fb()
{
    return m_output_fb;
}

const std::shared_ptr<Framebuffer>& RenderSystem::get_gbuffer_fb()
{
    return m_gbuffer_fb;
}

const std::shared_ptr<Framebuffer>& RenderSystem::get_ssao_fb()
{
    return m_ssao_config.main_fb;
}

const std::shared_ptr<Framebuffer>& RenderSystem::get_ssao_blur_fb()
{
    return m_ssao_config.blur_fb;
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

void RenderSystem::render_cube()
{
    // initialize (if necessary)
    if (m_light_cube_vao == 0) {
        constexpr float vertices[] = {
            // back face
            -0.1f, -0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.0f, 0.0f, // bottom-left
            0.1f, 0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.1f, 0.1f,   // top-right
            0.1f, -0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.1f, 0.0f,  // bottom-right
            0.1f, 0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.1f, 0.1f,   // top-right
            -0.1f, -0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.0f, 0.0f, // bottom-left
            -0.1f, 0.1f, -0.1f, 0.0f, 0.0f, -0.1f, 0.0f, 0.1f,  // top-left
            // front face
            -0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, // bottom-left
            0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f,  // bottom-right
            0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.1f, 0.1f,   // top-right
            0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.1f, 0.1f,   // top-right
            -0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.0f, 0.1f,  // top-left
            -0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, // bottom-left
            // left face
            -0.1f, 0.1f, 0.1f, -0.1f, 0.0f, 0.0f, 0.1f, 0.0f,   // top-right
            -0.1f, 0.1f, -0.1f, -0.1f, 0.0f, 0.0f, 0.1f, 0.1f,  // top-left
            -0.1f, -0.1f, -0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.1f, // bottom-left
            -0.1f, -0.1f, -0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.1f, // bottom-left
            -0.1f, -0.1f, 0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.1f, 0.1f, 0.1f, -0.1f, 0.0f, 0.0f, 0.1f, 0.0f,   // top-right
            // right face
            0.1f, 0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.0f,   // top-left
            0.1f, -0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f, // bottom-right
            0.1f, 0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.1f,  // top-right
            0.1f, -0.1f, -0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f, // bottom-right
            0.1f, 0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.1f, 0.0f,   // top-left
            0.1f, -0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
            // bottom face
            -0.1f, -0.1f, -0.1f, 0.0f, -0.1f, 0.0f, 0.0f, 0.1f, // top-right
            0.1f, -0.1f, -0.1f, 0.0f, -0.1f, 0.0f, 0.1f, 0.1f,  // top-left
            0.1f, -0.1f, 0.1f, 0.0f, -0.1f, 0.0f, 0.1f, 0.0f,   // bottom-left
            0.1f, -0.1f, 0.1f, 0.0f, -0.1f, 0.0f, 0.1f, 0.0f,   // bottom-left
            -0.1f, -0.1f, 0.1f, 0.0f, -0.1f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.1f, -0.1f, -0.1f, 0.0f, -0.1f, 0.0f, 0.0f, 0.1f, // top-right
            // top face
            -0.1f, 0.1f, -0.1f, 0.0f, 0.1f, 0.0f, 0.0f, 0.1f, // top-left
            0.1f, 0.1f, 0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f,   // bottom-right
            0.1f, 0.1f, -0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.1f,  // top-right
            0.1f, 0.1f, 0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f,   // bottom-right
            -0.1f, 0.1f, -0.1f, 0.0f, 0.1f, 0.0f, 0.0f, 0.1f, // top-left
            -0.1f, 0.1f, 0.1f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &m_light_cube_vao);
        glGenBuffers(1, &m_light_cube_vbo);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_light_cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(m_light_cube_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
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

SSAOConfig& RenderSystem::get_ssao_config()
{
    return m_ssao_config;
}

ColliderConfig& RenderSystem::get_collider_config()
{
    return m_collider_config;
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
