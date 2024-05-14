// Copyright © 2024 Jacob Curlin
#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * model * vec4(in_position, 1.0);
}
