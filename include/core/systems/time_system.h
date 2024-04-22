// Copyright © 2024 Jacob Curlin
// source: https://github.com/tksuoran/erhe/blob/aa0cc8dbd3edf0aea0f0f8a4512505a4eb6887f0/src/editor/time.hpp

#pragma once

#include "../common.h"
#include <chrono>

namespace cgx::core
{
class TimeSystem
{
public:
    TimeSystem();
    ~TimeSystem();

    void start();
    void frame_update(double limit = 0.0);

    [[nodiscard]] const std::chrono::steady_clock::time_point& get_current_time() const;
    [[nodiscard]] double                                       get_frame_time() const;
    [[nodiscard]] double get_uptime() const;
    [[nodiscard]] uint64_t                                     get_frame_number() const;

private:
    std::chrono::steady_clock::time_point m_current_time{};
    std::chrono::steady_clock::time_point m_start_time;

    double   m_frame_time{0.0};
    uint64_t m_frame_number{0};
};
}
