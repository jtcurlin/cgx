// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

namespace cgx::scene
{
    SceneNode::SceneNode(std::string label, cgx::ecs::Entity entity_id)
        : Node(label)
        , m_entity(entity_id)
    {}

    SceneNode* Scene::AddNode(const std::string& label)
    {
        cgx::ecs::Entity entity = m_entity_registry->CreateEntity();

        auto node = std::make_unique<SceneNode>(label, entity);
        SceneNode* node_ptr = node.get();

        m_nodes.push_back(std::make_unique<SceneNode>(label, entity));
        return node_ptr;
    }

    void Scene::Initialize(
        std::shared_ptr<cgx::ecs::EntityManager> entity_registry,
        std::shared_ptr<cgx::ecs::ComponentManager> component_registry,
        std::shared_ptr<cgx::ecs::SystemManager> system_registry,
        std::shared_ptr<cgx::event::EventHandler> event_handler)
    {
        m_entity_registry = entity_registry;
        m_component_registry = component_registry;
        m_system_registry = system_registry;

        m_event_handler = event_handler;
    }
    
    void Scene::RemoveNode(SceneNode* node)
    {
        if (!node) { return; }
        auto entity = node->getEntity();

        m_entity_registry->DestroyEntity(entity);
        m_component_registry->EntityDestroyed(entity);
        m_system_registry->EntityDestroyed(entity);

        bool success = false;
        if (!node->getParent())
        {
            auto it = std::find_if(m_nodes.begin(), m_nodes.end(),
                [node](const std::unique_ptr<SceneNode>& root_node) 
                {
                    return root_node.get() == node;
                });
            if (it != m_nodes.end())
            {
                m_nodes.erase(it);
                success = true;
            }
        }

        if (!success && node->getParent())
        {
            auto parent = std::static_pointer_cast<SceneNode>(node->getParent());
            auto siblings = parent->getChildren();
            siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
                [node](Node* child)
                {
                    return child == node;
                }), 
                siblings.end());
        }
    }

    Scene::Scene(std::string label)
        : m_label(label)
    {}

} // namespace cgx::scene