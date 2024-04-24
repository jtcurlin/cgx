// Copyright © 2024 Jacob Curlin

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "scene/scene_importer.h"

#include "asset/asset_manager.h"
#include "asset/model.h"
#include "asset/mesh.h"
#include "asset/pbr_material.h"
#include "asset/texture.h"

#include "core/components/hierarchy.h"
#include "core/components/transform.h"
#include "core/components/render.h"

#include "ecs/ecs_manager.h"

#include "scene/scene.h"
#include "scene/node.h"

#include "stb_image.h"

namespace cgx::scene
{
SceneImporter::SceneImporter(ecs::ECSManager* ecs_manager, asset::AssetManager* asset_manager)
    : m_ecs_manager{ecs_manager}
    , m_asset_manager{asset_manager} {}

SceneImporter::~SceneImporter() = default;

void SceneImporter::import(const std::string& path, Scene* scene)
{
    m_curr_path = path;

    tinygltf::Model    gltf_model;
    tinygltf::TinyGLTF gltf_context;
    std::string        err, warn;

    bool        success;
    std::string extension = m_curr_path.extension().string();
    if (extension == ".gltf") {
        success = gltf_context.LoadASCIIFromFile(&gltf_model, &err, &warn, m_curr_path.string());
    }
    else if (extension == ".glb") {
        success = gltf_context.LoadBinaryFromFile(&gltf_model, &err, &warn, m_curr_path.string());
    }
    else {
        CGX_ERROR("Unsupported file extension: {}", extension);
        return;
    }

    if (!success) {
        CGX_ERROR("Failed to load glTF file: {}", err);
        return;
    }


    for (const auto& gltf_node : gltf_model.scenes[gltf_model.defaultScene].nodes) {
        process_node(gltf_model, gltf_model.nodes[gltf_node], nullptr, scene);
    }
}

void SceneImporter::process_node(
    const tinygltf::Model& gltf_model,
    const tinygltf::Node&  gltf_node,
    Node*                  parent_node,
    Scene*                 scene)
{
    const std::string node_tag = "todo"; // todo

    auto new_node_entity = m_ecs_manager->acquire_entity();
    m_ecs_manager->add_component<component::Hierarchy>(new_node_entity, component::Hierarchy{});
    auto node = scene->add_node(new_node_entity, node_tag, parent_node);

    glm::vec3 translation(0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(1.0f);

    if (!gltf_node.translation.empty()) {
        translation = glm::vec3(
            static_cast<float>(gltf_node.translation[0]),
            static_cast<float>(gltf_node.translation[1]),
            static_cast<float>(gltf_node.translation[2]));
    }

    if (!gltf_node.rotation.empty()) {
        rotation = glm::vec3(
            static_cast<float>(gltf_node.rotation[0]),
            static_cast<float>(gltf_node.rotation[1]),
            static_cast<float>(gltf_node.rotation[2]));
    }

    if (!gltf_node.scale.empty()) {
        scale = glm::vec3(
            static_cast<float>(gltf_node.scale[0]),
            static_cast<float>(gltf_node.scale[1]),
            static_cast<float>(gltf_node.scale[2]));
    }

    component::Transform tc = {
        .translate = translation, .rotate = rotation, .scale = scale, .world_matrix = glm::mat4(1.0f), .dirty = true
    };
    m_ecs_manager->add_component<component::Transform>(node->get_entity(), tc);

    if (gltf_node.mesh >= 0) {
        const auto& gltf_mesh = gltf_model.meshes[gltf_node.mesh];
        auto        meshes    = process_mesh(gltf_model, gltf_mesh);

        if (!meshes.empty()) {
            static size_t     model_count = 0;
            std::stringstream tag_ss, source_path_ss;
            tag_ss << std::setw(3) << std::setfill('0') << ++model_count;
            source_path_ss << m_curr_path.string() << ":" << tag_ss.str();
            auto model_asset    = std::make_shared<asset::Model>(tag_ss.str(), source_path_ss.str(), meshes);
            auto model_asset_id = m_asset_manager->add_asset(model_asset);

            component::Render rc{};
            rc.model = model_asset;
            m_ecs_manager->add_component<component::Render>(node->get_entity(), rc);
        }
    }

    for (const auto& child_node_index : gltf_node.children) {
        const auto& child_gltf_node = gltf_model.nodes[child_node_index];
        process_node(gltf_model, child_gltf_node, node, scene);
    }
}

std::shared_ptr<asset::Material> SceneImporter::process_material(
    const tinygltf::Model&    gltf_model,
    const tinygltf::Material& gltf_material)
{
    const std::string& material_tag  = gltf_material.name; // todo
    const std::string& material_path = m_curr_path.string() + material_tag;

    if (!gltf_material.values.empty()) {

        glm::vec4 base_color_factor(1.0f);
        float     metallic_factor  = 1.0f;
        float     roughness_factor = 1.0f;

        auto base_color_factor_it = gltf_material.values.find("baseColorFactor");
        if (base_color_factor_it != gltf_material.values.end()) {
            const auto& color_factor = base_color_factor_it->second.ColorFactor();
            base_color_factor        = glm::vec4(color_factor[0], color_factor[1], color_factor[2], color_factor[3]);
        }

        auto metallic_factor_it = gltf_material.values.find("metallicFactor");
        if (metallic_factor_it != gltf_material.values.end()) {
            metallic_factor = static_cast<float>(metallic_factor_it->second.Factor());
        }

        auto roughness_factor_it = gltf_material.values.find("roughnessFactor");
        if (roughness_factor_it != gltf_material.values.end()) {
            roughness_factor = static_cast<float>(roughness_factor_it->second.Factor());
        }

        std::shared_ptr<asset::Texture> base_color_map         = nullptr;
        std::shared_ptr<asset::Texture> metallic_roughness_map = nullptr;
        std::shared_ptr<asset::Texture> normal_map             = nullptr;
        std::shared_ptr<asset::Texture> occlusion_map          = nullptr;
        std::shared_ptr<asset::Texture> emissive_map           = nullptr;

        auto base_color_texture_it = gltf_material.values.find("baseColorTexture");
        if (base_color_texture_it != gltf_material.values.end()) {
            const auto& gltf_texture = gltf_model.textures[base_color_texture_it->second.TextureIndex()];
            base_color_map           = process_texture(gltf_model, gltf_texture);
        }

        auto metallic_roughness_texture_it = gltf_material.values.find("metallicRoughnessTexture");
        if (metallic_roughness_texture_it != gltf_material.values.end()) {
            const auto& gltf_texture = gltf_model.textures[metallic_roughness_texture_it->second.TextureIndex()];
            metallic_roughness_map   = process_texture(gltf_model, gltf_texture);
        }

        auto normal_texture_it = gltf_material.additionalValues.find("normalTexture");
        if (normal_texture_it != gltf_material.additionalValues.end()) {
            const auto& gltf_texture = gltf_model.textures[normal_texture_it->second.TextureIndex()];
            normal_map               = process_texture(gltf_model, gltf_texture);
        }

        auto occlusion_texture_it = gltf_material.additionalValues.find("occlusionTexture");
        if (occlusion_texture_it != gltf_material.additionalValues.end()) {
            const auto& gltf_texture = gltf_model.textures[occlusion_texture_it->second.TextureIndex()];
            occlusion_map            = process_texture(gltf_model, gltf_texture);
        }

        auto emissive_texture_it = gltf_material.additionalValues.find("emissiveTexture");
        if (emissive_texture_it != gltf_material.additionalValues.end()) {
            const auto& gltf_texture = gltf_model.textures[emissive_texture_it->second.TextureIndex()];
            emissive_map             = process_texture(gltf_model, gltf_texture);
        }

        auto material_asset = std::make_shared<asset::PBRMaterial>(
            material_tag,
            material_path,
            base_color_factor,
            metallic_factor,
            roughness_factor,
            base_color_map,
            metallic_roughness_map,
            normal_map,
            occlusion_map,
            emissive_map);
        m_asset_manager->add_asset(material_asset);
        return material_asset;
    }
    CGX_INFO("(todo) No material value specified");
    return nullptr;
}

std::shared_ptr<asset::Texture> SceneImporter::process_texture(
    const tinygltf::Model&   gltf_model,
    const tinygltf::Texture& gltf_texture)
{
    const auto& texture_source = gltf_model.images[gltf_texture.source];
    const auto& texture_path   = m_curr_path.parent_path() / texture_source.uri;

    const auto& texture_asset_id = m_asset_manager->import_asset(texture_path.string());
    auto        texture          = dynamic_pointer_cast<asset::Texture>(m_asset_manager->get_asset(texture_asset_id));

    CGX_VERIFY(texture != nullptr);

    if (gltf_texture.sampler >= 0) {
        const auto& texture_sampler = gltf_model.samplers[gltf_texture.sampler];
        texture->set_min_filter(static_cast<asset::FilterMode>(texture_sampler.minFilter));
        texture->set_mag_filter(static_cast<asset::FilterMode>(texture_sampler.magFilter));
        texture->set_wrap_s(static_cast<asset::WrapMode>(texture_sampler.wrapS));
        texture->set_wrap_t(static_cast<asset::WrapMode>(texture_sampler.wrapT));
    }

    return texture;
}

std::vector<std::shared_ptr<asset::Mesh>> SceneImporter::process_mesh(
    const tinygltf::Model& gltf_model,
    const tinygltf::Mesh&  gltf_mesh)
{
    std::vector<std::shared_ptr<asset::Mesh>> meshes;

    for (const auto& primitive : gltf_mesh.primitives) {
        std::string mesh_tag = gltf_mesh.name;
        if (mesh_tag.empty()) {
            static size_t     mesh_counter = 0;
            std::stringstream default_mesh_tag;
            default_mesh_tag << "mesh_" << std::setw(3) << std::setfill('0') << ++mesh_counter;
            mesh_tag = default_mesh_tag.str();
        }
        std::string mesh_source_path = m_curr_path.string() + ":" + mesh_tag;

        std::vector<asset::Vertex> vertices;
        std::vector<uint32_t>      indices;

        // Process vertex positions
        const auto& positions_accessor    = gltf_model.accessors[primitive.attributes.at("POSITION")];
        const auto& positions_buffer_view = gltf_model.bufferViews[positions_accessor.bufferView];
        const auto& positions_buffer      = gltf_model.buffers[positions_buffer_view.buffer];
        const auto* positions_data        = positions_buffer.data.data() + positions_buffer_view.byteOffset +
                                            positions_accessor.byteOffset;
        const auto positions_count  = positions_accessor.count;
        const auto positions_stride = positions_accessor.ByteStride(positions_buffer_view);

        for (size_t i = 0 ; i < positions_count ; ++i) {
            const auto* position_data = positions_data + i * positions_stride;
            glm::vec3   position;
            std::memcpy(&position, position_data, sizeof(glm::vec3));
            vertices.push_back({position});
        }

        // Process vertex normals (if available)
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
            const auto& normals_accessor = gltf_model.accessors[primitive.attributes.at("NORMAL")];
            const auto& normals_buffer_view = gltf_model.bufferViews[normals_accessor.bufferView];
            const auto& normals_buffer = gltf_model.buffers[normals_buffer_view.buffer];
            const auto* normals_data = normals_buffer.data.data() + normals_buffer_view.byteOffset + normals_accessor.
                                       byteOffset;
            const auto normals_count  = normals_accessor.count;
            const auto normals_stride = normals_accessor.ByteStride(normals_buffer_view);

            for (size_t i = 0 ; i < normals_count ; ++i) {
                const auto* normal_data = normals_data + i * normals_stride;
                glm::vec3   normal;
                std::memcpy(&normal, normal_data, sizeof(glm::vec3));
                vertices[i].normal = normal;
            }
        }

        // Process vertex texture coordinates (if available)
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const auto& texcoords_accessor    = gltf_model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& texcoords_buffer_view = gltf_model.bufferViews[texcoords_accessor.bufferView];
            const auto& texcoords_buffer      = gltf_model.buffers[texcoords_buffer_view.buffer];
            const auto* texcoords_data        = texcoords_buffer.data.data() + texcoords_buffer_view.byteOffset +
                                                texcoords_accessor.byteOffset;
            const auto texcoords_count  = texcoords_accessor.count;
            const auto texcoords_stride = texcoords_accessor.ByteStride(texcoords_buffer_view);

            for (size_t i = 0 ; i < texcoords_count ; ++i) {
                const auto* texcoord_data = texcoords_data + i * texcoords_stride;
                glm::vec2   texcoord;
                std::memcpy(&texcoord, texcoord_data, sizeof(glm::vec2));
                vertices[i].uv = texcoord;
            }
        }

        // Process indices (if available)
        if (primitive.indices >= 0) {
            const auto& indices_accessor = gltf_model.accessors[primitive.indices];
            const auto& indices_buffer_view = gltf_model.bufferViews[indices_accessor.bufferView];
            const auto& indices_buffer = gltf_model.buffers[indices_buffer_view.buffer];
            const auto* indices_data = indices_buffer.data.data() + indices_buffer_view.byteOffset + indices_accessor.
                                       byteOffset;
            const auto indices_count          = indices_accessor.count;
            const auto indices_component_type = indices_accessor.componentType;

            switch (indices_component_type) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    for (size_t i = 0 ; i < indices_count ; ++i) {
                        const auto index = static_cast<uint32_t>(*(indices_data + i));
                        indices.push_back(index);
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    for (size_t i = 0 ; i < indices_count ; ++i) {
                        const auto index = static_cast<uint32_t>(reinterpret_cast<const uint16_t*>(indices_data)[i]);
                        indices.push_back(index);
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    std::memcpy(indices.data(), indices_data, indices_count * sizeof(uint32_t));
                    break;
                default: CGX_ERROR("Unsupported index component type: {}", indices_component_type);
                    continue;
            }
        }

        auto mesh = std::make_shared<asset::Mesh>(mesh_tag, mesh_source_path, vertices, indices);

        if (primitive.material >= 0) {
            const auto& gltf_material = gltf_model.materials[primitive.material];
            auto        material      = process_material(gltf_model, gltf_material);
            mesh->set_material(material);
        }
        m_asset_manager->add_asset(mesh);

        meshes.push_back(mesh);
    }

    return meshes;
}
}
