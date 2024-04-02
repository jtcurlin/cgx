// Copyright © 2024 Jacob Curlin

#include "core/common.h"
#include "ecs/system.h"

#include "render/camera.h"
#include "render/framebuffer.h"

#include <memory>

namespace cgx::ecs { class ComponentManager; }

namespace cgx::render
{
    class RenderSystem : public cgx::ecs::System
    {
    public:
        RenderSystem(std::shared_ptr<cgx::ecs::ComponentManager> component_registry);

        void Initialize();
        void Render();

        void Update(float dt) override;

        std::shared_ptr<cgx::render::Framebuffer> getFramebuffer() { return m_framebuffer; }

    private:
        std::unique_ptr<Camera> m_camera;
        std::shared_ptr<Framebuffer> m_framebuffer;

        uint32_t m_window_width     { 1920 };
        uint32_t m_window_height    { 1080 };
        uint32_t m_render_width     { 1280 };
        uint32_t m_render_height    {  720 };

        bool m_msaa_enabled         { false };
        bool m_skybox_enabled       { false };

        unsigned int m_msaa_framebuffer;
        void InitializeMSAA();

    }; // class RenderSystem

} // namespace cgx::render
