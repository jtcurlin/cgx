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

#include "scene/node.h"

#include <stb/stb_image.h>

#include "core/components/camera.h"
#include "core/components/controllable.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


namespace cgx::scene
{
SceneImporter::SceneImporter(ecs::ECSManager* ecs_manager, asset::AssetManager* asset_manager)
    : m_ecs_manager{ecs_manager}
    , m_asset_manager{asset_manager} {}

SceneImporter::~SceneImporter() = default;

void SceneImporter::import(const std::string& path, Node* parent)
{
    m_root_path = path;

    tinygltf::Model    gltf_model;
    tinygltf::TinyGLTF gltf_context;
    std::string        err, warn;

    bool              success;
    const std::string extension = m_root_path.extension().string();
    if (extension == ".gltf") {
        success = gltf_context.LoadASCIIFromFile(&gltf_model, &err, &warn, m_root_path.string());
    }
    else if (extension == ".glb") {
        success = gltf_context.LoadBinaryFromFile(&gltf_model, &err, &warn, m_root_path.string());
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
        process_node(gltf_model, gltf_model.nodes[gltf_node], parent);
    }
}

void SceneImporter::process_node(
    const tinygltf::Model& gltf_model,
    const tinygltf::Node&  gltf_node,
    Node*                  parent)
{

    NodeFlag flags = NodeFlag::None;
    auto entity = m_ecs_manager->acquire_entity();

    m_ecs_manager->add_component<component::Hierarchy>(entity, component::Hierarchy{});
    if (gltf_node.camera >= 0) {
        flags = flags | NodeFlag::Camera;
        auto& gltf_camera = gltf_model.cameras[gltf_node.camera];

        component::Camera camera;

        if (gltf_camera.type == "perspective") {
            camera.type         = component::Camera::Type::Perspective;
            camera.fov          = static_cast<float>(gltf_camera.perspective.yfov) * 180.0f / glm::pi<float>();
            camera.aspect_ratio = static_cast<float>(gltf_camera.perspective.aspectRatio);
            camera.near_plane   = static_cast<float>(gltf_camera.perspective.znear);
            camera.far_plane    = static_cast<float>(gltf_camera.perspective.zfar);
        }
        else if (gltf_camera.type == "orthographic") {
            camera.type       = component::Camera::Type::Orthographic;
            camera.x_mag       = static_cast<float>(gltf_camera.orthographic.xmag);
            camera.y_mag       = static_cast<float>(gltf_camera.orthographic.ymag);
            camera.near_plane = static_cast<float>(gltf_camera.orthographic.znear);
            camera.far_plane  = static_cast<float>(gltf_camera.orthographic.zfar);
        }

        m_ecs_manager->add_component<component::Camera>(entity, camera);
        m_ecs_manager->add_component<component::Controllable>(entity, component::Controllable{});
    }
    if (gltf_node.mesh >= 0) {
        flags = flags | NodeFlag::Mesh;
        const auto& gltf_mesh = gltf_model.meshes[gltf_node.mesh];

        auto        meshes    = process_mesh(gltf_model, gltf_mesh);

        if (!meshes.empty()) {

            std::stringstream default_tag_ss, source_path_ss;
            static size_t     model_count = 0;

            default_tag_ss << "mesh/" << std::setw(3) << std::setfill('0') << std::to_string(++model_count);
            source_path_ss << m_root_path.string() << ":" << default_tag_ss.str();

            std::string tag = !gltf_mesh.name.empty() ? gltf_mesh.name : default_tag_ss.str();

            auto model_asset = std::make_shared<asset::Model>(tag, source_path_ss.str(), meshes);
            m_asset_manager->add_asset(model_asset);

            component::Render rc{};
            rc.model = model_asset;
            m_ecs_manager->add_component<component::Render>(entity, rc);
        }
    }

    component::Transform transform;

    if (!gltf_node.translation.empty()) {
        transform.translation = glm::vec3(
            static_cast<float>(gltf_node.translation[0]),
            static_cast<float>(gltf_node.translation[1]),
            static_cast<float>(gltf_node.translation[2]));
    }

    if (!gltf_node.rotation.empty()) {
        glm::quat quaternion(
            static_cast<float>(gltf_node.rotation[3]),
            // w
            static_cast<float>(gltf_node.rotation[0]),
            // x
            static_cast<float>(gltf_node.rotation[1]),
            // y
            static_cast<float>(gltf_node.rotation[2]) // z
        );

        glm::vec3 euler_angles = eulerAngles(quaternion);
        euler_angles           = degrees(euler_angles);
        transform.rotation               = euler_angles;
    }

    if (!gltf_node.scale.empty()) {
        transform.scale = glm::vec3(
            static_cast<float>(gltf_node.scale[0]),
            static_cast<float>(gltf_node.scale[1]),
            static_cast<float>(gltf_node.scale[2]));
    }

    transform.world_matrix = glm::mat4(1.0f);
    transform.dirty = true;

    m_ecs_manager->add_component<component::Transform>(entity, transform);

    std::string tag = !gltf_node.name.empty() ? gltf_node.name : "[Untagged Node]"; // todo: derive tag

    const auto node = std::make_shared<Node>(std::move(tag), entity, flags);
    node->set_parent(parent);


    for (const auto& child_node_index : gltf_node.children) {
        const auto& child_gltf_node = gltf_model.nodes[child_node_index];
        process_node(gltf_model, child_gltf_node, node.get());
    }
}

std::shared_ptr<asset::Material> SceneImporter::process_material(
    const tinygltf::Model&    gltf_model,
    const tinygltf::Material& gltf_material)
{
    std::stringstream path_ss;
    path_ss << m_root_path.string() << ":material_" << std::setw(3) << std::setfill('0') << m_material_count++;


    const std::string tag  = gltf_material.name;
    const std::string path = path_ss.str();

    if (auto id = m_asset_manager->get_id_by_path(path) ; id != asset::k_invalid_id) {
        return dynamic_pointer_cast<asset::Material>(m_asset_manager->get_asset(id));
    }

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
            tag,
            path,
            base_color_factor,
            metallic_factor,
            roughness_factor,
            base_color_map,
            metallic_roughness_map,
            normal_map,
            occlusion_map,
            emissive_map);
        m_asset_manager->add_asset(material_asset, true);
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

