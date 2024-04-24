#version 330 core

// Vertex shader
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vNormal;
out vec2 vTexCoord;

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    vNormal = normalize((model * vec4(normal, 0.0)).xyz);
    vTexCoord = uv;
}
