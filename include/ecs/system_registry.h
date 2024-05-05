// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include <unordered_map>

namespace cgx::ecs
{
class System;

class ECSManager;

class SystemRegistry
{
public:
    explicit SystemRegistry(ECSManager* ecs_manager);
    ~SystemRegistry();

    template<typename T>
    std::shared_ptr<T> register_system()
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(m_systems.find(type_name) == m_systems.end(), "Registering system more than once.");

        auto system = std::make_shared<T>(m_ecs_manager);
        m_systems.insert({type_name, system});
        return system;
    }

    template<typename T>
    void set_signature(Signature signature)
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(m_systems.find(type_name) != m_systems.end(), "System used before being registered.");

        m_signatures.insert({type_name, signature});
    }

    void frame_update(float dt);
    void fixed_update(float fixed_dt);
    void on_entity_released(Entity entity) const;
    void on_entity_updated(Entity entity, Signature entitySignature);

private:
    ECSManager* m_ecs_manager;

    std::unordered_map<const char*, Signature>               m_signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
};
}
