// Copyright © 2024 Jacob Curlin

#include "resource/import/resource_importer_image.h"

#define STB_IMAGE_IMPLEMENTATION

#include "core/common.h"
#include "resource/resource_manager.h"
#include "resource/texture.h"

#include "glad/glad.h"
#include "stb/stb_image.h"


namespace cgx::resource
{
    bool ResourceImporterImage::Initialize(const std::string& path)
    {
        // check source file path non-empty
        if (path.empty())
        {
            CGX_ERROR("ResourceImporterImage: failed to import file. (empty path provided)");
            return false;
        }

        // check source file path exists
        m_source_path = std::filesystem::path(path);  
        if (!std::filesystem::exists(m_source_path))
        {
            CGX_ERROR("ResourceImporterImage: failed to import file at provided path [{}]. " \
                      "(invalid file path)", m_source_path.string());
            return false;
        }

        // normalize path (convert '//', '\', etc.)
        m_source_path = m_source_path.make_preferred().string();

        m_initialized = true;
        return m_initialized;
    }

    RUID ResourceImporterImage::Import()
    {
        auto& resource_manager = ResourceManager::getSingleton();

        stbi_set_flip_vertically_on_load(1);

        stbi_uc* data = nullptr;
        int width, height, num_channels;
        data = stbi_load(m_source_path.c_str(), &width, &height, &num_channels, 0);

        GLenum format = 0;
        if (data)
        {
            if (num_channels == 1)      { format = GL_RED; }
            else if (num_channels == 3) { format = GL_RGB; }
            else if (num_channels == 4) { format = GL_RGBA; }

            if (format == 0) 
            { 
                CGX_ERROR("ResourceImporterImage: Failed to determine valid texture data " \
                          "format for specified path. [{}]", m_source_path.string()); 
                return k_invalid_id;
            } 
        }
        else 
        { 
            CGX_ERROR("ResourceImporterImage: Failed to load texture data for specified " \
                      "path. [{}]", m_source_path.string()); 
            return k_invalid_id;
        }

        if (data && format != 0)
        {
            auto texture = std::make_shared<Texture>(
                m_source_path, m_source_path.stem().string(),
                width, height, num_channels, 
                format, data
            );
            return resource_manager.RegisterResource<Texture>(texture, false);
        }
        else
        {
            CGX_ERROR("ResourceImporterImage: Failed to load texture resource at path {}", m_source_path.string());
            return k_invalid_id;
        }
    }

} // namespace cgx::resource