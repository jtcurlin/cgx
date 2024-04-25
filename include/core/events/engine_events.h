// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/event.h"

namespace cgx::core::event::camera
{
constexpr EventId MOVE      = "event::camera::MOVE"_hash;
constexpr ParamId DIRECTION = "event::camera::direction"_hash;
}
