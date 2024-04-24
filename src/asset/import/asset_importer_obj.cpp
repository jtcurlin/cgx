// Copyright © 2024 Jacob Curlin

#include "asset/import/asset_importer_obj.h"
#include "asset/asset_manager.h"

#include "core/common.h"
#include "asset/model.h"
#include "asset/mesh.h"
#include "asset/phong_material.h"
#include "asset/texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "glm/glm.hpp"

#include <sstream>
#include <iomanip>

namespace cgx::asset
{
AssetImporterOBJ::AssetImporterOBJ()
    : AssetImporter("Wavefront OBJ Importer", {".obj"}, {AssetType::Model}) {}

AssetImporterOBJ::~AssetImporterOBJ() = default;

AssetID AssetImporterOBJ::import(const std::string& source_path)
{
    const std::filesystem::path fs_path(source_path);

    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = fs_path.parent_path().string();
    if (!m_tinyobj_reader.ParseFromFile(source_path, config)) {
        CGX_ERROR(
            "AssetImporterOBJ: Initialization Failed. (tinyobjreader " "failed to parse file at path [{}]",
            source_path);

        if (!m_tinyobj_reader.Error().empty()) {
            CGX_ERROR("AssetImporterOBJ: {}", m_tinyobj_reader.Error());
        }
        return false;
    }
    if (m_enable_format_warnings && !m_tinyobj_reader.Warning().empty()) {
        CGX_WARN("AssetImporterOBJ: TinyObjReader : {}", m_tinyobj_reader.Warning());
    }

    import_materials(source_path);
    import_meshes(source_path);

    const auto asset_manager = m_asset_manager.lock();
    if (!asset_manager) {
        CGX_ERROR("AssetImporterImage: failed to acquire Access Manager instance. check initialization.");
        return k_invalid_id;
    }

    // construct vector of Mesh shared ptr's from the stored Mesh RUID vector
    std::vector<std::shared_ptr<Mesh>> meshes;
    for (const auto& asset_id : m_mesh_asset_ids) {
        auto mesh = std::dynamic_pointer_cast<Mesh>(asset_manager->get_asset(asset_id));
        meshes.push_back(mesh);
    }

    // construct Model resource
    const auto model = std::make_shared<Model>(fs_path.stem().string(), source_path, meshes);

    reset(); // reset importer

    // register Model resource and return its resource UID.
    return asset_manager->add_asset(model);
}

void AssetImporterOBJ::import_materials(const std::filesystem::path& source_path)
{
    // auto& resource_manager = AssetManager::getSingleton();
    auto& materials = m_tinyobj_reader.GetMaterials();

    for (size_t mat_id = 0 ; mat_id < materials.size() ; mat_id++) {
        auto& mat = materials[mat_id];

        // build material's tag string
        std::stringstream tag_ss;
        tag_ss << source_path.stem().string() << "_";
        if (!mat.name.empty()) {
            tag_ss << mat.name;
        }
        else {
            tag_ss << "mat" << std::setw(2) << std::setfill('0') << mat_id;
        }

        // parse & load material's corresponding texture resources
        std::shared_ptr<Texture> ambient_map, diffuse_map, specular_map, normal_map;

        auto asset_manager = m_asset_manager.lock();
        CGX_ASSERT(asset_manager, "failed to get asset_manager");

        if (!mat.ambient_texname.empty()) {
            std::string tex_path = (source_path.parent_path() / mat.ambient_texname).string();
            AssetID id = asset_manager->import_asset(tex_path);
            ambient_map = std::dynamic_pointer_cast<Texture>(asset_manager->get_asset(id));
        }
        else {
            ambient_map = nullptr;
        }

        if (!mat.diffuse_texname.empty()) {
            std::string tex_path = (source_path.parent_path() / mat.diffuse_texname).string();
            AssetID id = asset_manager->import_asset(tex_path);
            diffuse_map = std::dynamic_pointer_cast<Texture>(asset_manager->get_asset(id));
        }
        else {
            diffuse_map = nullptr;
        }

        if (!mat.specular_texname.empty()) {
            std::string tex_path = (source_path.parent_path() / mat.specular_texname).string();
            AssetID id = asset_manager->import_asset(tex_path);
            specular_map = std::dynamic_pointer_cast<Texture>(asset_manager->get_asset(id));
        }
        else {
            specular_map = nullptr;
        }

        if (!mat.bump_texname.empty()) {
            std::string tex_path = (source_path.parent_path() / mat.bump_texname).string();
            AssetID id = asset_manager->import_asset(tex_path);
            normal_map = std::dynamic_pointer_cast<Texture>(asset_manager->get_asset(id));
        }
        else {
            normal_map = nullptr;
        }

        std::string material_source_path = source_path.string() + ":"  + tag_ss.str();
        auto material = std::make_shared<PhongMaterial>(
            tag_ss.str(),
            material_source_path,
            static_cast<float>(mat.shininess),
            glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]),
            glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]),
            glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]),
            ambient_map,
            diffuse_map,
            specular_map,
            normal_map);

        // initialize Material resource and load into ResourceManager
        AssetID material_asset_id = asset_manager->add_asset(material);

        // if ruid valid (successfully loaded/registered), add to tinyobj_mat_id -> resource UID map
        if (material_asset_id != k_invalid_id) {
            m_mat_asset_ids[mat_id] = material_asset_id;
        }
        else {
            CGX_WARN("AssetImporterOBJ: Failed to register material. (path : [{}])", source_path.string());
        }
    }
}

