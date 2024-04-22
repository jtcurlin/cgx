// Copyright © 2024 Jacob Curlin

#pragma once

#include <any>
#include <unordered_map>

namespace cgx::ecs
{
using EventId = std::uint32_t;
using ParamId = std::uint32_t;

class Event
{
public:
    Event() = delete;

    explicit Event(const EventId type) : m_type(type) {}

    template<typename T>
    void set_param(const ParamId id, T value)
    {
        m_data[id] = value;
    }

    template<typename T>
    T get_param(const ParamId id)
    {
        return std::any_cast<T>(m_data[id]);
    }

    [[nodiscard]] EventId get_type() const;

private:
    EventId                               m_type{};
    std::unordered_map<EventId, std::any> m_data{};
};
}
