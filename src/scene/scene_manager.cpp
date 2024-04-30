// Copyright © 2024 Jacob Curlin

#include "scene/scene_manager.h"
#include "scene/scene_importer.h"
#include "scene/node.h"

#include "asset/asset_manager.h"
#include "core/components/hierarchy.h"
#include "core/components/transform.h"
#include "core/components/render.h"
#include "core/components/camera.h"
#include "core/components/controllable.h"
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

Node* SceneManager::add_node(std::string tag, NodeFlag flags, Node* parent) const
{
    static size_t node_count = 0;
    std::string default_tag = "Empty Node";
    const auto new_entity = m_ecs_manager->acquire_entity();


    // ! every node gets a hierarchy component
    m_ecs_manager->add_component<component::Hierarchy>(new_entity, component::Hierarchy{});

    if (flags != NodeFlag::None) {
        default_tag = "Transform";
        m_ecs_manager->add_component<component::Transform>(new_entity, component::Transform{});
    }
    if (has_flag(flags, NodeFlag::Mesh)) {
        default_tag = "Mesh";
        m_ecs_manager->add_component<component::Render>(new_entity, component::Render{});
    }
    if (has_flag(flags, NodeFlag::Camera)) {
        default_tag = "Camera";
        m_ecs_manager->add_component<component::Camera>(new_entity, component::Camera{});
        m_ecs_manager->add_component<component::Controllable>(new_entity, component::Controllable{});
    }
    if (has_flag(flags, NodeFlag::Light)) {
        default_tag = "Light";
        // todo: light nodes
    }

    const auto new_node = std::make_shared<Node>(tag.empty() ? default_tag : tag, new_entity, flags);
    // if no parent specified, parent = root
    new_node->set_parent(parent ? parent : m_active_scene->get_root());

    return new_node.get();
}

void SceneManager::remove_node(Node* node) const
{
    CGX_ASSERT(node != m_active_scene->get_root(), "attempted to remove scene root node");

    const auto entity = node->get_entity();
    node->remove();
    m_ecs_manager->release_entity(entity);
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

    // todo: release entities for recursively removed children
    m_ecs_manager->release_entity(node->get_entity());
    node->recursive_remove();
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

const std::unordered_map<std::string, std::unique_ptr<Scene>>& SceneManager::get_scenes()
{
    return m_scenes;
}

void SceneManager::import_node(const std::string& path, Node* parent) const
{
    auto importer = SceneImporter(m_ecs_manager, m_asset_manager);
    importer.import(path, parent ? parent : m_active_scene->get_root());
}
}