void AssetImporterOBJ::import_meshes(const std::filesystem::path& source_path)
{
    auto& attrib = m_tinyobj_reader.GetAttrib();
    auto& shapes = m_tinyobj_reader.GetShapes();

    std::vector<std::shared_ptr<Mesh>> meshes;

    std::unordered_map<unsigned int, std::vector<Vertex>>   material_to_vertices;
    std::unordered_map<unsigned int, std::vector<uint32_t>> material_to_indices;

    // iterate shapes
    // for (size_t s = 0; s < shapes.size(); s++)
    for (auto& shape : shapes) {
        size_t       index_offset = 0;
        unsigned int material_id;
        for (size_t f = 0 ; f < shape.mesh.num_face_vertices.size() ; f++) // iterate faces
        {
            // check if material ID is new and initialize it in material maps if so
            material_id = shape.mesh.material_ids[f];
            if (material_to_vertices.find(material_id) == material_to_vertices.end()) {
                material_to_vertices[material_id] = std::vector<Vertex>();
                material_to_indices[material_id]  = std::vector<unsigned int>();
            }

            // process face's vertices
            size_t fv = shape.mesh.num_face_vertices[f];
            for (size_t v = 0 ; v < fv ; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Vertex           vertex;

                // get current vertex position vector {x,y,z}
                vertex.position = glm::vec3(
                    attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 0],
                    attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 1],
                    attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 2]);

                // if present, get current vertex normal vector {x,y,z}
                if (idx.normal_index >= 0) {
                    vertex.normal = glm::vec3(
                        attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 0],
                        attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 1],
                        attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 2]);
                }

                // if present, get current vertex texture (uv) coordinates {u,v}
                if (idx.texcoord_index >= 0) {
                    vertex.uv = glm::vec2(
                        attrib.texcoords[2 * static_cast<size_t>(idx.texcoord_index) + 0],
                        attrib.texcoords[2 * static_cast<size_t>(idx.texcoord_index) + 1]);
                }

                material_to_vertices[material_id].push_back(vertex);
                // add index of vertex (within its by-material vertex vector) to its by-material index vector
                material_to_indices[material_id].push_back(material_to_vertices[material_id].size() - 1);
            }
            index_offset += fv;
        }
    }

    size_t mesh_index    = 0;
    auto   asset_manager = m_asset_manager.lock();
    CGX_ASSERT(asset_manager, "asset manager inaccessible");
    for (auto& [material_id, vertices] : material_to_vertices) {
        std::stringstream source_path_ss, tag_ss;
        tag_ss << source_path.stem().string() << "_mesh" << std::setw(3) << std::setfill('0') << mesh_index;
        source_path_ss << source_path.string() << ":" << tag_ss.str();
        mesh_index++;

        auto  material_asset = asset_manager->get_asset(m_mat_asset_ids[material_id]);
        auto  material       = std::dynamic_pointer_cast<Material>(material_asset);
        auto& indices        = material_to_indices[material_id];

        auto mesh = std::make_shared<Mesh>(tag_ss.str(), source_path_ss.str(), vertices, indices, material);

        auto mesh_asset_id = asset_manager->add_asset(mesh);
        m_mesh_asset_ids.push_back(mesh_asset_id);
    }
}

void AssetImporterOBJ::reset()
{
    m_tinyobj_reader = tinyobj::ObjReader();

    m_mat_asset_ids.clear();
    m_mesh_asset_ids.clear();
}
} // namespace cgx::resource
