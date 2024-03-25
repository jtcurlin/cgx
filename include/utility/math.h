// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"

namespace cgx::math
{

    // clamp a value between a specified minimum and maximum
    template<typename T>
    constexpr T clamp(T val, T min, T max)
    {
        return (val < min) ? min : (val > max) ? max : val;
    }




} // namespace cgx::math