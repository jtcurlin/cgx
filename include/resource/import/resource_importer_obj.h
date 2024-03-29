// Copyright © 2024 Jacob Curlin

#pragma once

#include "resource/import/resource_importer.h"
#include "tinyobjloader/tiny_obj_loader.h"

#include <filesystem>

namespace cgx::resource
{

    class ResourceImporterOBJ : public ResourceImporter
    {
    public:
        bool Initialize(const std::string& path) override;
        RUID Import() override;


        virtual std::string getTypeName() const override { return "Model"; }

        void ImportMaterials();
        void ImportMeshes();

        void Reset();

    private:
        std::filesystem::path m_source_path;
        std::filesystem::path m_mat_dir_path;

        bool m_enable_format_warnings = false;
        tinyobj::ObjReader          m_tinyobj_reader;
        tinyobj::ObjReaderConfig    m_tinyobj_reader_config;

        std::unordered_map<int, RUID> m_mat_ruids;
        std::vector<RUID> m_mesh_ruids;


    }; // class ResourceImporterOBJ

}; // namespace cgx::resource