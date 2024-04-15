// Copyright © 2024 Jacob Curlin

#include "event/event_handler.h"
#include "event/event.h"

namespace cgx::event
{
EventHandler::EventHandler() = default;

EventHandler& EventHandler::get_instance()
{
    static EventHandler instance;
    return instance;
}

void EventHandler::AddListener(const EventId event_id, std::function<void(Event&)> const& listener)
{
    m_listeners[event_id].push_back(listener);
}

void EventHandler::SendEvent(Event& event)
{
    for (auto const& listener : m_listeners[event.get_type()]) {
        listener(event);
    }
}

void EventHandler::SendEvent(const EventId event_id)
{
    Event event(event_id);

    for (auto const& listener : m_listeners[event_id]) {
        listener(event);
    }
}
}