    if (!texture_source.image.empty()) {
        const auto& image_data = texture_source.image;
        const auto& image_width = texture_source.width;
        const auto& image_height = texture_source.height;
        const auto& image_component = texture_source.component;

        uint32_t num_channels = 0;
        switch (image_component) {
            case 1:
                num_channels = 1;
                break;
            case 2:
                num_channels = 2;
                break;
            case 3:
                num_channels = 3;
                break;
            case 4:
                num_channels = 4;
                break;
            default: CGX_ERROR("Unsupported image component: {}", image_component);
                return nullptr;
        }

        asset::Texture::Format format = asset::Texture::Format::Unsupported;
        switch (num_channels) {
            case 1:
                format = asset::Texture::Format::Red;
                break;
            case 2:
                format = asset::Texture::Format::RG;
                break;
            case 3:
                format = asset::Texture::Format::RGB;
                break;
            case 4:
                format = asset::Texture::Format::RGBA;
                break;
            default: {
                CGX_ERROR("Unsupported number of channels: {}", num_channels);
                return nullptr;
            }
        }

        std::string source_path = m_root_path.string() + ":textures/" + gltf_texture.name;

        auto texture = std::make_shared<asset::Texture>(
            gltf_texture.name,
            source_path,
            image_width,
            image_height,
            num_channels,
            format,
            asset::Texture::DataType::UnsignedByte,
            const_cast<unsigned char*>(image_data.data()));

        auto texture_asset_id = m_asset_manager->add_asset(texture, true);

        if (gltf_texture.sampler >= 0) {
            const auto& texture_sampler = gltf_model.samplers[gltf_texture.sampler];
            texture->set_min_filter(static_cast<asset::Texture::FilterMode>(texture_sampler.minFilter));
            texture->set_mag_filter(static_cast<asset::Texture::FilterMode>(texture_sampler.magFilter));
            texture->set_wrap_s(static_cast<asset::Texture::WrapMode>(texture_sampler.wrapS));
            texture->set_wrap_t(static_cast<asset::Texture::WrapMode>(texture_sampler.wrapT));
        }

        return dynamic_pointer_cast<asset::Texture>(m_asset_manager->get_asset(texture_asset_id));
    }
    else if (!texture_source.uri.empty()) {
        // The texture is external and referenced by URI
        const auto& texture_path = m_root_path.parent_path() / texture_source.uri;

        const auto& texture_asset_id = m_asset_manager->import_asset(texture_path.string());
        auto        texture = dynamic_pointer_cast<asset::Texture>(m_asset_manager->get_asset(texture_asset_id));

        CGX_VERIFY(texture != nullptr);

        // Set texture sampling parameters
        if (gltf_texture.sampler >= 0) {
            const auto& texture_sampler = gltf_model.samplers[gltf_texture.sampler];
            texture->set_min_filter(static_cast<asset::Texture::FilterMode>(texture_sampler.minFilter));
            texture->set_mag_filter(static_cast<asset::Texture::FilterMode>(texture_sampler.magFilter));
            texture->set_wrap_s(static_cast<asset::Texture::WrapMode>(texture_sampler.wrapS));
            texture->set_wrap_t(static_cast<asset::Texture::WrapMode>(texture_sampler.wrapT));
        }

        return texture;
    }
    else {
        // No texture data or URI found
        CGX_WARN("No texture data or URI found for texture index {}", gltf_texture.source);
        return nullptr;
    }
}

std::vector<std::shared_ptr<asset::Mesh>> SceneImporter::process_mesh(
    const tinygltf::Model& gltf_model,
    const tinygltf::Mesh&  gltf_mesh)
{
    std::vector<std::shared_ptr<asset::Mesh>> meshes;

    for (const auto& primitive : gltf_mesh.primitives) {
        static size_t     mesh_counter = 0;
        std::stringstream default_mesh_tag;
        default_mesh_tag << "sub_mesh_" << std::setw(3) << std::setfill('0') << ++mesh_counter;

        std::string mesh_tag = gltf_mesh.name;
        if (mesh_tag.empty()) {
            mesh_tag = default_mesh_tag.str();
        }
        std::string mesh_source_path = m_root_path.string() + ":" + default_mesh_tag.str();

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
                    indices.resize(indices_count);
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

void quat_to_euler(
    const double x,
    const double y,
    const double z,
    const double w,
    double&      roll,
    double&      pitch,
    double&      yaw)
{
    const double sinr_cosp = 2 * (w * x + y * z);
    const double cosr_cosp = 1 - 2 * (x * x + y * y);
    roll                   = std::atan2(sinr_cosp, cosr_cosp);

    const double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1) pitch = std::copysign(M_PI / 2, sinp);
    else pitch                     = std::asin(sinp);

    const double siny_cosp = 2 * (w * z + x * y);
    const double cosy_cosp = 1 - 2 * (y * y + z * z);
    yaw                    = std::atan2(siny_cosp, cosy_cosp);
}

}
