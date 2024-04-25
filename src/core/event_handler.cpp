// Copyright © 2024 Jacob Curlin

#include "core/event_handler.h"
#include "../../include/core/event.h"

namespace cgx::core
{
EventHandler::EventHandler() = default;

EventHandler& EventHandler::get_instance()
{
    static EventHandler instance;
    return instance;
}

void EventHandler::add_listener(const event::EventId event_id, std::function<void(event::Event&)> const& listener)
{
    m_listeners[event_id].push_back(listener);
}

void EventHandler::send_event(event::Event& event)
{
    for (auto const& listener : m_listeners[event.get_type()]) {
        listener(event);
    }
}

void EventHandler::send_event(const event::EventId event_id)
{
    event::Event event(event_id);

    for (auto const& listener : m_listeners[event_id]) {
        listener(event);
    }
}
}
