// Copyright © 2024 Jacob Curlin
#version 330 core

#define BASE_COLOR_MAP_BIT 1 << 0
#define NORMAL_MAP_BIT 1 << 1
#define METALLIC_ROUGHNESS_MAP_BIT 1 << 2
#define OCCLUSION_MAP_BIT 1 << 3
#define EMISSIVE_MAP_BIT 1 << 4

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec3 g_albedo;
layout (location = 3) out float g_metallic;
layout (location = 4) out float g_roughness;

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform vec4 base_color_factor;
uniform float metallic_factor;
uniform float roughness_factor;

uniform sampler2D base_color_map;
uniform sampler2D metallic_roughness_map;
uniform sampler2D normal_map;
uniform sampler2D occlusion_map;
uniform sampler2D emissive_map;

uniform int map_bitset;

void main()
{
    bool has_base_color_map = ((map_bitset & BASE_COLOR_MAP_BIT) != 0);
    bool has_normal_map = ((map_bitset & NORMAL_MAP_BIT) != 0);
    bool has_metallic_roughness_map = ((map_bitset & METALLIC_ROUGHNESS_MAP_BIT) != 0);
    bool has_occlusion_map = ((map_bitset & OCCLUSION_MAP_BIT) != 0);
    bool has_emissive_map = ((map_bitset & EMISSIVE_MAP_BIT) != 0);

    vec3 albedo = has_base_color_map ? pow(texture(base_color_map, uv).rgb, vec3(2.2)) : base_color_factor.rgb;
    float alpha = has_base_color_map ? texture(base_color_map, uv).a : base_color_factor.a;
    float metallic = has_metallic_roughness_map ? texture(metallic_roughness_map, uv).b : metallic_factor;
    float roughness = has_metallic_roughness_map ? texture(metallic_roughness_map, uv).g : roughness_factor;
    float ao        = has_occlusion_map ? texture(occlusion_map, uv).r : 1.0;

    g_position = position;
    g_normal = normalize(normal);
    g_albedo = albedo;
    g_roughness = roughness;
    g_metallic = metallic;
}
