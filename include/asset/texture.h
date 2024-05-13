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
class Texture final : public Asset
{
    friend class gui::PropertiesPanel;

public:
    enum class FilterMode
    {
        Nearest              = GL_LINEAR,
        Linear               = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear  = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear   = GL_LINEAR_MIPMAP_LINEAR
    };

    enum class WrapMode
    {
        Repeat         = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge    = GL_CLAMP_TO_EDGE,
        ClampToBorder  = GL_CLAMP_TO_BORDER
    };

    enum class Format
    {
        Red               = GL_RED,
        RG                = GL_RG,
        RGB               = GL_RGB,
        RGBA              = GL_RGBA,
        R16F              = GL_R16F,
        RG16F             = GL_RG16F,
        RGB16F            = GL_RGB16F,
        RGBA16F           = GL_RGBA16F,
        R32F              = GL_R32F,
        RG32F             = GL_RG32F,
        RGB32F            = GL_RGB32F,
        RGBA32F           = GL_RGBA32F,
        DepthComponent    = GL_DEPTH_COMPONENT,
        DepthComponent16  = GL_DEPTH_COMPONENT16,
        DepthComponent24  = GL_DEPTH_COMPONENT24,
        DepthComponent32  = GL_DEPTH_COMPONENT32,
        DepthComponent32F = GL_DEPTH_COMPONENT32F,
        StencilIndex      = GL_STENCIL_INDEX,
        StencilIndex8     = GL_STENCIL_INDEX8,
        DepthStencil      = GL_DEPTH_STENCIL,
        Depth24Stencil8   = GL_DEPTH24_STENCIL8,
        Unsupported       = GL_NONE
    };

    enum class DataType
    {
        UnsignedByte    = GL_UNSIGNED_BYTE,
        Byte            = GL_BYTE,
        UnsignedShort   = GL_UNSIGNED_SHORT,
        Short           = GL_SHORT,
        UnsignedInt     = GL_UNSIGNED_INT,
        Int             = GL_INT,
        Float           = GL_FLOAT,
        UnsignedInt24_8 = GL_UNSIGNED_INT_24_8,
        Unsupported     = GL_NONE
    };

    Texture(
        std::string tag,
        std::string source_path,
        uint32_t    width,
        uint32_t    height,
        uint32_t    num_channels,
        Format      format,
        DataType    data_type,
        void*       pixels,
        bool        stb_owned_data = false);
    ~Texture() override;

    void setup();
    void bind(uint32_t slot) const;

    [[nodiscard]] uint32_t get_texture_id() const;
    [[nodiscard]] uint32_t get_width() const;
    [[nodiscard]] uint32_t get_height() const;
    [[nodiscard]] uint32_t get_num_channels() const;
    [[nodiscard]] void*    get_pixels() const;
    [[nodiscard]] Format   get_format() const;
    [[nodiscard]] DataType get_data_type() const;

    [[nodiscard]] FilterMode get_min_filter() const;
    [[nodiscard]] FilterMode get_mag_filter() const;
    [[nodiscard]] WrapMode   get_wrap_s() const;
    [[nodiscard]] WrapMode   get_wrap_t() const;

    void resize(uint32_t new_width, uint32_t new_height);

    void set_min_filter(FilterMode filter);
    void set_mag_filter(FilterMode filter);
    void set_wrap_s(WrapMode wrap);
    void set_wrap_t(WrapMode wrap);

    std::string     get_path_prefix() const override;
    AssetType::Type get_asset_type() const override;

private:
    uint32_t m_texture_id{};
    uint32_t m_width{};
    uint32_t m_height{};
    uint32_t m_num_channels{};
    Format   m_format;
    DataType m_data_type;
    void*    m_pixels{nullptr};
    bool m_stb_owned_data{false};

    FilterMode m_min_filter{FilterMode::Linear};
    FilterMode m_mag_filter{FilterMode::Linear};
    WrapMode   m_wrap_s{WrapMode::Repeat};
    WrapMode   m_wrap_t{WrapMode::Repeat};
};
}
