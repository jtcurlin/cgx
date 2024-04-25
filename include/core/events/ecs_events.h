// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/event.h"

namespace cgx::core::event::entity
{
constexpr EventId ACQUIRED = "event::entity::ACQUIRED"_hash;
constexpr EventId RELEASED = "event::entity::RELEASED"_hash;

constexpr ParamId ID = "event::entity::ID"_hash;
}

namespace cgx::core::event::component
{
constexpr EventId ADDED     = "event::component::CREATED"_hash;
constexpr EventId REMOVED   = "event::component::REMOVED"_hash;
constexpr EventId MODIFIED   = "event::component::MODIFIED"_hash;

constexpr ParamId TYPE      = "event::component::TYPE"_hash;
constexpr ParamId ENTITY_ID = "event::component::ENTITY_ID"_hash;
}

namespace cgx::core::event::component::hierarchy
{
constexpr EventId PARENT_UPDATE = "event::component::hierarchy::PARENT_UPDATE"_hash;
constexpr ParamId OLD_PARENT    = "event::component::hierarchy::OLD_PARENT"_hash;
constexpr ParamId NEW_PARENT    = "event::component::hierarchy::NEW_PARENT"_hash;
constexpr ParamId CHILD         = "event::component::hierarchy::CHILD"_hash;
}

namespace cgx::core::event::system
{
    // todo
}