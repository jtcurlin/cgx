// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/scene.h"

namespace cgx::asset
{
class AssetManager;
}

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::scene
{
class SceneManager
{
public:
    explicit SceneManager(ecs::ECSManager* ecs_manager, asset::AssetManager* asset_manager);
    ~SceneManager();

    [[nodiscard]] Node* add_node(NodeType::Type type, std::string tag, Node* parent=nullptr) const;

    void remove_node(Node* node) const;
    void remove_node_recursive(Node* node) const;

    Scene* add_scene(const std::string& label);
    void   remove_scene(const std::string& label);

    [[nodiscard]] Scene* get_active_scene() const;
    void                 set_active_scene(const std::string& label);

    const std::unordered_map<std::string, std::unique_ptr<Scene>>& get_scenes();

    void import_scene(const std::string& path, Node* root = nullptr) const;

private:
    Scene*                                                  m_active_scene{nullptr};
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes{};

    asset::AssetManager* m_asset_manager;
    ecs::ECSManager*     m_ecs_manager;
};
}
