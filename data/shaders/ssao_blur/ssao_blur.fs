// Copyright © 2024 Jacob Curlin
#version 330 core

out float FragColor;

in vec2 uv;

uniform sampler2D ssao_input_tex;

void main()
{
    vec2 texel_size = 1.0 / vec2(textureSize(ssao_input_tex, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao_input_tex, uv + offset).r;
        }
    }
    FragColor = result / (4.0 * 4.0);
}