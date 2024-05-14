// Copyright © 2024 Jacob Curlin
#version 330 core

layout (location = 0) in vec3 in_position;

out vec3 position;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    position = in_position;
    gl_Position =  proj * view * vec4(position, 1.0);
}
