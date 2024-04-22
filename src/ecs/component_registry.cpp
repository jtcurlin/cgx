// Copyright © 2024 Jacob Curlin

#include "ecs/component_registry.h"

namespace cgx::ecs
{
ComponentRegistry::ComponentRegistry() = default;
ComponentRegistry::~ComponentRegistry() = default;

void ComponentRegistry::on_entity_released(const Entity entity) const
{
    for (auto const& pair : m_component_arrays) {
        auto const& component_array = pair.second;

        component_array->entity_destroyed(entity);
    }
}
}
