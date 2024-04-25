// Copyright © 2024 Jacob Curlin

#pragma once
#include "core/event.h"

namespace cgx::ecs::event::scene::node
{
constexpr EventId ADDED   = "event::scene::node::ADDED"_hash;
constexpr EventId REMOVED = "event::scene::node::REMOVED"_hash;

constexpr ParamId ID            = "event::scene::node::ID"_hash;
constexpr ParamId TAG           = "event::scene::node::TAG"_hash;
constexpr ParamId INTERNAL_PATH = "event::scene::node::INTERNAL_PATH"_hash;
constexpr ParamId EXTERNAL_PATH = "event::scene::node::EXTERNAL_PATH"_hash;
}
