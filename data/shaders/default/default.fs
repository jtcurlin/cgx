#version 330 core

// Fragment shader
uniform sampler2D base_color_map;
uniform vec3 light_direction;

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 FragColor;

void main() {
    vec3 base_color = texture(base_color_map, uv).rgb;
    float diffuse = max(dot(normal, light_direction), 0.0) * 1;
    FragColor = vec4(base_color * diffuse, 1.0);
}