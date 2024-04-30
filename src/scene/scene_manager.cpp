// Copyright © 2024 Jacob Curlin

#include "scene/scene_manager.h"
#include "scene/scene_importer.h"

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

Node* SceneManager::add_node(const NodeType::Type type, std::string tag, Node* parent) const
{
    const auto entity = m_ecs_manager->acquire_entity();
    m_ecs_manager->add_component<component::Hierarchy>(entity, component::Hierarchy{});

    switch (type) {
        case NodeType::Type::Mesh: {
            m_ecs_manager->add_component<component::Render>(entity, component::Render{});
            m_ecs_manager->add_component<component::Transform>(entity, component::Transform{});
            break;
        }
        case NodeType::Type::Camera: {
            m_ecs_manager->add_component<component::Camera>(entity, component::Camera{});
            m_ecs_manager->add_component<component::Transform>(
                entity,
                component::Transform{.translation = glm::vec3(0.0f, 1.0f, 3.0f), .rotation=glm::vec3(-5.0f, 0.0f, 0.0f)});
            m_ecs_manager->add_component<component::Controllable>(entity, component::Controllable{.use_relative_movement=true});
            break;
        }
        default:
            CGX_FATAL("unsupported node type");
    }

    return m_active_scene->add_node(type, std::move(tag), entity, parent);
}

void SceneManager::remove_node(Node* node) const
{
    CGX_INFO("SceneManager : Removing Node '{}' (entity={})", node->get_tag(), node->get_entity());
    CGX_ASSERT(node, "attempt to remove invalid node");

    const auto entity = node->get_entity();
    m_active_scene->remove_node(node);
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

const std::unordered_map<std::string, std::unique_ptr<Scene>>& SceneManager::get_scenes()
{
    return m_scenes;
}

void SceneManager::import_scene(const std::string& path, Node* root) const
{
    if (root == nullptr) {
        root = get_active_scene()->get_root();
    }
    auto importer = SceneImporter(m_ecs_manager, m_asset_manager);
    importer.import(path, m_active_scene, root);
}
}
