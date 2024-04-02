// Copyright © 2024 Jacob Curlin

#pragma once

#include <any>
#include <unordered_map>

namespace cgx::event
{
    using EventId = std::uint32_t;
    using ParamId = std::uint32_t;

    class Event
    {
    public:
        Event() = delete;

        explicit Event(EventId type) : m_type(type) {}

        template<typename T>
        void SetParam(ParamId id, T value)
        {
            m_data[id] = value;
        }

        template<typename T>
        T GetParam(ParamId id)
        {
            return std::any_cast<T>(m_data[id]);
        }

        EventId getType() const { return m_type; }

    private:

        EventId m_type{};
        std::unordered_map<EventId, std::any> m_data{};

    }; // class Event

} // namespace cgx::event