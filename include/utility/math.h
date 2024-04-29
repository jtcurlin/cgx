// Copyright © 2024 Jacob Curlin

#pragma once

namespace cgx::math
{
template<typename T>
constexpr T clamp(T val, T min, T max)
{
    return (val < min) ? min : (val > max) ? max : val;
}

struct Color4f
{
    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    float a{0.0f};
};

inline Color4f HexToColor4f(const std::string& hexCode)
{
    Color4f color{};

    std::string hex = (hexCode[0] == '#') ? hexCode.substr(1) : hexCode;

    int r, g, b;
    sscanf(hex.c_str(), "%02x%02x%02x", &r, &g, &b);

    color.r = static_cast<float>(r) / 255.0f;
    color.g = static_cast<float>(g) / 255.0f;
    color.b = static_cast<float>(b) / 255.0f;
    color.a = 1.0f;

    return color;
}

}
