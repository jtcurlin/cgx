// Copyright © 2024 Jacob Curlin
#version 330 core

layout (location = 0) out vec4 FragColor;

uniform vec3 light_color;

void main()
{
    FragColor = vec4(light_color, 1.0);
}