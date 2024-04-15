// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include "ecs/component_registry.h"
#include <set>


namespace cgx::ecs
{
class System
{
public:
    explicit System(const std::shared_ptr<ComponentRegistry>& component_registry)
        : m_component_registry(component_registry) {}

    virtual ~System() = default;

    virtual void update(float dt) = 0;

    template<typename T>
    T& GetComponent(const Entity entity)
    {
        return m_component_registry->get_component<T>(entity);
    }

    std::set<Entity> m_entities;

protected:
    std::shared_ptr<ComponentRegistry> m_component_registry;
};
}
