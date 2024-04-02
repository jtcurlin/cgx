// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "ecs/common.h"
#include "ecs/component_manager.h"
#include "ecs/system.h"
#include <unordered_map>

namespace cgx::ecs
{

    class SystemManager
    {
    public:
        SystemManager(std::shared_ptr<cgx::ecs::ComponentManager> component_registry);

        template<typename T>
        std::shared_ptr<T> RegisterSystem()
        {
            const char* type_name = typeid(T).name();

            CGX_ASSERT(m_systems.find(type_name) == m_systems.end(), "Registering system more than once.");

            auto system = std::make_shared<T>(m_component_registry);
            m_systems.insert({type_name, system});
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature)
        {
            const char* type_name = typeid(T).name();

            CGX_ASSERT(m_systems.find(type_name) != m_systems.end(), "System used before being registered.");

            m_signatures.insert({type_name, signature});
        }

        void Update(float dt)
        {
            for (auto& [type_id, system] : m_systems)
            {
                system->Update(dt);
            }
        }

        void EntityDestroyed(Entity entity);

        void EntitySignatureChanged(Entity entity, Signature entitySignature);

    private:
        std::shared_ptr<cgx::ecs::ComponentManager> m_component_registry;

        std::unordered_map<const char*, Signature> m_signatures{};
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
    };

}