// Copyright © 2024 Jacob Curlin

#pragma once

#include "utility/logging.h"

#include <bitset>
#include <cstdint>

namespace cgx::ecs
{
using Entity        = std::uint32_t;
using ComponentType = std::uint8_t;

static const Entity        MAX_ENTITIES   = 5000;
static const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;
}
