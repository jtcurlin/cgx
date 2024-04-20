// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"
#include "ecs/component_registry.h"

#include "event/event_handler.h"
#include "event/events/engine_events.h"

#include <set>

namespace cgx::ecs
{
class System
{
public:
    explicit System(const std::shared_ptr<ComponentRegistry>& component_registry)
        : m_component_registry(component_registry)
    {
        event::EventHandler::get_instance().AddListener(events::component::UPDATED, [this](event::Event& event) {
            this->on_component_updated(event.get_param<Entity>(events::component::ENTITY_ID));
        });
    }

    virtual ~System() = default;

    virtual void update(float dt) = 0;
    virtual void on_entity_added(Entity entity) = 0;
    virtual void on_entity_removed(Entity entity) = 0;

    virtual void on_component_updated(Entity entity) = 0;

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
