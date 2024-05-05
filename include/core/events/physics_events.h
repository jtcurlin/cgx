// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/event.h"

namespace cgx::core::event::physics
{
	constexpr EventId COLLISION = "event::physics::COLLISION"_hash;

	constexpr ParamId ENTITY_A = "event::physics::ENTITY_A"_hash;
	constexpr ParamId ENTITY_B = "event::physics::ENTITY_B"_hash;
}
