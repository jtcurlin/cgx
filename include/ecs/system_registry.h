// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include "ecs/component_registry.h"
#include "event/event_handler.h"
#include "ecs/system.h"
#include <unordered_map>

namespace cgx::ecs
{
class SystemRegistry
{
public:
    explicit SystemRegistry(const std::shared_ptr<ComponentRegistry>& component_registry);
    ~SystemRegistry();

    template<typename T>
    std::shared_ptr<T> register_system()
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(m_systems.find(type_name) == m_systems.end(), "Registering system more than once.");

        auto system = std::make_shared<T>(m_component_registry);
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

    void update(const float dt)
    {
        for (auto& [type_id, system] : m_systems) {
            system->update(dt);
        }
    }

    void entity_destroyed(Entity entity);
    void entity_signature_changed(Entity entity, Signature entitySignature);

private:
    std::shared_ptr<ComponentRegistry> m_component_registry;

    std::unordered_map<const char*, Signature>               m_signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
};
}
