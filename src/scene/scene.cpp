// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

#include "cgx.h"
#include "core/events/ecs_events.h"
#include "core/event_handler.h"
#include "scene/camera_node.h"
#include "scene/mesh_node.h"

namespace cgx::scene
{
Scene::Scene(std::string label) : m_label(std::move(label))
{
    CGX_INFO("scene '{}' : initialized", m_label);
}
Scene::~Scene() = default;

std::vector<Node*> Scene::get_roots() const
{
    std::vector<Node*> roots;
    for (auto& root : m_roots) {
        roots.push_back(root.get());
    }
    return roots;
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
            std::exit(1);
            break;
        }
    }

    Node* node_ptr = node.get();
    if (parent) {
        CGX_INFO(" >> parent specified - setting parent to {}", parent->get_id());
        node->set_parent(parent);
    }
    else {
        m_roots.push_back(std::move(node));
    }

    core::event::Event event(core::event::entity::ACQUIRED);
    event.set_param(core::event::entity::ID, entity);
    core::EventHandler::get_instance().send_event(event);

    // return m_roots.back().get();
    return node_ptr;
}

void Scene::remove_node(Node* node)
{
    CGX_ASSERT(node, "attempt to remove invalid node");
    auto entity = node->get_entity();

    auto root_node_it = std::find_if(
        m_roots.begin(),
        m_roots.end(),
        [node](const std::shared_ptr<Node>& root_node) {
            return root_node.get() == node;
        });

    if (root_node_it != m_roots.end()) {
        const std::vector<std::shared_ptr<core::Hierarchy>>& children = node->get_mutable_children();

        for (const auto&   child : children) {
            if (child->get_item_type() == core::ItemType::Node) {
                auto child_node = dynamic_cast<Node*>(child.get());
                if (child_node->get_node_type() == NodeType::Type::Mesh) {
                    auto child_mesh_node = dynamic_cast<MeshNode*>(child_node);
                    m_roots.push_back(std::make_unique<MeshNode>(*child_mesh_node));
                }
                else if (child_node->get_node_type() == NodeType::Type::Camera) {
                    auto child_camera_node = dynamic_cast<CameraNode*>(child_node);
                    m_roots.push_back(std::make_unique<CameraNode>(*child_camera_node));
                }
            }
        }
        node->remove();
        m_roots.erase(root_node_it);
    }
    else {
        node->remove();
    }

    core::event::Event event(core::event::entity::RELEASED);
    event.set_param(core::event::entity::ID, entity);
    core::EventHandler::get_instance().send_event(event);
}

void Scene::remove_node_recursive(Node* node)
{
    CGX_ASSERT(node, "attempt to recursively remove invalid node");

    auto root_node_it = std::find_if(
        m_roots.begin(),
        m_roots.end(),
        [node](const std::shared_ptr<Node>& root_node) {
            return root_node.get() == node;
        });

    node->recursive_remove();
    if (root_node_it != m_roots.end()) {
        m_roots.erase(root_node_it);
    }
}
}
