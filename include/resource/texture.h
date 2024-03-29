// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/resource.h"

namespace cgx::resource
{
    using GLenum = unsigned int;

    enum class TextureFilter
    {
        Nearest,
        Linear
    };

    class Texture : public Resource
    {
    public:
        Texture(
            const std::string& path, 
            const std::string& tag,
            uint32_t width, uint32_t height, 
            uint32_t num_channels, 
            GLenum format, 
            unsigned char* pixels); 
        ~Texture();

        virtual ResourceType getType() const override { return ResourceType::Texture; }

        void Setup();
        void Bind(uint32_t slot) const;

        [[nodiscard]] uint32_t GetWidth() const { return m_width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_height; }
        [[nodiscard]] uint32_t GetNumChannels() const { return m_num_channels; }
        [[nodiscard]] TextureFilter GetTextureFilter() const { return m_filter; }

        void SetTextureFilter(TextureFilter filter);

    private:
        uint32_t m_width; 
        uint32_t m_height;
        uint32_t m_num_channels;
        uint32_t m_texture_id;

        unsigned char* m_pixels;
        GLenum m_format;

        TextureFilter m_filter;

    }; // class Texture

} // namespace cgx::resource