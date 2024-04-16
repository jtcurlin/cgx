// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/node.h"
#include "ecs/common.h"

#include "ecs/entity_registry.h"
#include "ecs/component_registry.h"
#include "ecs/system_registry.h"

#include <string>

namespace cgx::render
{
class Renderer;
}

namespace cgx::scene
{
class Scene
{
public:
    Scene(
        std::string                                    label,
        const std::shared_ptr<ecs::EntityRegistry>&    entity_registry,
        const std::shared_ptr<ecs::ComponentRegistry>& component_registry,
        const std::shared_ptr<ecs::SystemRegistry>&    system_registry);
    ~Scene();

    [[nodiscard]] const std::shared_ptr<Node>& get_root() const;

    void add_node(const std::string& tag, NodeType node_type, Node* parent = nullptr) const;

    void add_entity_node(const std::string& name) const;
    void add_entity_node(const std::string& name, Node* parent) const;

    // void remove_node(Node* node);

    template<typename T>
    void add_component(EntityNode* node, const T& component)
    {
        const auto entity = node->get_entity();
        m_component_registry->add_component<T>(entity, component);
        CGX_INFO("Scene: Added Component to entity {}", entity);

        auto signature = m_entity_registry->get_signature(entity);
        signature.set(m_component_registry->get_component_type<T>(), true);
        m_entity_registry->set_signature(entity, signature);

        m_system_registry->entity_signature_changed(entity, signature);
    }

    template<typename T>
    void remove_component(EntityNode* node) const
    {
        const auto entity = node->get_entity();
        m_component_registry->remove_component<T>(entity);

        auto signature = m_entity_registry->get_signature(entity);
        signature.set(m_component_registry->get_component_type<T>(), false);
        m_entity_registry->set_signature(entity, signature);

        m_system_registry->entity_signature_changed(entity, signature);
    }

    template<typename T>
    T& get_component(EntityNode* node)
    {
        const auto entity = node->get_entity();
        return m_component_registry->get_component<T>(entity);
    }

    template<typename T>
    [[nodiscard]] ecs::ComponentType get_component_type() const
    {
        return m_component_registry->get_component_type<T>();
    }

    template<typename T>
    bool has_component(EntityNode* node) const
    {
        const auto entity    = node->get_entity();
        const auto signature = m_entity_registry->get_signature(entity);
        return signature.test(m_component_registry->get_component_type<T>());
    }

private:
    std::string           m_label;
    std::shared_ptr<Node> m_root{};

    std::shared_ptr<ecs::EntityRegistry>    m_entity_registry;
    std::shared_ptr<ecs::ComponentRegistry> m_component_registry;
    std::shared_ptr<ecs::SystemRegistry>    m_system_registry;
};
}
