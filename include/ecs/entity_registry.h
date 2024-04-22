// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include <array>
#include <bitset>
#include <queue>
#include <vector>

namespace cgx::ecs
{
class EntityRegistry
{
public:
    EntityRegistry();
    ~EntityRegistry();

    Entity acquire_entity();
    void   release_entity(Entity entity);

    void      set_signature(Entity entity, Signature signature);
    Signature get_signature(Entity entity);

    [[nodiscard]] std::vector<Entity> get_active_entities() const;

private:
    std::queue<Entity>                  m_available_entities{};
    std::array<Signature, MAX_ENTITIES> m_signatures{};

    uint32_t m_active_entity_count{0};
};
}
