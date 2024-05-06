// Copyright © 2024 Jacob Curlin

#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 position;
out vec3 normal;
out vec2 uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
    position = vec3(u_model * vec4(in_position, 1.0));
    normal = mat3(transpose(inverse(u_model))) * in_normal;
    uv = in_uv;

    gl_Position = u_proj * u_view * vec4(position, 1.0);
}