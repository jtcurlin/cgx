// Copyright © 2024 Jacob Curlin

#define STB_IMAGE_IMPLEMENTATION
#include "asset/import/asset_importer_image.h"
#include "asset/asset_manager.h"
#include "asset/texture.h"
#include "utility/paths.h"

#include "core/common.h"
#include "glad/glad.h"
#include "stb/stb_image.h"

namespace cgx::asset
{
AssetImporterImage::AssetImporterImage()
    : AssetImporter("Image Importer", std::string(ASSETS_DIRECTORY), {".jpg", ".png", ".tga"}, {AssetType::Texture}) {}

AssetImporterImage::~AssetImporterImage() = default;

AssetID AssetImporterImage::import(const std::string& path)
{
    const std::filesystem::path relative_path = get_relative_path(path);
    const std::filesystem::path source_path = get_absolute_path(relative_path);

    stbi_set_flip_vertically_on_load(1);

    stbi_uc* data = nullptr;
    int      width, height, num_channels;
    data = stbi_load(source_path.c_str(), &width, &height, &num_channels, 0);

    GLenum format = 0;
    if (data) {
        if (num_channels == 1) {
            format = GL_RED;
        }
        else if (num_channels == 3) {
            format = GL_RGB;
        }
        else if (num_channels == 4) {
            format = GL_RGBA;
        }

        if (format == 0) {
            CGX_ERROR(
                "AssetImporterImage: Failed to determine valid texture data " "format for specified path. [{}]",
                source_path.string());
            return k_invalid_id;
        }
    }
    if (!data) {
        CGX_ERROR("AssetImporterImage: Failed to load texture resource at path {}", source_path.string());
        return k_invalid_id;
    }


    const auto texture = std::make_shared<Texture>(
        source_path.string(),
        relative_path.stem().string(),
        width,
        height,
        num_channels,
        format,
        data);

    // this class has an inherited member variable std::weak_ptr<AssetManager>  m_asset_manager
    // at the start of import, it should get this asset manager object / make sure its been initialized / still exists
    const auto asset_manager = m_asset_manager.lock();
    if (!asset_manager) {
        CGX_ERROR("AssetImporterImage: failed to acquire Access Manager instance. check initialization.");
        return k_invalid_id;
    }
    return asset_manager->add_asset(texture);
}

} // namespace cgx::resource
