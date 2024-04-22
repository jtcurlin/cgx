// Copyright © 2024 Jacob Curlin

// Implements an interface for managing components within the ECS, allowing for registration 
// of component types and management of each type's corresponding component array. 

#pragma once

#include "ecs/common.h"
#include "ecs/component_array.h"
#include "event_handler.h"
#include "utility/logging.h"

#include <unordered_map>

namespace cgx::ecs
{
class ComponentRegistry
{
public:
    ComponentRegistry();
    ~ComponentRegistry();

    template<typename T>
    void register_component()
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(
            m_component_types.find(type_name) == m_component_types.end(),
            "Registering component type more than once.");

        ComponentType type_id = m_next_component_type;
        m_component_types.insert({type_name, type_id});

        auto component_array = std::make_shared<ComponentArray<T>>();

        m_component_arrays.insert({type_name, component_array});

        m_type_id_to_arrays.insert({type_id, component_array});

        ++m_next_component_type;
    }

    template<typename T>
    ComponentType get_component_type()
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(
            m_component_types.find(type_name) != m_component_types.end(),
            "Component not registered before use.");

        return m_component_types[type_name];
    }

    template<typename T>
    void add_component(Entity entity, T component)
    {
        get_component_array<T>()->insert_data(entity, component);
    }

    template<typename T>
    void remove_component(Entity entity)
    {
        get_component_array<T>()->remove_data(entity);
    }

    template<typename T>
    T& get_component(Entity entity)
    {
        return get_component_array<T>()->get_data(entity);
    }

    void on_entity_released(Entity entity) const;

private:
    std::unordered_map<const char*, ComponentType>                      m_component_types{};
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>>   m_component_arrays{};
    std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_type_id_to_arrays{};
    ComponentType                                                       m_next_component_type{};

    template<typename T>
    std::shared_ptr<ComponentArray<T>> get_component_array()
    {
        const char* type_name = typeid(T).name();

        CGX_ASSERT(
            m_component_types.find(type_name) != m_component_types.end(),
            "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(m_component_arrays[type_name]);
    }
};
}
