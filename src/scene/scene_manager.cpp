// Copyright © 2024 Jacob Curlin

#include "scene/scene_manager.h"
#include "scene/scene_importer.h"

#include "asset/asset_manager.h"
#include "core/components/hierarchy.h"
#include "ecs/ecs_manager.h"

namespace cgx::scene
{
SceneManager::SceneManager(ecs::ECSManager* ecs_manager, asset::AssetManager* asset_manager)
    : m_ecs_manager(ecs_manager)
    , m_asset_manager(asset_manager)
{
    CGX_INFO("scene manager : initialized");
}

SceneManager::~SceneManager() = default;

Node* SceneManager::add_node(Node* parent, const std::string& tag) const
{
    const auto entity = m_ecs_manager->acquire_entity();
    m_ecs_manager->add_component<component::Hierarchy>(entity, component::Hierarchy{});
    return m_active_scene->add_node(entity, tag, parent);
}

void SceneManager::remove_node(Node* node) const
{
    CGX_ASSERT(node, "attempt to remove invalid node");

    m_ecs_manager->release_entity(node->get_entity());
    m_active_scene->remove_node(node);
}

void SceneManager::remove_node_recursive(Node* node) const
{
    CGX_ASSERT(node, "attempt to recursively remove invalid node");

    node->for_each(
        [this](core::Hierarchy& hierarchy) -> bool {
            if (const Node* casted_node = dynamic_cast<Node*>(&hierarchy) ; casted_node) {
                m_ecs_manager->release_entity(casted_node->get_entity());
            }
            return true;
        });

    m_ecs_manager->release_entity(node->get_entity());
    m_active_scene->remove_node_recursive(node);
}

Scene* SceneManager::add_scene(const std::string& label)
{
    const auto scene_it = m_scenes.find(label);
    CGX_ASSERT(scene_it == m_scenes.end(), "specified label is already associated with a scene");

    m_scenes[label] = std::make_unique<Scene>(label);
    return m_scenes[label].get();
}

void SceneManager::remove_scene(const std::string& label)
{
    const auto scene_it = m_scenes.find(label);
    CGX_ASSERT(scene_it != m_scenes.end(), "attempt to remove non-existent scene");

    m_scenes.erase(scene_it);
}

Scene* SceneManager::get_active_scene() const
{
    CGX_ASSERT(m_active_scene, "attempt to retreive active scene when no scene active");

    return m_active_scene;
}

void SceneManager::set_active_scene(const std::string& label)
{
    const auto scene_it = m_scenes.find(label);
    CGX_ASSERT(scene_it != m_scenes.end() && scene_it->second, "attempt to retreive non-existent scene");

    m_active_scene = scene_it->second.get();
}

void SceneManager::import_scene(const std::string& path) const
{
    auto importer = SceneImporter(m_ecs_manager, m_asset_manager);
    importer.import(path, m_active_scene);
}
}
