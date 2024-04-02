// Copyright © 2024 Jacob Curlin

#include "event/event_handler.h"

namespace cgx::event
{
    void EventHandler::AddListener(EventId event_id, std::function<void(Event&)> const& listener)
    {
        m_listeners[event_id].push_back(listener);
    }

    void EventHandler::SendEvent(Event& event)
    {
        uint32_t type = event.getType();

        for (auto const& listener : m_listeners[type])
        {
            listener(event);
        }
    }

    void EventHandler::SendEvent(EventId event_id)
    {
        Event event(event_id);

        for (auto const& listener : m_listeners[event_id])
        {
            listener(event);
        }
    }

} // namespace cgx::event