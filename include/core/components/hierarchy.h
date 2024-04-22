// Copyright © 2024 Jacob Curlin

#pragma once
#include "ecs/common.h"

namespace cgx::component
{
struct Hierarchy {
    ecs::Entity parent{ecs::MAX_ENTITIES};
    std::vector<ecs::Entity> children{};
    std::vector<ecs::Entity> siblings{};
};
}