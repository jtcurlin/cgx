// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "asset/asset.h"
#include "asset/import/asset_importer.h"

#include "event/event_handler.h"
#include "utility/paths.h"

#include <unordered_map>


namespace cgx::asset
{
struct PathConfig
{
    std::filesystem::path shader_dir{SHADERS_DIRECTORY};
    std::filesystem::path asset_dir{ASSETS_DIRECTORY};
    std::filesystem::path font_dir{FONTS_DIRECTORY};
};

class AssetManager : public std::enable_shared_from_this<AssetManager>
{
public:
    explicit AssetManager(PathConfig path_config = PathConfig());
    ~AssetManager();

    // todo: refine importer ownership model & extension -> importer mapping
    void register_importer(const std::shared_ptr<AssetImporter>& importer);

    AssetID import_asset(const std::string& path);
    AssetID add_asset(const std::shared_ptr<Asset>& asset);
    bool    remove_asset(AssetID asset_id);


    AssetID                     get_id_by_path(const std::string& path);
    std::vector<AssetID>        getAllIDs();
    const std::vector<AssetID>& getAllIDs(AssetType type_filter);

    std::shared_ptr<Asset>                                     get_asset(AssetID asset_id);
    const std::unordered_map<AssetID, std::shared_ptr<Asset>>& get_assets() const;
    const std::vector<std::shared_ptr<AssetImporter>>&         get_importers() const;

    bool is_path_supported(const std::string& path) const;

private:
    PathConfig m_path_config;

    std::vector<std::shared_ptr<AssetImporter>>                   m_importers;
    std::unordered_map<std::string, std::weak_ptr<AssetImporter>> m_extension_to_importer_map;

    std::unordered_map<AssetID, std::shared_ptr<Asset>>   m_assets;
    std::unordered_map<std::string, AssetID>              m_source_path_to_id;
    std::unordered_map<std::string, std::vector<AssetID>> m_name_to_id;
    std::unordered_map<AssetType, std::vector<AssetID>>   m_type_to_id;

    static std::filesystem::path clean_path(std::string path);
};
}
