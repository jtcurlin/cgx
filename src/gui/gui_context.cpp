// Copyright © 2024 Jacob Curlin

#include "gui/gui_context.h"
#include "ecs/ecs_manager.h"
#include "core/window_manager.h"
#include "asset/asset_manager.h"
#include "render/render_system.h"
#include "scene/scene_manager.h"
#include "core/systems/time_system.h"

namespace cgx::gui
{
GUIContext::GUIContext(
    asset::AssetManager*  asset_manager,
    ecs::ECSManager*      ecs_manager,
    render::RenderSystem* render_system,
    scene::SceneManager*  scene_manager,
    core::WindowManager*  window_manager,
    core::TimeSystem*     time_system)
    : m_asset_manager(asset_manager)
    , m_ecs_manager(ecs_manager)
    , m_render_system(render_system)
    , m_scene_manager(scene_manager)
    , m_window_manager(window_manager)
    , m_time_system(time_system) {}

GUIContext::~GUIContext() = default;

asset::AssetManager* GUIContext::get_asset_manager() const
{
    CGX_ASSERT(m_asset_manager, "attempt to retreive invalid asset manager ptr");
    return m_asset_manager;
}

ecs::ECSManager* GUIContext::get_ecs_manager() const
{
    return m_ecs_manager;
}

render::RenderSystem* GUIContext::get_render_system() const
{
    CGX_ASSERT(m_render_system, "attempt to retreive invalid render system ptr");
    return m_render_system;
}

scene::SceneManager* GUIContext::get_scene_manager() const
{
    CGX_ASSERT(m_scene_manager, "attempt to retreive invalid scene manager ptr");
    return m_scene_manager;
}

core::WindowManager* GUIContext::get_window_manager() const
{
    CGX_ASSERT(m_scene_manager, "attempt to retreive invalid window manager ptr");
    return m_window_manager;
}

core::TimeSystem* GUIContext::get_time_system() const
{
    CGX_ASSERT(m_scene_manager, "attempt to retreive invalid time sytsem ptr");
    return m_time_system;
}

void GUIContext::set_item_to_birth(core::Item* item)
{
    m_item_to_birth = item;
}

void GUIContext::set_item_to_rename(core::Item* item)
{
    m_item_to_rename = item;
}

void GUIContext::set_item_to_inspect(core::Item* item)
{
    m_item_to_inspect = item;
}

core::Item* GUIContext::get_item_to_birth() const
{
    return m_item_to_birth;
}

core::Item* GUIContext::get_item_to_rename() const
{
    return m_item_to_rename;
}

core::Item* GUIContext::get_item_to_inspect() const
{
    return m_item_to_inspect;
}

void GUIContext::activate_file_import_dialog(const FileImportDialogConfig& config)
{
    m_file_import_config = config;
    m_file_import_active = true;
}

void GUIContext::activate_text_input_dialog(const TextInputDialogConfig& config)
{
    m_text_input_config = config;
    m_text_input_active = true;
}

void GUIContext::deactivate_file_import_dialog()
{
    m_file_import_config = FileImportDialogConfig{};
    m_file_import_active = false;
}

void GUIContext::deactivate_text_input_dialog()
{
    m_text_input_config = TextInputDialogConfig{};
    m_text_input_active = false;
}

bool GUIContext::is_file_import_active() const
{
    return m_file_import_active;
}

bool GUIContext::is_text_input_active() const
{
    return m_text_input_active;
}

const FileImportDialogConfig& GUIContext::get_file_import_config() const
{
    return m_file_import_config;
}

const TextInputDialogConfig& GUIContext::get_text_input_dialog_config() const
{
    return m_text_input_config;
}


}
