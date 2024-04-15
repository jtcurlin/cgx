// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/import/asset_importer.h"

#include <filesystem>

namespace cgx::asset
{
class AssetImporterImage : public AssetImporter
{
public:
    AssetImporterImage();
    ~AssetImporterImage() override;

    AssetID import(const std::string &path) override;
};
}
