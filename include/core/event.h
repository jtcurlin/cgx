// Copyright © 2024 Jacob Curlin

#pragma once

#include <any>
#include <unordered_map>
#include <functional>

namespace cgx::core::event
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

    [[nodiscard]] EventId get_type() const { return m_type; }

private:
    EventId                               m_type{};
    std::unordered_map<EventId, std::any> m_data{};
};

constexpr std::uint32_t fnv1a_32(char const* s, const std::size_t count)
{
    return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
}

constexpr std::uint32_t operator "" _hash(char const* s, const std::size_t count)
{
    return fnv1a_32(s, count);
}

}
