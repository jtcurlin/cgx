// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/window_manager.h"
#include "asset/asset_manager.h"
#include "render/render_system.h"
#include "scene/scene_manager.h"
#include "core/time_system.h"

namespace cgx::gui
{
class GUIContext
{
public:
    GUIContext(
        const std::shared_ptr<asset::AssetManager>&  asset_manager,
        const std::shared_ptr<render::RenderSystem>& render_system,
        const std::shared_ptr<scene::SceneManager>&  scene_manager,
        const std::shared_ptr<core::WindowManager>&  window_manager,
        const std::shared_ptr<core::TimeSystem>&     time_system);
    ~GUIContext();

    [[nodiscard]] std::shared_ptr<asset::AssetManager>  get_asset_manager() const;
    [[nodiscard]] std::shared_ptr<render::RenderSystem> get_render_system() const;
    [[nodiscard]] std::shared_ptr<scene::SceneManager>  get_scene_manager() const;
    [[nodiscard]] std::shared_ptr<core::WindowManager>  get_window_manager() const;
    [[nodiscard]] std::shared_ptr<core::TimeSystem>     get_time_system() const;

    [[nodiscard]] core::Item* get_selected_item() const;
    void                      set_selected_item(core::Item* item);

private:
    std::weak_ptr<asset::AssetManager>  m_asset_manager;
    std::weak_ptr<render::RenderSystem> m_render_system;
    std::weak_ptr<scene::SceneManager>  m_scene_manager;
    std::weak_ptr<core::WindowManager>  m_window_manager;
    std::weak_ptr<core::TimeSystem>     m_time_system;

    std::filesystem::path m_data_directory;

    core::Item* m_selected_item;
};
}
