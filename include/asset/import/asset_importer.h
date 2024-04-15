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
    AssetImporter(
        const std::string&       label,
        const std::string&       base_dir,
        std::vector<std::string> extensions,
        std::vector<AssetType>   asset_types);
    virtual ~AssetImporter();

    void initialize(const std::shared_ptr<AssetManager>& asset_manager);

    virtual AssetID import(const std::string& path) = 0;

    [[nodiscard]] const std::vector<std::string>& get_supported_file_extensions() const;
    [[nodiscard]] const std::vector<AssetType>&   get_output_asset_types() const;
    [[nodiscard]] const std::string&              get_label() const;

    [[nodiscard]] std::string get_relative_path(const std::filesystem::path& absolute_path) const;
    [[nodiscard]] std::string get_absolute_path(const std::filesystem::path& relative_path) const;

protected:
    std::string           m_label{};
    std::filesystem::path m_base_dir;

    std::weak_ptr<AssetManager> m_asset_manager{};
    std::vector<std::string>    m_extensions{}; // supported file extensions
    std::vector<AssetType>      m_asset_types{};

};
}
