// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace cgx::asset
{
class AssetImporter
{
public:
    AssetImporter(std::string label, std::vector<std::string> extensions, std::vector<AssetType::Type> asset_types);
    virtual ~AssetImporter();

    void initialize(const std::shared_ptr<AssetManager>& asset_manager);

    virtual AssetID import(const std::string& source_path) = 0;

    [[nodiscard]] const std::vector<std::string>&     get_supported_file_extensions() const;
    [[nodiscard]] const std::vector<AssetType::Type>& get_output_asset_types() const;
    [[nodiscard]] const std::string&                  get_label() const;

protected:
    std::string                  m_label{};
    std::weak_ptr<AssetManager>  m_asset_manager{};
    std::vector<std::string>     m_extensions{}; // supported file extensions
    std::vector<AssetType::Type> m_asset_types{};
};
}
