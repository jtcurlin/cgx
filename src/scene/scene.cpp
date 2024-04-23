// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

#include "core/events/engine_events.h"
#include "ecs/event_handler.h"

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

Node* Scene::add_node(const ecs::Entity entity, const std::string& tag, Node* parent)
{
    CGX_INFO("Adding node  entity={} ; tag={}", entity, tag);
    auto node = std::make_shared<Node>(entity, tag);
    if (parent) {
        CGX_INFO(" >> parent specified - setting parent to {}", parent->get_id());
        node->set_parent(parent);
    }
    else {
        m_roots.push_back(std::move(node));
    }

    ecs::Event event(events::entity::ACQUIRED);
    event.set_param(events::entity::ID, entity);
    ecs::EventHandler::get_instance().send_event(event);

    return m_roots.back().get();
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
            if (const auto child_node = dynamic_cast<Node*>(child.get()) ; child_node) {
                m_roots.push_back(std::make_unique<Node>(*child_node));
            }
        }
        node->remove();
        m_roots.erase(root_node_it);
    }
    else {
        node->remove();
    }

    ecs::Event event(events::entity::RELEASED);
    event.set_param(events::entity::ID, entity);
    ecs::EventHandler::get_instance().send_event(event);
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
