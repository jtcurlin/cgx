// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include "glad/glad.h"

namespace cgx::gui
{
class PropertiesPanel;
}

namespace cgx::asset
{

enum class FilterMode
{
    Nearest = GL_LINEAR,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class WrapMode
{
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER
};

class Texture final : public Asset
{
    friend class gui::PropertiesPanel;

public:
    Texture(
        std::string    tag,
        std::string    source_path,
        uint32_t       width,
        uint32_t       height,
        uint32_t       num_channels,
        GLenum         format,
        unsigned char* pixels);
    ~Texture() override;

    void bind(uint32_t slot) const;

    [[nodiscard]] uint32_t       get_width() const;
    [[nodiscard]] uint32_t       get_height() const;
    [[nodiscard]] uint32_t       get_num_channels() const;
    [[nodiscard]] unsigned char* get_pixels() const;
    [[nodiscard]] GLenum         get_format() const;

    [[nodiscard]] FilterMode get_min_filter() const;
    [[nodiscard]] FilterMode get_mag_filter() const;
    [[nodiscard]] WrapMode get_wrap_s() const;
    [[nodiscard]] WrapMode get_wrap_t() const;

    void set_min_filter(FilterMode filter);
    void set_mag_filter(FilterMode filter);
    void set_wrap_s(WrapMode wrap);
    void set_wrap_t(WrapMode wrap);

    std::string get_path_prefix() const override;
    AssetType::Type    get_asset_type() const override;

private:
    uint32_t       m_texture_id;
    uint32_t       m_width;
    uint32_t       m_height;
    uint32_t       m_num_channels;
    GLenum         m_format;
    unsigned char* m_pixels;

    FilterMode m_min_filter{FilterMode::Linear};
    FilterMode m_mag_filter{FilterMode::Linear};
    WrapMode m_wrap_s{WrapMode::Repeat};
    WrapMode m_wrap_t{WrapMode::Repeat};
};
}
