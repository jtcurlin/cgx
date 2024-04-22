// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/entity_registry.h"
#include "ecs/component_registry.h"
#include "ecs/system_registry.h"

namespace cgx::ecs
{

class ECSManager
{
public:
    ECSManager();
    ~ECSManager();

    void update(float dt) const;

    Entity acquire_entity() const
    {
        return m_entity_registry->acquire_entity();
    }

    void release_entity(const Entity entity) const
    {
        m_system_registry->on_entity_released(entity);
        m_entity_registry->release_entity(entity);
        m_component_registry->on_entity_released(entity);
    }

    template<typename T>
    void register_component() const
    {
        m_component_registry->register_component<T>();
    }

    template<typename T>
    void add_component(const Entity entity, T component)
    {
        m_component_registry->add_component<T>(entity, component);
        CGX_INFO("Scene: Added Component to entity {}", entity);

        auto signature = m_entity_registry->get_signature(entity);
        signature.set(m_component_registry->get_component_type<T>(), true);
        m_entity_registry->set_signature(entity, signature);

        m_system_registry->on_entity_updated(entity, signature);
    }

    template<typename T>
    void remove_component(const Entity entity) const
    {
        m_component_registry->remove_component<T>(entity);

        auto signature = m_entity_registry->get_signature(entity);
        signature.set(m_component_registry->get_component_type<T>(), false);
        m_entity_registry->set_signature(entity, signature);

        m_system_registry->on_entity_updated(entity, signature);
    }

    template<typename T>
    T& get_component(const Entity entity)
    {
        return m_component_registry->get_component<T>(entity);
    }

    template<typename T>
    [[nodiscard]] ComponentType get_component_type() const
    {
        return m_component_registry->get_component_type<T>();
    }

    template<typename T>
    [[nodiscard]] bool has_component(const Entity entity) const
    {
        const auto signature = m_entity_registry->get_signature(entity);
        return signature.test(m_component_registry->get_component_type<T>());
    }

    template<typename T>
    std::shared_ptr<T> register_system()
    {
        return m_system_registry->register_system<T>();
    }

    template<typename T>
    void set_system_signature(const Signature signature) const
    {
        m_system_registry->set_signature<T>(signature);
    }

private:
    std::unique_ptr<EntityRegistry>    m_entity_registry;
    std::unique_ptr<ComponentRegistry> m_component_registry;
    std::unique_ptr<SystemRegistry>    m_system_registry;
};
}
