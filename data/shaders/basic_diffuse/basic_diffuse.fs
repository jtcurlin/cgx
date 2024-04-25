#version 330 core

// Fragment shader
uniform sampler2D base_color_map;
uniform vec3 light_direction;

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

void main() {
    vec3 base_color = texture(base_color_map, vTexCoord).rgb;
    float diffuse = max(dot(vNormal, light_direction), 0.0) * 1;
    FragColor = vec4(base_color * diffuse, 1.0);
}