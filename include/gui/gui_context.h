// Copyright © 2024 Jacob Curlin

#pragma once
#include <string>
#include <functional>

namespace cgx::core
{
class WindowManager;
class TimeSystem;
class Item;
}

namespace cgx::render
{
class RenderSystem;
}

namespace cgx::asset
{
class AssetManager;
}

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::scene
{
class SceneManager;
}


namespace cgx::gui
{

struct FileImportDialogConfig
{
    std::string title{"Import File"};
    std::string filters{};

    std::function<void(const std::string&)> callback{};
};

struct TextInputDialogConfig
{
    std::string title{"Enter Text"};

    std::function<void(const std::string&)> callback{};
};

class GUIContext
{
public:
    GUIContext(
        asset::AssetManager*  asset_manager,
        ecs::ECSManager*      ecs_manager,
        render::RenderSystem* render_system,
        scene::SceneManager*  scene_manager,
        core::WindowManager*  window_manager,
        core::TimeSystem*     time_system);
    ~GUIContext();

    [[nodiscard]] asset::AssetManager*  get_asset_manager() const;
    [[nodiscard]] ecs::ECSManager*      get_ecs_manager() const;
    [[nodiscard]] render::RenderSystem* get_render_system() const;
    [[nodiscard]] scene::SceneManager*  get_scene_manager() const;
    [[nodiscard]] core::WindowManager*  get_window_manager() const;
    [[nodiscard]] core::TimeSystem*     get_time_system() const;

    void set_item_to_birth(core::Item* item);
    void set_item_to_rename(core::Item* item);
    void set_item_to_inspect(core::Item* item);

    [[nodiscard]] core::Item* get_item_to_birth() const;
    [[nodiscard]] core::Item* get_item_to_rename() const;
    [[nodiscard]] core::Item* get_item_to_inspect() const;

private:
    asset::AssetManager*  m_asset_manager{nullptr};
    ecs::ECSManager*      m_ecs_manager{nullptr};
    render::RenderSystem* m_render_system{nullptr};
    scene::SceneManager*  m_scene_manager{nullptr};
    core::WindowManager*  m_window_manager{nullptr};
    core::TimeSystem*     m_time_system{nullptr};

    core::Item* m_item_to_inspect{nullptr};
    core::Item* m_item_to_birth{nullptr};
    core::Item* m_item_to_rename{nullptr};
    core::Item* m_item_to_display_popup{nullptr};

    std::string m_string_buffer{};
    char        m_char_buffer[256]{};
};
}
