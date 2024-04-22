// Copyright © 2024 Jacob Curlin

#include "../../../include/core/systems/time_system.h"

namespace cgx::core
{
TimeSystem::TimeSystem()
{
    m_start_time = std::chrono::steady_clock::now();
}

TimeSystem::~TimeSystem() = default;

void TimeSystem::start()
{
    m_current_time = std::chrono::steady_clock::now();
}

void TimeSystem::frame_update(const double limit)
{
    auto new_time = std::chrono::steady_clock::now();

    // if limit is set (!= 0.0), limit framerate by sleeping
    if (limit != 0.0) {
        const auto next_frame_time = m_current_time + std::chrono::duration<double>(limit);
        while (std::chrono::steady_clock::now() < next_frame_time) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        new_time = std::chrono::steady_clock::now();
    }

    m_frame_time   = std::chrono::duration<double>(new_time - m_current_time).count();
    m_current_time = new_time;
    m_frame_number++;
}

const std::chrono::steady_clock::time_point& TimeSystem::get_current_time() const
{
    return m_current_time;
}

double TimeSystem::get_frame_time() const
{
    return m_frame_time;
}

double TimeSystem::get_uptime() const
{
    const std::chrono::duration<double> uptime = m_current_time - m_start_time;
    return uptime.count();
}


uint64_t TimeSystem::get_frame_number() const
{
    return m_frame_number;
}
}
