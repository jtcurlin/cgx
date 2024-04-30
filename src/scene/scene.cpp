// Copyright © 2024 Jacob Curlin

#include "scene/scene.h"

#include "cgx.h"
#include "core/event_handler.h"
#include "scene/node.h"

namespace cgx::scene
{
Scene::Scene(std::string label) : m_label(std::move(label))
{
    m_root = std::make_shared<Node>("root", ecs::MAX_ENTITIES, NodeFlag::None);
    CGX_INFO("scene '{}' : initialized", m_label);
}
Scene::~Scene() = default;

Node* Scene::get_root() const
{
    return m_root.get();
}

const std::string& Scene::get_label() const
{
    return m_label;
}
}
