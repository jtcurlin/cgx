// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/event.h"
#include "ecs/event_handler.h"

namespace cgx::events::engine
{
constexpr ecs::EventId QUIT                   = "Events::Engine::QUIT"_hash;
constexpr ecs::EventId ENABLE_CAMERA_CONTROL  = "Events::Engine::ENABLE_CAMERA_CONTROL"_hash;
constexpr ecs::EventId DISABLE_CAMERA_CONTROL = "Events::Engine::DISABLE_CAMERA_CONTROL"_hash;
}

namespace cgx::events::camera
{
constexpr ecs::EventId MOVE = "cgx::events::camera::move"_hash;

namespace cgx::events::movement
{
constexpr ecs::ParamId DIRECTION = "cgx::events::camera::move::direction"_hash;
}}

namespace cgx::events::entity
{
constexpr ecs::EventId ACQUIRED   = "cgx::events::ecs::ENTITY_CREATED"_hash;
constexpr ecs::EventId RELEASED = "cgx::events::ecs::ENTITY_DESTROYED"_hash;
constexpr ecs::ParamId ID        = "cgx::events::ecs::ENTITY_ID"_hash;
}

namespace cgx::events::component
{
constexpr ecs::EventId ADDED = "cgx::events::component::CREATED"_hash;
constexpr ecs::EventId REMOVED = "cgx::events::component::DESTROYED"_hash;
constexpr ecs::EventId UPDATED = "cgx::events::component::UPDATED"_hash;
constexpr ecs::ParamId TYPE = "cgx::Events::component::TYPE"_hash;
constexpr ecs::ParamId ENTITY_ID = "cgx::events::component::ENTITY_ID"_hash;
}

namespace cgx::events::hierarchy
{
constexpr ecs::EventId PARENT_UPDATE = "cgx::events::component::PARENT_UPDATE"_hash;
constexpr ecs::ParamId OLD_PARENT = "cgx::events::component::OLD_PARENT"_hash;
constexpr ecs::ParamId NEW_PARENT = "cgx::events::component::NEW_PARENT"_hash;
constexpr ecs::ParamId CHILD = "cgx::events::component::CHILD"_hash;
}

namespace cgx::events::asset
{
constexpr ecs::EventId ASSET_ADDED         = "cgx::events::asset::ASSET_ADDED"_hash;
constexpr ecs::EventId ASSET_REMOVED       = "cgx::events::asset::ASSET_REMOVED"_hash;
constexpr ecs::EventId ASSET_MODIFIED      = "cgx::events::asset::ASSET_REMOVED"_hash;
constexpr ecs::EventId IMPORTER_REGISTERED = "cgx::events::asset::IMPORTER_REGISTERED"_hash;

constexpr ecs::ParamId ASSET_ID                      = "cgx::events::asset::ASSET_ID"_hash;
constexpr ecs::ParamId ASSET_NAME                    = "cgx::events:asset::ASSET_NAME"_hash;
constexpr ecs::ParamId ASSET_PATH                    = "cgx::events:asset::ASSET_PATH"_hash;
constexpr ecs::ParamId ASSET_TYPE                    = "cgx::events:asset::ASSET_TYPE"_hash;
constexpr ecs::ParamId IMPORTER_LABEL                = "cgx::events::asset::IMPORTER_LABEL"_hash;
constexpr ecs::ParamId IMPORTER_SUPPORTED_EXTENSIONS = "cgx::events::asset::IMPORTER_SUPPORTED_EXTENSIONS"_hash;
}

namespace cgx::ecs::scene
{
constexpr EventId NODE_ADDED   = "cgx::event::scene::NODE_ADDED"_hash;
constexpr EventId NODE_REMOVED = "cgx::event::scene::NODE_REMOVED"_hash;

constexpr ParamId NODE_LABEL     = "cgx::event::scene::NODE_LABEL"_hash;
constexpr ParamId NODE_PATH      = "cgx::event::scene::NODE_PATH"_hash;
constexpr ParamId NODE_ENTITY_ID = "cgx::event::scene::NODE_ENTITY_ID"_hash;
}

namespace cgx::ecs::gui::scene
{
constexpr EventId NODE_ADDED   = "cgx::event::gui::scene::NODE_ADDED"_hash;
constexpr EventId NODE_REMOVED = "cgx::event::gui::scene::NODE_REMOVED"_hash;

constexpr ParamId NODE_LABEL = "cgx::event::gui::scene::NODE_LABEL"_hash;
constexpr ParamId NODE_PATH  = "cgx::event::gui::scene::NODE_LABEL"_hash;
}
