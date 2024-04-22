// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/scene.h"

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::scene
{
class SceneManager
{
public:
    explicit SceneManager(ecs::ECSManager* ecs_manager);
    ~SceneManager();

    [[nodiscard]] Node* add_node(Node* parent, const std::string& tag) const;

    void remove_node(Node* node) const;
    void remove_node_recursive(Node* node) const;

    Scene* add_scene(const std::string& label);
    void   remove_scene(const std::string& label);

    [[nodiscard]] Scene* get_active_scene() const;
    void                 set_active_scene(const std::string& label);

private:
    Scene*                                                  m_active_scene{nullptr};
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes{};

    ecs::ECSManager* m_ecs_manager;
};
}
