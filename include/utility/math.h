// Copyright © 2024 Jacob Curlin

#pragma once

namespace cgx::math
{
template<typename T>
constexpr T clamp(T val, T min, T max)
{
    return (val < min) ? min : (val > max) ? max : val;
}
}
