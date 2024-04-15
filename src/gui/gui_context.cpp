// Copyright © 2024 Jacob Curlin

#include "gui/gui_context.h"

namespace cgx::gui
{
GUIContext::GUIContext(
    const std::shared_ptr<asset::AssetManager>&  asset_manager,
    const std::shared_ptr<render::RenderSystem>& render_system,
    const std::shared_ptr<scene::SceneManager>&  scene_manager,
    const std::shared_ptr<core::WindowManager>&  window_manager,
    const std::shared_ptr<core::TimeSystem>&     time_system)
    : m_asset_manager(asset_manager)
    , m_render_system(render_system)
    , m_scene_manager(scene_manager)
    , m_window_manager(window_manager)
    , m_time_system(time_system)
    , m_selected_item{nullptr} {}

GUIContext::~GUIContext() = default;

std::shared_ptr<asset::AssetManager> GUIContext::get_asset_manager() const
{
    auto asset_manager = m_asset_manager.lock();
    CGX_ASSERT(asset_manager, "failed to obtain valid asset manager instance");
    return asset_manager;
}

std::shared_ptr<render::RenderSystem> GUIContext::get_render_system() const
{
    auto render_system = m_render_system.lock();
    CGX_ASSERT(render_system, "failed to obtain valid render system instance");
    return render_system;
}

std::shared_ptr<scene::SceneManager> GUIContext::get_scene_manager() const
{
    auto scene_manager = m_scene_manager.lock();
    CGX_ASSERT(scene_manager, "failed to obtain valid scene_manager instance");
    return scene_manager;
}

std::shared_ptr<core::WindowManager> GUIContext::get_window_manager() const
{
    auto window_manager = m_window_manager.lock();
    CGX_ASSERT(window_manager, "failed to obtain valid window manager instance");
    return window_manager;
}

std::shared_ptr<core::TimeSystem> GUIContext::get_time_system() const
{
    auto time_system = m_time_system.lock();
    CGX_ASSERT(time_system, "failed to obtain valid window manager instance");
    return time_system;
}


void GUIContext::set_selected_item(core::Item* item)
{
    m_selected_item = item;
}

core::Item* GUIContext::get_selected_item() const
{
    return m_selected_item;
}
}
