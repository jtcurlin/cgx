// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include "ecs/ecs_manager.h"

#include <set>

namespace cgx::ecs
{

class System
{
public:
    explicit System(ECSManager* ecs_manager)
        : m_ecs_manager(ecs_manager)
    {
    }

    virtual ~System() = default;

    virtual void update(float dt) = 0;
    virtual void on_entity_added(Entity entity) = 0;
    virtual void on_entity_removed(Entity entity) = 0;

    template<typename T>
    T& get_component(const Entity entity)
    {
        return m_ecs_manager->get_component<T>(entity);
    }

    std::set<Entity> m_entities;

protected:
    ECSManager* m_ecs_manager;
};
}
