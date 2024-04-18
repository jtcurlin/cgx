// Copyright © 2024 Jacob Curlin

#include "asset/import/asset_importer.h"

namespace cgx::asset
{
AssetImporter::AssetImporter(
    const std::string&       label,
    std::vector<std::string> extensions,
    std::vector<AssetType>   asset_types)
    : m_label(label)
    , m_extensions(std::move(extensions))
    , m_asset_types(std::move(asset_types)) {}

AssetImporter::~AssetImporter() = default;

void AssetImporter::initialize(const std::shared_ptr<AssetManager>& asset_manager)
{
    m_asset_manager = asset_manager;
}

const std::vector<std::string>& AssetImporter::get_supported_file_extensions() const
{
    return m_extensions;
}

const std::vector<AssetType>& AssetImporter::get_output_asset_types() const
{
    return m_asset_types;
}

const std::string& AssetImporter::get_label() const
{
    return m_label;
}

}
