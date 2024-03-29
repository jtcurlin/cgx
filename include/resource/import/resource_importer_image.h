// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/import/resource_importer.h"
#include "resource/resource.h"

#include <filesystem>
#include <string>

namespace cgx::resource
{
    class ResourceImporterImage : public ResourceImporter
    {
    public:
        bool Initialize(const std::string& path) override;
        RUID Import() override;

        virtual std::string getTypeName() const override { return "Texture"; }

    public:
        std::filesystem::path m_source_path;

    }; // class ResourceImporterImage

} // namespace cgx::resource