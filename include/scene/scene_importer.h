// Copyright © 2024 Jacob Curlin

#pragma once

#include "tiny_gltf.h"

#include <filesystem>


namespace cgx::asset
{
class AssetManager;
class Model;
class Mesh;
class Material;
class Texture;
}

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::scene
{
class Node;
class Scene;

class SceneImporter
{
public:
    SceneImporter(ecs::ECSManager* ecs_manager, asset::AssetManager* asset_manager);
    ~SceneImporter();

    void import(const std::string& path, Node* parent=nullptr);

private:
    std::filesystem::path m_root_path = "";
    size_t m_node_count{0};
    size_t m_mesh_count{0};
    size_t m_submesh_count{0};
    size_t m_material_count{0};
    size_t m_texture_count{0};

    asset::AssetManager* m_asset_manager{nullptr};
    ecs::ECSManager*     m_ecs_manager{nullptr};

    void process_node(
        const tinygltf::Model& gltf_model,
        const tinygltf::Node&  gltf_node,
        Node*                  parent);

    std::shared_ptr<asset::Material> process_material(
        const tinygltf::Model&    gltf_model,
        const tinygltf::Material& gltf_material);

    std::vector<std::shared_ptr<asset::Mesh>> process_mesh(
        const tinygltf::Model& gltf_model,
        const tinygltf::Mesh&  gltf_mesh);

    std::shared_ptr<asset::Texture> process_texture(
        const tinygltf::Model&   gltf_model,
        const tinygltf::Texture& gltf_texture);

};
}
