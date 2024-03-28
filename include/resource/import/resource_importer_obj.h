// Copyright © 2024 Jacob Curlin

#pragma once

#include "resource/import/resource_importer.h"

namespace cgx::resource
{
    class ResourceImporterOBJ : public ResourceImporter
    {
    public:
        void Initialize(const std::string& path) override;
        RUID Import() override;

        virtual std::string getTypeName() const override { return "Model"; }

    private:
        std::string m_source_path;

    }; // class ResourceImporterOBJ

}; // namespace cgx::resource