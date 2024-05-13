// Copyright © 2024 Jacob Curlin

#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_metallic;
uniform sampler2D g_roughness;
uniform sampler2D ssao;

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float cutoff;
};

uniform PointLight lights[100];
uniform int num_point_lights;
uniform vec3 view_pos;

uniform bool ssao_enabled = false;

const float PI = 3.14159265359;

/*
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
*/

// ----------------------------------------------------------------------------
float distribution_ggx(vec3 N, vec3 H, float roughness)
{
   float a = roughness * roughness;
       float a2 = a * a;
       float NdotH = max(dot(N, H), 0.0);
       float NdotH2 = NdotH * NdotH;

       float nom   = a2;
       float denom = (NdotH2 * (a2 - 1.0) + 1.0);
       denom = PI * denom * denom;

       return nom / denom;
}
// ----------------------------------------------------------------------------
float geometry_schlick_ggx(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_ggx(NdotV, roughness);
    float ggx1 = geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
// Fresnel-Schlick Approximation: F(h,v,F0) = F0 + (1-F0)(1 - (h*v))^5
// F0: surface reflection at zero incidence (how much a surface reflects if looking directly at the surface)
//
vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 position = texture(g_position, uv).rgb;
    vec3 normal = texture(g_normal, uv).rgb;
    vec3 albedo = texture(g_albedo, uv).rgb;
    float metallic = texture(g_metallic, uv).r;
    float roughness = texture(g_roughness, uv).r;

    vec3 N = normalize(normal);
    vec3 V = normalize(view_pos - position);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i=0; i < num_point_lights; i++)
    {
        vec3 L = normalize(lights[i].position - position);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - position);
        // float attenuation = 1.0 / (distance * distance);

        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance*distance);
        attenuation *= clamp(1.0 - pow(distance / lights[i].range, lights[i].cutoff), 0.0, 1.0);

        vec3 radiance = lights[i].color * attenuation * lights[i].intensity;

        float NDF = distribution_ggx(N, H, roughness);
        float G = geometry_smith(N, V, L, roughness);
        vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo;

    ambient *= ssao_enabled ? texture(ssao, uv).r : 1.0;

    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}