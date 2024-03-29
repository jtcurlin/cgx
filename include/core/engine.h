// Copyright © 2024 Jacob Curlin

#pragma once

#define GL_SILENCE_DEPRECATION

#include "core/common.h"
#include "core/window_manager.h"
#include "core/time.h"
#include "core/physics_system.h"

#include "input/input_manager.h"

#include "ecs/ecs_manager.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/rigid_body.h"
#include "ecs/components/render_component.h"
#include "ecs/components/light_component.h"

#include "gui/imgui_manager.h"
#include "gui/imgui_render_window.h"
#include "gui/imgui_ecs_window.h"
#include "gui/imgui_performance_window.h"
#include "gui/imgui_render_settings_window.h"
#include "gui/imgui_resource_manager_window.h"
#include "gui/resource_manager_adapter.h"

#include "resource/resource.h"
#include "resource/resource_manager.h"
#include "resource/material.h"
#include "resource/model.h"
#include "resource/mesh.h"
#include "resource/shader.h"
#include "resource/texture.h"
#include "resource/import/resource_importer.h"
#include "resource/import/resource_importer_image.h"
#include "resource/import/resource_importer_obj.h"

#include "render/camera.h"
#include "render/framebuffer.h"

#include "geometry/primitive_mesh.h"

#include "utility/logging.h"
#include "utility/paths.h"      // cmake-configured shader/asset paths
#include "utility/math.h"

#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace cgx::core {

    struct EngineSettings {
        uint32_t window_width = 1920;
        uint32_t window_height = 1080;
        uint32_t render_width = 1280;
        uint32_t render_height = 720;

        std::filesystem::path shader_dir = SHADERS_DIRECTORY;
        std::filesystem::path asset_dir = ASSETS_DIRECTORY;
    };

    struct TimeData {
        double delta_time;
        double last_time;
    };

    class Engine {
    public:
        Engine();

        virtual ~Engine() = default;

        void Run();

    protected:
        virtual void Initialize();

        virtual void Update();

        virtual void Render();

        virtual void Shutdown();

        void SetupEngineEvents();

    protected:
        EngineSettings m_settings;
        TimeData m_time_data;

        Time m_time_system;

        // unique
        std::shared_ptr<cgx::core::WindowManager> m_window_manager;
        std::shared_ptr<cgx::input::InputManager> m_input_manager;
        std::unique_ptr<cgx::render::Camera> m_camera;

        std::shared_ptr<cgx::core::PhysicsSystem> m_physics_system;

        std::shared_ptr<cgx::render::Framebuffer> m_framebuffer;

        // shared
        std::shared_ptr<cgx::ecs::ECSManager> m_ecs_manager;

        std::unique_ptr<cgx::gui::ImGuiManager> m_imgui_manager;
        std::unique_ptr<cgx::gui::ImGuiECSWindow> m_imgui_ecs_window;
        std::unique_ptr<cgx::gui::ImGuiRenderWindow> m_imgui_render_window;
        std::unique_ptr<cgx::gui::ImGuiPerformanceWindow> m_imgui_performance_window;
        std::unique_ptr<cgx::gui::ImGuiRenderSettingsWindow> m_imgui_render_settings_window;
        std::unique_ptr<cgx::gui::ImGuiResourceManagerWindow> m_imgui_resource_manager_window;
        std::shared_ptr<cgx::gui::ResourceManagerAdapter> m_resource_manager_adapter;

        std::shared_ptr<cgx::gui::RenderSettings> m_render_settings;

        // std::shared_ptr<cgx::resource::Mesh> m_primitive_plane_mesh;
        // std::shared_ptr<cgx::resource::Shader> m_primitive_shader;

        bool m_is_running;
        bool m_imgui_active;
    };

}