// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

#include "cgx.h"
#include "core/event_handler.h"
#include "scene/camera_node.h"
#include "scene/mesh_node.h"
#include "scene/root_node.h"

namespace cgx::scene
{
Scene::Scene(std::string label) : m_label(std::move(label))
{
    m_root = std::make_shared<RootNode>("Scene " + m_label + " Root");
    CGX_INFO("scene '{}' : initialized", m_label);
}
Scene::~Scene() = default;

Node* Scene::get_root() const
{
    return m_root.get();
}

Node* Scene::add_node(const NodeType::Type type, std::string tag, const ecs::Entity entity, Node* parent)
{
    std::shared_ptr<Node> node;
    switch (type) {
        case NodeType::Type::Mesh: {
            node = std::make_shared<MeshNode>(std::move(tag), entity);
            break;
        }
        case NodeType::Type::Camera: {
            node = std::make_shared<CameraNode>(std::move(tag), entity);
            break;
        }
        default: {
            CGX_FATAL("Unknown node type specified");
        }
    }

    node->set_parent(parent);
    return node.get();
}

void Scene::remove_node(Node* node)
{
    CGX_ASSERT(node, "attempt to remove invalid node");
    CGX_ASSERT(node != m_root.get(), "attempted to remove scene root node");

    node->remove();
}

void Scene::remove_node_recursive(Node* node)
{
    CGX_ASSERT(node, "attempt to recursively remove invalid node");
    CGX_ASSERT(node != m_root.get(), "attempted to remove scene root node");

    node->recursive_remove();
}
}
