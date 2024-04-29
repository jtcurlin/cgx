// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

#include "cgx.h"
#include "core/events/ecs_events.h"
#include "core/event_handler.h"
#include "scene/camera_node.h"
#include "scene/mesh_node.h"
#include "scene/root_node.h"

namespace cgx::scene
{
Scene::Scene(std::string label) : m_label(std::move(label))
{
    m_root = std::make_shared<RootNode>("Scene " + label + " Root");
    CGX_INFO("scene '{}' : initialized", m_label);
}
Scene::~Scene() = default;

Node* Scene::get_root() const
{
    return m_root.get();
}

Node* Scene::add_node(const NodeType::Type type, std::string tag, const ecs::Entity entity, Node* parent)
{
    CGX_INFO("Adding node  entity={} ; tag={}; type={}", entity, tag, NodeType::get_lower_typename(type));

    std::shared_ptr<Node> node;
    switch (type) {
        case NodeType::Type::Mesh: {
            node = std::make_shared<MeshNode>(tag, entity);
            break;
        }
        case NodeType::Type::Camera: {
            node = std::make_shared<CameraNode>(tag, entity);
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
