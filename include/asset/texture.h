// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include "glad/glad.h"

namespace cgx::asset
{
enum class TextureFilter
{
    Nearest,
    Linear
};

class Texture final : public Asset
{
public:
    Texture(
        const std::string& source_path,
        const std::string& tag,
        uint32_t           width,
        uint32_t           height,
        uint32_t           num_channels,
        GLenum             format,
        unsigned char*     pixels);
    ~Texture() override;

    void bind(uint32_t slot) const;

    [[nodiscard]] uint32_t       get_width() const;
    [[nodiscard]] uint32_t       get_height() const;
    [[nodiscard]] uint32_t       get_num_channels() const;
    [[nodiscard]] unsigned char* get_pixels() const;
    [[nodiscard]] GLenum         get_format() const;
    [[nodiscard]] TextureFilter  get_texture_filter() const;

    void               set_texture_filter(TextureFilter filter);

private:
    uint32_t       m_texture_id;
    uint32_t       m_width;
    uint32_t       m_height;
    uint32_t       m_num_channels;
    GLenum         m_format;
    unsigned char* m_pixels;

    TextureFilter m_filter;
};
}
