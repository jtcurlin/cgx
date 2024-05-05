// Copyright © 2024 Jacob Curlin

#pragma once

#define GL_SILENCE_DEPRECATION

#include "core/common.h"

#include <filesystem>

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::audio
{
class AudioSystem;
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

namespace cgx::asset
{
class AssetManager;
}

namespace cgx::core
{
class TimeSystem;
class WindowManager;
class InputManager;

struct EngineSettings
{
    std::string window_title{"CGX"};

    uint32_t window_width{1920};
    uint32_t window_height{1080};
    uint32_t render_width{1280};
    uint32_t render_height{720};

    std::filesystem::path data_dir{DATA_DIRECTORY};
};

enum class Mode
{
    Game,
    GUI
};

}

namespace cgx::core
{
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

    void setup_gui();
    void setup_engine_events();

    EngineSettings m_settings{};
    bool           m_is_running{false};
    Mode           m_interface_mode{Mode::GUI};
    Mode           m_control_mode{Mode::GUI};

    std::unique_ptr<TimeSystem>           m_time_system;
    std::shared_ptr<WindowManager>        m_window_manager;
    std::unique_ptr<ecs::ECSManager>      m_ecs_manager;
    std::shared_ptr<scene::SceneManager>  m_scene_manager;
    std::shared_ptr<asset::AssetManager>  m_asset_manager;
    std::unique_ptr<gui::GUIContext>      m_gui_context;
    std::unique_ptr<gui::ImGuiManager>    m_imgui_manager;
    std::shared_ptr<render::RenderSystem> m_render_system;
    std::shared_ptr<audio::AudioSystem>   m_audio_system;
};
}
