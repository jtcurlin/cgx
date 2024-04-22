// Copyright © 2024 Jacob Curlin

#include "../../include/ecs/event_handler.h"
#include "../../include/ecs/event.h"

namespace cgx::ecs
{
EventHandler::EventHandler() = default;

EventHandler& EventHandler::get_instance()
{
    static EventHandler instance;
    return instance;
}

void EventHandler::add_listener(const EventId event_id, std::function<void(Event&)> const& listener)
{
    m_listeners[event_id].push_back(listener);
}

void EventHandler::send_event(Event& event)
{
    for (auto const& listener : m_listeners[event.get_type()]) {
        listener(event);
    }
}

void EventHandler::send_event(const EventId event_id)
{
    Event event(event_id);

    for (auto const& listener : m_listeners[event_id]) {
        listener(event);
    }
}
}
