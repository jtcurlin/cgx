// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/cubemap.h"
#include "core/components/camera.h"
#include "ecs/system.h"

#include "render/camera.h"
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

    bool     m_render_test_enabled{false};
    uint32_t m_render_test_shader{};
    uint32_t m_render_test_vao{};
};

class RenderSystem : public ecs::System
{
public:
    explicit RenderSystem(ecs::ECSManager* ecs_manager);
    ~RenderSystem() override;

    void initialize();
    void render();

    void update(float dt) override;
    void on_entity_added(ecs::Entity entity) override {}
    void on_entity_removed(ecs::Entity entity) override {}

    void draw_skybox() const;

    const std::shared_ptr<Framebuffer>& getFramebuffer();

    void set_skybox_cubemap(const std::shared_ptr<asset::Cubemap>& cubemap);
    [[nodiscard]] const std::shared_ptr<asset::Cubemap>& get_skybox_cubemap() const;

    void setup_test_triangle();

    RenderSettings& get_render_settings();

    [[nodiscard]] ecs::Entity get_camera() const;
    void set_camera(ecs::Entity camera_entity);

private:
    ecs::Entity m_camera{ecs::MAX_ENTITIES};
    std::shared_ptr<Framebuffer> m_framebuffer;

    glm::mat4 m_view_mat{glm::mat4(1.0f)};
    glm::mat4 m_proj_mat{glm::mat4(1.0f)};

    std::shared_ptr<asset::Cubemap> m_skybox_cubemap{};

    RenderSettings m_settings;

    unsigned int m_msaa_framebuffer{0};
    void         init_msaa();
};
}
