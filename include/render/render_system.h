// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/cubemap.h"
#include "core/components/camera.h"
#include "ecs/system.h"

#include "render/framebuffer.h"

namespace cgx::asset
{
class Shader;
class Model;
}

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::core
{
class EventHandler;
}

namespace cgx::render
{
struct RenderSettings
{
    uint32_t window_width{1920};
    uint32_t window_height{1080};
    uint32_t render_width{1280};
    uint32_t render_height{720};

    bool manual_control{false};
    bool msaa_enabled{false};
    bool skybox_enabled{false};

    bool              default_shader_enabled{false};
    const std::string default_shader_path{std::string(DATA_DIRECTORY) + "/shaders/pbr"};

    bool              draw_colliders{false};
    const std::string collider_shader_path{std::string(DATA_DIRECTORY) + "/shaders/collider"};


    const std::string geometry_shader_path{std::string(DATA_DIRECTORY) + "/shaders/geometry"};
    const std::string lighting_shader_path{std::string(DATA_DIRECTORY) + "/shaders/lighting"};
    const std::string light_mesh_shader_path{std::string(DATA_DIRECTORY) + "/shaders/light_mesh"};

    bool default_model_enabled{false};

    bool     m_render_test_enabled{false};
    uint32_t m_render_test_shader{};
    uint32_t m_render_test_vao{};
};

class RenderSystem final : public ecs::System
{
public:
    explicit RenderSystem(ecs::ECSManager* ecs_manager);
    ~RenderSystem() override;

    void initialize();
    void begin_render();
    void render();
    void end_render();

    void geometry_pass();
    void lighting_pass();
    void light_mesh_pass();
    void output_pass();

    void render_quad();

    void frame_update(float dt) override {};
    void fixed_update(float dt) override {};

    void on_entity_added(ecs::Entity entity) override {}
    void on_entity_removed(ecs::Entity entity) override {}

    void draw_skybox() const;

    const std::shared_ptr<Framebuffer>& get_output_buffer();
    const std::shared_ptr<Framebuffer>& get_g_buffer();

    void set_skybox_cubemap(const std::shared_ptr<asset::Cubemap>& cubemap);
    [[nodiscard]] const std::shared_ptr<asset::Cubemap>& get_skybox_cubemap() const;

    void setup_test_triangle();

    void draw_cube(const glm::vec3& size);
    void draw_sphere(float radius);

    void            render_cube();
    RenderSettings& get_render_settings();

    [[nodiscard]] ecs::Entity get_camera() const;
    void                      set_camera(ecs::Entity camera_entity);

private:
    ecs::Entity m_camera{ecs::MAX_ENTITIES};

    std::shared_ptr<Framebuffer> m_output_framebuffer;
    std::shared_ptr<Framebuffer> m_g_buffer;

    glm::mat4 m_view_mat{glm::mat4(1.0f)};
    glm::mat4 m_proj_mat{glm::mat4(1.0f)};

    std::shared_ptr<asset::Cubemap> m_skybox_cubemap{};

    std::unique_ptr<asset::Shader> m_geometry_shader{};
    std::unique_ptr<asset::Shader> m_lighting_shader{};
    std::unique_ptr<asset::Shader> m_light_mesh_shader{};
    std::unique_ptr<asset::Model>  m_default_model{};

    std::unique_ptr<asset::Shader>          m_collider_shader{};

    RenderSettings m_settings{};

    unsigned int m_msaa_framebuffer{0};
    void         init_msaa();

    std::vector<ecs::Entity> m_curr_lights{};

    uint32_t m_light_cube_vao{0};
    uint32_t m_light_cube_vbo{0};

};
}
