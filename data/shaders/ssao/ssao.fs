// Copyright © 2024 Jacob Curlin
#version 330 core

out float FragColor;

in vec2 uv;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D noise_tex;

uniform float power;
uniform float radius;
uniform float bias;
uniform int kernel_size;

uniform vec3 samples[64];

// int kernel_size = 64;
// float radius = 0.5;
// float bias = 0.025;

const vec2 noise_scale = vec2(1280.0/4.0, 720.0/4.0);

uniform mat4 view;
uniform mat4 proj;

void main()
{
    // get input for SSAO algorithm
    vec3 position = texture(g_position, uv).xyz;
    vec3 normal = normalize(texture(g_normal, uv).rgb);
    vec3 random_vec = normalize(texture(noise_tex, uv * noise_scale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernel_size; ++i)
    {
        // get sample position
        vec3 sample_pos = TBN * samples[i].xyz; // from tangent to view-space
        sample_pos = position + sample_pos * radius;
        sample_pos = (view * vec4(sample_pos, 1.0)).xyz;
        // sample_pos = position + sample_pos * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample_pos, 1.0);
        offset = proj * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sample_depth = texture(g_position, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, radius / abs(position.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
    }
    occlusion = 1.0 - (occlusion / kernel_size);

    FragColor = pow(occlusion, power);
}