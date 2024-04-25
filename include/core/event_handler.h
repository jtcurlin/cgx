// Copyright © 2024 Jacob Curlin

#pragma once

#include "event.h"

#include <list>
#include <unordered_map>

namespace cgx::core
{
class EventHandler
{
    // ! SINGLETON

public:
    EventHandler(const EventHandler&)            = delete;
    EventHandler& operator=(const EventHandler&) = delete;

    static EventHandler& get_instance();

    void add_listener(event::EventId event_id, std::function<void(event::Event&)> const& listener);
    void send_event(event::Event& event);
    void send_event(event::EventId event_id);

private:
    EventHandler();

    std::unordered_map<event::EventId, std::list<std::function<void(event::Event&)>>> m_listeners;
};
}

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), std::placeholders::_1)


