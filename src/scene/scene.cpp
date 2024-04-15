// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

namespace cgx::scene
{
Scene::Scene(
    std::string                                   label,
    const std::shared_ptr<ecs::EntityRegistry> &   entity_registry,
    const std::shared_ptr<ecs::ComponentRegistry> &component_registry,
    const std::shared_ptr<ecs::SystemRegistry> &   system_registry
)
    : m_label(std::move(label)),
      m_entity_registry{entity_registry},
      m_component_registry{component_registry},
      m_system_registry{system_registry}
{
    m_root = std::make_shared<Node>(NodeType::Unknown, "scene_root");
}

Scene::~Scene() = default;

const std::shared_ptr<Node>& Scene::get_root() const
{
    return m_root;
}

void Scene::add_node(const std::string& tag, const NodeType node_type, Node* parent) const
{
    auto* root = m_root.get();
    switch(node_type) {
        case NodeType::Camera: {
            const auto node = std::make_shared<CameraNode>(tag);
            if (parent) node->set_parent(parent);
            else node->set_parent(root);
            break;
        }
        case NodeType::Entity: {
            ecs::Entity entity = m_entity_registry->create_entity();
            const auto node = std::make_shared<EntityNode>(entity, tag);
            if (parent) node->set_parent(parent);
            else node->set_parent(root);
            break;
        }
        case NodeType::Light: {
            const auto node = std::make_shared<LightNode>(tag);
            if (parent) node->set_parent(parent);
            else node->set_parent(root);
            break;
        }
        default: {
            CGX_ERROR("invalid node type specified");
            std::exit(1);
        }
    }
}


void Scene::add_entity_node(const std::string &name) const
{
    auto *      root = m_root.get();
    ecs::Entity entity = m_entity_registry->create_entity();

    const auto node = std::make_shared<EntityNode>(entity, name);
    node->set_parent(root);
}

void Scene::add_entity_node(const std::string &name, Node* parent) const
{
    ecs::Entity entity = m_entity_registry->create_entity();

    const auto node = std::make_shared<EntityNode>(entity, name);
    node->set_parent(parent);
}
}
