// Copyright © 2024 Jacob Curlin

#include "resource/import/resource_importer_image.h"

#include "resource/resource_manager.h"
#include "resource/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glad/glad.h>

namespace cgx::resource
{
    void ResourceImporterImage::Initialize(const std::string& path)
    {
        m_source_path = path;
    }

    RUID ResourceImporterImage::Import()
    {
        if (m_source_path.empty()) { return 0; }

        std::string normalized_path = m_source_path;
        std::replace(normalized_path.begin(), normalized_path.end(), '\\', '/');

        stbi_set_flip_vertically_on_load(1);

        stbi_uc* data = nullptr;
        int width, height, num_channels;
        data = stbi_load(normalized_path.c_str(), &width, &height, &num_channels, 0);

        GLenum format = 0;
        if (data)
        {
            if (num_channels == 1)      { format = GL_RED; }
            else if (num_channels == 3) { format = GL_RGB; }
            else if (num_channels == 4) { format = GL_RGBA; }

            if (format == 0) { CGX_ERROR("Failed to determine valid texture data format"); } 
        }
        else { CGX_ERROR("Failed to load texture data. ({})", normalized_path); }

        if (data && format != 0)
        {
            auto texture = std::make_shared<Texture>(m_source_path, width, height, num_channels, format, data);
            return ResourceManager::getSingleton().loadResource<Texture>(texture)->getRUID();
        }
        else
        {
            CGX_ERROR("Failed to load texture resource at path {}", m_source_path);
            return 0;
            /*
            unsigned char placeholder_data[] = {
                255, 0, 255,   255, 255, 255,   255, 0, 255,   255, 255, 255,
                255, 255, 255, 255, 0, 255,     255, 255, 255, 255, 0, 255,
                255, 0, 255,   255, 255, 255,   255, 0, 255,   255, 255, 255,
                255, 255, 255, 255, 0, 255,     255, 255, 255, 255, 0, 255
            };

            width = 4;
            height = 4;
            num_channels = 3;

            auto texture = std::make_shared<Texture>(id, width, height, num_channels, format, placeholder_data);
            m_textures[id] = texture;
            return texture;
            */ 
        }
    }

} // namespace cgx::resource