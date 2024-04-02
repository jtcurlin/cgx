// Copyright © 2024 Jacob Curlin

#pragma once

#include "event/event.h"

#include <functional>
#include <list>
#include <unordered_map>

namespace cgx::event
{
    class EventHandler
    {
    public:

        void AddListener(EventId event_id, std::function<void(Event&)> const& listener);

        void SendEvent(Event& event);

        void SendEvent(EventId event_id);

    private:
        std::unordered_map<EventId, std::list<std::function<void(Event&)>>> m_listeners;

    }; // class EventHandler

} // namespace cgx::event

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}
