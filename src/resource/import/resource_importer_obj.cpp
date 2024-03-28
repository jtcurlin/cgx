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

#include <filesystem>
#include <sstream>
#include <iomanip>

namespace cgx::resource
{
    void ResourceImporterOBJ::Initialize(const std::string& path) 
    {
        m_source_path = path;
    }

    RUID ResourceImporterOBJ::Import()
    {
        CGX_ASSERT(!m_source_path.empty(), "ResourceImporterOBJ::Import() called with empty source path.");

        std::filesystem::path obj_file_path = m_source_path;
        std::filesystem::path mat_dir_path = obj_file_path.parent_path();

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(m_source_path, reader_config))
        {
            if (!reader.Error().empty())
            {
                CGX_ERROR("TinyObjReader: {}", reader.Error());
            }
            return false;
        }

        /* note: disabling warning messages for the moment since the syntax warnings are annoying as fuck
        if (!reader.Warning().empty())
        {
            CGX_WARN("[ResourceManager::loadModel] TinyObjReader: {}", reader.Warning())
        }
        */

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        std::unordered_map<unsigned int, std::shared_ptr<Material>> material_id_to_material;
        
        for (size_t src_mat_id = 0; src_mat_id < materials.size(); src_mat_id++)
        {
            auto src_mat = materials[src_mat_id];
            // create material id ( {.obj model path}_{material name} )
            std::ostringstream derived_path_stream;
            derived_path_stream << m_source_path << "_" << src_mat.name;
            std::string derived_path = derived_path_stream.str();

            glm::vec3 ambient_color, diffuse_color, specular_color;
            std::filesystem::path ambient_tex_path, diffuse_tex_path, specular_tex_path, normal_tex_path;
            float shininess;

            shininess = static_cast<float>(src_mat.shininess);
            ambient_color = glm::vec3(src_mat.ambient[0], src_mat.ambient[1], src_mat.ambient[2]);
            diffuse_color = glm::vec3(src_mat.diffuse[0], src_mat.diffuse[1], src_mat.diffuse[2]);
            specular_color = glm::vec3(src_mat.specular[0], src_mat.specular[1], src_mat.specular[2]);

            if (!src_mat.ambient_texname.empty()) { ambient_tex_path = mat_dir_path / src_mat.ambient_texname; }
            if (!src_mat.diffuse_texname.empty()) { diffuse_tex_path = mat_dir_path / src_mat.diffuse_texname; }
            if (!src_mat.specular_texname.empty()) { specular_tex_path = mat_dir_path / src_mat.specular_texname; } 
            if (!src_mat.bump_texname.empty()) { normal_tex_path = mat_dir_path / src_mat.bump_texname; }

            std::shared_ptr<Texture> ambient_map = !ambient_tex_path.empty() ? 
                ResourceManager::getSingleton().importResource<Texture>(ambient_tex_path.string()) : nullptr; 
            std::shared_ptr<Texture> diffuse_map = !diffuse_tex_path.empty() ? 
                ResourceManager::getSingleton().importResource<Texture>(diffuse_tex_path.string()) : nullptr; 
            std::shared_ptr<Texture> specular_map = !specular_tex_path.empty() ? 
                ResourceManager::getSingleton().importResource<Texture>(specular_tex_path.string()) : nullptr; 
            std::shared_ptr<Texture> normal_map = !normal_tex_path.empty() ? 
                ResourceManager::getSingleton().importResource<Texture>(normal_tex_path.string()) : nullptr; 

            std::shared_ptr<Material> material; 
            material = std::make_shared<Material>(
                m_source_path,
                derived_path,
                src_mat.name,
                ambient_color,
                diffuse_color,
                specular_color,
                shininess,
                ambient_map,
                diffuse_map,
                specular_map,
                normal_map
            );

            auto curr_material = ResourceManager::getSingleton().loadResource<Material>(material);
            material_id_to_material[src_mat_id] = curr_material;
        }

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
        for (auto& [material_id, vertices] : material_to_vertices) 
        {
            std::shared_ptr<Material> material = material_id_to_material[material_id];
            std::vector<unsigned int>& indices = material_to_indices[material_id];
            
            auto mesh = std::make_shared<cgx::resource::Mesh>(
                obj_file_path.string(), 
                obj_file_path.string() + "_mesh" + std::to_string(mesh_index), 
                "mesh " + std::to_string(mesh_index),
                vertices, indices, material);

            mesh = ResourceManager::getSingleton().loadResource<Mesh>(mesh);
            meshes.push_back(mesh);
        }

        std::shared_ptr<Model> model = std::make_shared<Model>(obj_file_path.string(), obj_file_path.filename().string(), meshes);
        return ResourceManager::getSingleton().loadResource<Model>(model)->getRUID();
        
    }


} // namespace cgx::resource