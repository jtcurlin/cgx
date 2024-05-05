// Copyright © 2024 Jacob Curlin

#pragma once

// core
#include "core/common.h"
#include "core/engine.h"
#include "core/hierarchy.h"
#include "core/item.h"
#include "core/input_manager.h"
#include "core/systems/physics_system.h"
#include "core/systems/time_system.h"
#include "core/window_manager.h"

// asset
#include "asset/asset.h"
#include "asset/asset_manager.h"
#include "asset/cubemap.h"
#include "asset/material.h"
#include "asset/mesh.h"
#include "asset/model.h"
#include "asset/shader.h"
#include "asset/texture.h"
#include "asset/import/asset_importer.h"
#include "asset/import/asset_importer_image.h"
#include "asset/import/asset_importer_obj.h"

// event & entity component system
#include "ecs/common.h"
#include "ecs/component_array.h"
#include "ecs/component_registry.h"
#include "ecs/entity_registry.h"
#include "ecs/system.h"
#include "ecs/system_registry.h"
#include "core/event.h"
#include "core/event_handler.h"

// gui
#include "gui/gui_context.h"
#include "gui/imgui_manager.h"
#include "gui/imgui_panel.h"
#include "gui/panels/asset_panel.h"
#include "gui/panels/profiler_panel.h"
#include "gui/panels/properties_panel.h"
#include "gui/panels/render_settings_panel.h"
#include "gui/panels/scene_panel.h"
#include "gui/panels/viewport_panel.h"

// render
#include "render/framebuffer.h"
#include "render/render_system.h"

// scene
#include "scene/node.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"

// utility
#include "utility/error.h"
#include "utility/logging.h"
#include "utility/math.h"
#include "utility/paths.h"
#include "utility/primitive_mesh.h"


