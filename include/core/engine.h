// Copyright © 2024 Jacob Curlin

#pragma once

#define GL_SILENCE_DEPRECATION

#include "core/common.h"
#include "utility/paths.h"

#include <filesystem>

namespace cgx::ecs
{
class EntityRegistry;
class ComponentRegistry;
class SystemRegistry;
}

namespace cgx::scene
{
class SceneManager;
}

namespace cgx::render
{
class RenderSystem;
}

namespace cgx::gui
{
class GUIContext;
class ImGuiManager;
}

namespace cgx::input
{
class InputManager;
}

namespace cgx::asset
{
class AssetManager;
}

namespace cgx::core
{
class TimeSystem;
class WindowManager;
}

namespace cgx::core
{
struct EngineSettings
{
    std::string window_title{"CGX"};

    uint32_t window_width{1920};
    uint32_t window_height{1080};
    uint32_t render_width{1280};
    uint32_t render_height{720};

    std::filesystem::path shader_dir{SHADERS_DIRECTORY};
    std::filesystem::path asset_dir{ASSETS_DIRECTORY};
    std::filesystem::path font_dir{FONTS_DIRECTORY};
};

struct PathConfig
{
    std::filesystem::path shader_dir{SHADERS_DIRECTORY};
    std::filesystem::path asset_dir{ASSETS_DIRECTORY};
    std::filesystem::path font_dir{FONTS_DIRECTORY};
};

class Engine
{
public:
    Engine();
    virtual ~Engine();

    void run();

protected:
    virtual void initialize();
    virtual void update();
    virtual void render();
    // virtual void shutdown();

    void setup_ecs();
    void setup_gui();
    void setup_engine_events();

    EngineSettings m_settings{};

    bool                        m_is_running{false};
    std::shared_ptr<TimeSystem> m_time_system;

    std::shared_ptr<ecs::EntityRegistry>    m_entity_registry;
    std::shared_ptr<ecs::ComponentRegistry> m_component_registry;
    std::shared_ptr<ecs::SystemRegistry>    m_system_registry;

    std::shared_ptr<WindowManager>       m_window_manager;
    std::shared_ptr<input::InputManager> m_input_manager;

    std::shared_ptr<scene::SceneManager> m_scene_manager;

    std::shared_ptr<render::RenderSystem> m_render_system;

    std::shared_ptr<asset::AssetManager> m_asset_manager;

    std::shared_ptr<gui::GUIContext>   m_gui_context;
    std::unique_ptr<gui::ImGuiManager> m_imgui_manager;
};
}
