// Copyright © 2024 Jacob Curlin

#include "ecs/component_registry.h"

namespace cgx::ecs
{
void ComponentRegistry::entity_destroyed(Entity entity)
{
    for (auto const& pair : m_component_arrays) {
        auto const& component_array = pair.second;

        component_array->entity_destroyed(entity);
    }
}
}
