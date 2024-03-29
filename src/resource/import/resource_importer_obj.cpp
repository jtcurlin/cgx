// Copyright © 2024 Jacob Curlin

#include "resource/import/resource_importer_obj.h"

#include "core/common.h"
#include "resource/model.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/texture.h"
#include "resource/resource_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include "glm/glm.hpp"

#include <sstream>
#include <iomanip>

namespace cgx::resource
{
    bool ResourceImporterOBJ::Initialize(const std::string& path)
    {
        // check source file path non-empty
        if (path.empty())
        {
            CGX_ERROR("ResourceImporterOBJ: failed to import file. (empty path provided)");
            return false;
        }

        // check source file path exists
        m_source_path = std::filesystem::path(path);  
        if (!std::filesystem::exists(m_source_path))
        {
            CGX_ERROR("ResourceImporter: failed to import file at provided path [{}]. " \
                      "(invalid file path)", m_source_path.string());
            return false;
        }

        // normalize path (convert '//', '\', etc.)
        m_source_path = m_source_path.make_preferred().string();

        m_mat_dir_path = m_source_path.parent_path();
        m_tinyobj_reader_config.mtl_search_path = m_mat_dir_path.string();

        // initialize / execute parsing w/ tinyobjreader
        if (!m_tinyobj_reader.ParseFromFile(m_source_path, m_tinyobj_reader_config))
        {
            CGX_ERROR("ResourceImporterOBJ: Initialization Failed. (tinyobjreader " \
                      "failed to parse file at path [{}]", m_source_path.string());

            if (!m_tinyobj_reader.Error().empty())
            {
                CGX_ERROR("ResourceImporterOBJ: {}", m_tinyobj_reader.Error());
            }
            return false;
        }
        if (m_enable_format_warnings && !m_tinyobj_reader.Warning().empty())
        {
            CGX_WARN("ResourceImporterOBJ: TinyObjReader : {}", m_tinyobj_reader.Warning());
        }
        m_initialized = true;
        return m_initialized;
    }

    void ResourceImporterOBJ::Reset()
    {
        m_source_path.clear();
        m_mat_dir_path.clear();

        m_tinyobj_reader_config = tinyobj::ObjReaderConfig();

        m_tinyobj_reader = tinyobj::ObjReader();

        m_mat_ruids.clear();
        m_mesh_ruids.clear();

        m_enable_format_warnings = false;
    }
    
    RUID ResourceImporterOBJ::Import()
    {
        ImportMaterials();
        ImportMeshes();

        auto& resource_manager = ResourceManager::getSingleton();

        // construct vector of Mesh shared ptr's from the stored Mesh RUID vector
        std::vector<std::shared_ptr<Mesh>> meshes;  
        for (auto& ruid : m_mesh_ruids)
        {
            meshes.push_back(resource_manager.getResource<Mesh>(ruid));
        }

        // construct Model resource
        auto model = std::make_shared<Model>(
            m_source_path.string(), 
            m_source_path.stem().string(),
            meshes
        );

        Reset(); // reset importer

        // register Model resource and return its resource UID.
        return resource_manager.RegisterResource<Model>(model, false);
    }

