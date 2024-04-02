// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include <set>

#include "ecs/component_manager.h"

namespace cgx::ecs
{
    class System
    {
    public:
        System(std::shared_ptr<cgx::ecs::ComponentManager> component_registry)
            : m_component_registry(component_registry) {}

        virtual ~System() = default;

        virtual void Update(float dt) = 0;

        template<typename T>
        T& GetComponent(Entity entity)
        {
            return m_component_registry->GetComponent<T>(entity);
        }

        // system manager updates this vector with entities possessing components
        // that this sytem is registered for
        std::set<Entity> m_entities; 
    protected:
        std::shared_ptr<cgx::ecs::ComponentManager> m_component_registry;

    };
}