// Copyright © 2024 Jacob Curlin

#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;

out vec2 uv;

void main()
{
    uv = in_uv;
	gl_Position = vec4(in_position, 1.0);
}