// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"
#include "ecs/common.h"

#include "ecs/entity_manager.h"
#include "ecs/component_manager.h"
#include "ecs/system_manager.h"

#include "event/event_handler.h"

#include <string>
#include <memory>

namespace cgx::render
{
    class Renderer;
}

namespace cgx::scene
{
    class SceneNode : public Node
    {
    public:
        SceneNode(std::string label, cgx::ecs::Entity entity_id);

        const cgx::ecs::Entity getEntity() { return m_entity; }

    private:
        cgx::ecs::Entity m_entity;

    }; // class SceneNode

    class Scene
    {
    public:
        Scene(std::string label);
        ~Scene() = default;

        void Initialize(
            std::shared_ptr<cgx::ecs::EntityManager> entity_registry,
            std::shared_ptr<cgx::ecs::ComponentManager> component_registry,
            std::shared_ptr<cgx::ecs::SystemManager> system_registry,
            std::shared_ptr<cgx::event::EventHandler> event_handler
        );

        SceneNode* AddNode(const std::string& label);
        void RemoveNode(SceneNode* node);

        template<typename T>
        void AddComponent(SceneNode* node, const T& component)
        {
            auto entity = node->getEntity();
            m_component_registry->AddComponent<T>(entity, component);

            auto signature = m_entity_registry->GetSignature(entity);
            signature.set(m_component_registry->GetComponentType<T>(), true);
            m_entity_registry->SetSignature(entity, signature);

            m_system_registry->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void RemoveComponent(SceneNode* node)
        {
            auto entity = node->getEntity();
            m_component_registry->RemoveComponent<T>(entity);

            auto signature = m_entity_registry->GetSignature(entity);
            signature.set(m_component_registry->GetComponentType<T>(), false);
            m_entity_registry->SetSignature(entity, signature);

            m_system_registry->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        T& GetComponent(SceneNode* node, T component)
        {
            auto entity = node->getEntity();
            return m_component_registry->GetComponent<T>(entity);
        }

        template<typename T>
        cgx::ecs::ComponentType GetComponentType()
        {
            return m_component_registry->GetComponentType<T>();
        }

        template<typename T>
        bool HasComponent(SceneNode* node)
        {
            auto entity = node->getEntity();
            auto signature = m_entity_registry->GetSignature(entity);
            return signature.test(m_component_registry->GetComponentType<T>());
        }

    private:
        std::string                                  m_label;
        std::vector<std::unique_ptr<SceneNode>>      m_nodes;

        std::shared_ptr<cgx::ecs::EntityManager>    m_entity_registry;
        std::shared_ptr<cgx::ecs::ComponentManager> m_component_registry;
        std::shared_ptr<cgx::ecs::SystemManager>    m_system_registry;

        std::shared_ptr<cgx::event::EventHandler>   m_event_handler;

    }; // class Scene

} // namespace cgx::scene