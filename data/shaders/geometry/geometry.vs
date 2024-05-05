// Copyright © 2024 Jacob Curlin
#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 position;
out vec3 normal;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 world_pos = model * vec4(in_position, 1.0);
    position = world_pos.xyz;
    uv = in_uv;

    mat3 normal_mat = transpose(inverse(mat3(model)));
    normal = normal_mat * in_normal;

    gl_Position = proj * view * world_pos;
}
