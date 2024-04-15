// Copyright © 2024 Jacob Curlin

#include "scene/scene_manager.h"

#include "ecs/entity_registry.h"
#include "ecs/component_registry.h"
#include "ecs/system_registry.h"

#include "event/event_handler.h"

namespace cgx::scene
{
SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

std::shared_ptr<Scene> SceneManager::add_scene(
    std::string                                   label,
    const std::shared_ptr<ecs::EntityRegistry> &   entity_registry,
    const std::shared_ptr<ecs::ComponentRegistry> &component_registry,
    const std::shared_ptr<ecs::SystemRegistry> &   system_registry
)
{
    const auto scene = std::make_shared<Scene>(
        label,
        entity_registry,
        component_registry,
        system_registry
    );
    m_scenes[label] = scene;


    m_active_scene = scene;

    return m_active_scene;
}

bool SceneManager::remove_scene(const std::string &label)
{
    return m_scenes.erase(label) > 0;
}

std::shared_ptr<Scene> SceneManager::get_scene(const std::string &label) const
{
    if (const auto it = m_scenes.find(label); it != m_scenes.end()) {
        return it->second;
    }
    return nullptr;
}

void SceneManager::set_active_scene(const std::string &label)
{
    if (const auto scene = get_scene(label); scene) {
        m_active_scene = scene;
    }
    else {
        CGX_ERROR("SceneManager::setActiveScene({}): Failed to find scene.", label);
    }
}

std::shared_ptr<Scene> SceneManager::get_active_scene() const
{
    return m_active_scene;
}
}
