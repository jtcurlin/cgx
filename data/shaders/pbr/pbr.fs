// Copyright © 2024 Jacob Curlin

#version 330 core

out vec4 FragColor;

in vec3 position;
in vec2 uv;
in vec3 normal;

#define BASE_COLOR_MAP_BIT 1 << 0
#define NORMAL_MAP_BIT 2 << 1
#define METALLIC_ROUGHNESS_MAP_BIT 1 << 2
#define OCCLUSION_MAP_BIT 1 << 3
#define EMISSIVE_MAP_BIT 1 << 4

uniform int map_bitset;

// material parameters
uniform sampler2D base_color_map;
uniform sampler2D normal_map;
uniform sampler2D metallic_roughness_map;
uniform sampler2D occlusion_map;
uniform sampler2D emissive_map;

uniform vec4 base_color_factor;
uniform float metallic_factor;
uniform float roughness_factor;

// lights
uniform vec3 light_positions[4];
uniform vec3 light_colors[4];

uniform vec3 cam_pos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anyways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_map, uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(position);
    vec3 Q2  = dFdy(position);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
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

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cam_pos - position);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(light_positions[i] - position);
        vec3 H = normalize(V + L);
        float distance = length(light_positions[i] - position);
        float attenuation = 1.0 / (distance * distance * 0.01 + 1.0);
        vec3 radiance = light_colors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.13) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, alpha);
}