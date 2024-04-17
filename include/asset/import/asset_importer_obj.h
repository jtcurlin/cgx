// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"
#include "asset/import/asset_importer.h"
#include "tiny_obj_loader.h"

#include <filesystem>

namespace cgx::asset
{
class AssetImporterOBJ : public AssetImporter
{
public:
    AssetImporterOBJ();
    ~AssetImporterOBJ() override;

    AssetID import(const std::string& path) override;
    void    import_materials(const std::filesystem::path& source_path);
    void    import_meshes(const std::filesystem::path& source_path);

    void reset();

private:
    bool               m_enable_format_warnings{false};
    tinyobj::ObjReader m_tinyobj_reader{};

    std::unordered_map<int, AssetID> m_mat_asset_ids{};
    std::vector<AssetID>             m_mesh_asset_ids{};
};
}