    void ResourceImporterOBJ::ImportMaterials()
    {
        auto& resource_manager = ResourceManager::getSingleton();
        auto& materials = m_tinyobj_reader.GetMaterials();

        std::stringstream path_ss, tag_ss;
        for (size_t mat_id=0; mat_id < materials.size(); mat_id++)
        {
            auto& mat = materials[mat_id];

            // build material's path and tag strings
            path_ss << m_source_path.string() << ":";
            tag_ss << m_source_path.stem().string() << "_"; 
            if (!mat.name.empty())
            {
                path_ss << "material_" << mat.name;
                tag_ss << mat.name;
            } 
            else 
            { 
                path_ss << "material_" << std::setw(3) << std::setfill('0') << mat_id;
                tag_ss << "material" << std::setw(3) << std::setfill('0') << mat_id; 
            }

            // parse & load material's corresponding texture resources
            std::shared_ptr<Texture> ambient_map, diffuse_map, specular_map, normal_map;

            std::filesystem::path tex_path; // temp for holding derived tex paths
            if (!mat.ambient_texname.empty()) 
            { 
                tex_path = (m_mat_dir_path / mat.ambient_texname).string(); 
                RUID id = resource_manager.ImportResource<Texture>(tex_path);
                ambient_map = resource_manager.getResource<Texture>(id);
            } else { ambient_map = nullptr; }

            if (!mat.diffuse_texname.empty()) 
            { 
                tex_path = (m_mat_dir_path / mat.diffuse_texname).string(); 
                RUID id = resource_manager.ImportResource<Texture>(tex_path);
                diffuse_map = resource_manager.getResource<Texture>(id);
            } else { diffuse_map = nullptr; }

            if (!mat.specular_texname.empty()) 
            {   
                tex_path = (m_mat_dir_path / mat.specular_texname).string(); 
                RUID id = resource_manager.ImportResource<Texture>(tex_path);
                specular_map = resource_manager.getResource<Texture>(id);
            } else {specular_map = nullptr; }

            if (!mat.bump_texname.empty()) 
            { 
                tex_path = (m_mat_dir_path / mat.bump_texname).string(); 
                RUID id = resource_manager.ImportResource<Texture>(tex_path);
                normal_map = resource_manager.getResource<Texture>(id);
            } else {normal_map = nullptr; }

            auto material = std::make_shared<Material>(
                path_ss.str(), tag_ss.str(), static_cast<float>(mat.shininess),
                glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]),
                glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]),
                glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]),
                ambient_map, diffuse_map, specular_map, normal_map
            );

            // initialize Material resource and load into ResourceManager
            RUID material_ruid = resource_manager.RegisterResource<Material>(material, false);

            // if ruid valid (successfully loaded/registered), add to tinyobj_mat_id -> resource UID map
            if (material_ruid != k_invalid_id) 
            {
                m_mat_ruids[mat_id] = material_ruid;
            }
            else
            {
                CGX_WARN("ResourceImporterOBJ: Failed to register material. (path : [{}])", path_ss.str());
            }

            path_ss.clear();
            tag_ss.clear();
        }

    }

    void ResourceImporterOBJ::ImportMeshes()
    {
        auto& resource_manager = ResourceManager::getSingleton();

        auto& attrib = m_tinyobj_reader.GetAttrib();
        auto& shapes = m_tinyobj_reader.GetShapes();
        
        std::vector<std::shared_ptr<Mesh>> meshes;

        std::unordered_map<unsigned int, std::vector<Vertex>> material_to_vertices;
        std::unordered_map<unsigned int, std::vector<uint32_t>> material_to_indices;

        // iterate shapes 
        // for (size_t s = 0; s < shapes.size(); s++)
        for (auto& shape : shapes)
        {
            size_t index_offset = 0;
            unsigned int material_id;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)                                                  // iterate faces
            {
                // check if material ID is new and initialize it in material maps if so
                material_id = shape.mesh.material_ids[f];
                if (material_to_vertices.find(material_id) == material_to_vertices.end())
                {
                    material_to_vertices[material_id] = std::vector<Vertex>();
                    material_to_indices[material_id] = std::vector<unsigned int>();
                }

                // process face's vertices
                size_t fv = shape.mesh.num_face_vertices[f];
                for (size_t v = 0; v < fv; v++)
                {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    Vertex vertex;

                    // get current vertex position vector {x,y,z}
                    vertex.position = glm::vec3(
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1], 
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2] 
                    );

                    // if present, get current vertex normal vector {x,y,z}
                    if (idx.normal_index >= 0)
                    {
                        vertex.normal = glm::vec3(
                            attrib.normals[3 * size_t(idx.normal_index) + 0],
                            attrib.normals[3 * size_t(idx.normal_index) + 1],
                            attrib.normals[3 * size_t(idx.normal_index) + 2]
                        );
                    }

                    // if present, get current vertex texture (uv) coordinates {u,v}
                    if (idx.texcoord_index >= 0)
                    {
                        vertex.texCoord = glm::vec2(
                            attrib.texcoords[2 * size_t(idx.texcoord_index) + 0],
                            attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]
                        );
                    }

                    material_to_vertices[material_id].push_back(vertex);      
                    // add index of vertex (within its by-material vertex vector) to its by-material index vector
                    material_to_indices[material_id].push_back(material_to_vertices[material_id].size() - 1);   
                }
                index_offset += fv;
            }
        }

        size_t mesh_index = 0;
        std::stringstream path_ss, tag_ss;
        for (auto& [material_id, vertices] : material_to_vertices) 
        {
            path_ss << m_source_path.string() << ":mesh_" << std::setw(3) << std::setfill('0') << mesh_index;
            tag_ss << m_source_path.stem().string() << "_mesh" << std::setw(3) << std::setfill('0') << mesh_index;
            mesh_index++;

            auto material = resource_manager.getResource<Material>(m_mat_ruids[material_id]);
            auto& indices = material_to_indices[material_id];

            auto mesh = std::make_shared<cgx::resource::Mesh>(
                path_ss.str(), tag_ss.str(),
                vertices, indices, material
            );

            path_ss.clear();
            tag_ss.clear();

            auto mesh_ruid = resource_manager.RegisterResource<Mesh>(mesh, false);
            m_mesh_ruids.push_back(mesh_ruid);
        }
    }


} // namespace cgx::resource