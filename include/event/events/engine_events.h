// Copyright © 2024 Jacob Curlin

#pragma once

#include "event/event.h"
#include "event/event_handler.h"

namespace cgx::events::engine
{
constexpr event::EventId QUIT                   = "Events::Engine::QUIT"_hash;
constexpr event::EventId ENABLE_CAMERA_CONTROL  = "Events::Engine::ENABLE_CAMERA_CONTROL"_hash;
constexpr event::EventId DISABLE_CAMERA_CONTROL = "Events::Engine::DISABLE_CAMERA_CONTROL"_hash;
}

namespace cgx::events::camera
{
constexpr event::EventId MOVE = "cgx::events::camera::move"_hash;

namespace cgx::events::movement
{
constexpr event::ParamId DIRECTION = "cgx::events::camera::move::direction"_hash;
}}

namespace cgx::events::ecs
{
constexpr event::EventId ENTITY_CREATED   = "cgx::events::ecs::ENTITY_CREATED"_hash;
constexpr event::EventId ENTITY_DESTROYED = "cgx::events::ecs::ENTITY_DESTROYED"_hash;
constexpr event::ParamId ENTITY_ID        = "cgx::events::ecs::ENTITY_ID"_hash;
}

namespace cgx::events::component
{
constexpr event::EventId ADDED = "cgx::events::component::CREATED"_hash;
constexpr event::EventId REMOVED = "cgx::events::component::DESTROYED"_hash;
constexpr event::EventId UPDATED = "cgx::events::component::UPDATED"_hash;
constexpr event::ParamId TYPE = "cgx::Events::component::TYPE"_hash;
constexpr event::ParamId ENTITY_ID = "cgx::events::component::ENTITY_ID"_hash;
}

namespace cgx::events::asset
{
constexpr event::EventId ASSET_ADDED         = "cgx::events::asset::ASSET_ADDED"_hash;
constexpr event::EventId ASSET_REMOVED       = "cgx::events::asset::ASSET_REMOVED"_hash;
constexpr event::EventId ASSET_MODIFIED      = "cgx::events::asset::ASSET_REMOVED"_hash;
constexpr event::EventId IMPORTER_REGISTERED = "cgx::events::asset::IMPORTER_REGISTERED"_hash;

constexpr event::ParamId ASSET_ID                      = "cgx::events::asset::ASSET_ID"_hash;
constexpr event::ParamId ASSET_NAME                    = "cgx::events:asset::ASSET_NAME"_hash;
constexpr event::ParamId ASSET_PATH                    = "cgx::events:asset::ASSET_PATH"_hash;
constexpr event::ParamId ASSET_TYPE                    = "cgx::events:asset::ASSET_TYPE"_hash;
constexpr event::ParamId IMPORTER_LABEL                = "cgx::events::asset::IMPORTER_LABEL"_hash;
constexpr event::ParamId IMPORTER_SUPPORTED_EXTENSIONS = "cgx::events::asset::IMPORTER_SUPPORTED_EXTENSIONS"_hash;
}

namespace cgx::event::scene
{
constexpr EventId NODE_ADDED   = "cgx::event::scene::NODE_ADDED"_hash;
constexpr EventId NODE_REMOVED = "cgx::event::scene::NODE_REMOVED"_hash;

constexpr ParamId NODE_LABEL     = "cgx::event::scene::NODE_LABEL"_hash;
constexpr ParamId NODE_PATH      = "cgx::event::scene::NODE_PATH"_hash;
constexpr ParamId NODE_ENTITY_ID = "cgx::event::scene::NODE_ENTITY_ID"_hash;
}

namespace cgx::event::gui::scene
{
constexpr EventId NODE_ADDED   = "cgx::event::gui::scene::NODE_ADDED"_hash;
constexpr EventId NODE_REMOVED = "cgx::event::gui::scene::NODE_REMOVED"_hash;

constexpr ParamId NODE_LABEL = "cgx::event::gui::scene::NODE_LABEL"_hash;
constexpr ParamId NODE_PATH  = "cgx::event::gui::scene::NODE_LABEL"_hash;
}
