// Copyright © 2024 Jacob Curlin

#include "scene/scene_manager.h"

#include "ecs/entity_manager.h"
#include "ecs/component_manager.h"
#include "ecs/system_manager.h"

#include "event/event_handler.h"

namespace cgx::scene
{

    std::shared_ptr<Scene> SceneManager::AddScene(
        std::string label, 
        std::shared_ptr<cgx::ecs::EntityManager> entity_registry,
        std::shared_ptr<cgx::ecs::ComponentManager> component_registry,
        std::shared_ptr<cgx::ecs::SystemManager> system_registry,
        std::shared_ptr<cgx::event::EventHandler> event_handler)
    {
        auto scene = std::make_shared<Scene>(label);
        m_scenes[label] = scene;

        scene->Initialize(entity_registry,
                          component_registry,
                          system_registry,
                          event_handler);
        m_active_scene = scene;

        return m_active_scene;
    }

    bool SceneManager::RemoveScene(const std::string& label)
    {
        return m_scenes.erase(label) > 0;
    }

    std::shared_ptr<Scene> SceneManager::getScene(const std::string& label)
    {
        auto it = m_scenes.find(label);
        if (it != m_scenes.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void SceneManager::setActiveScene(const std::string& label)
    {
        auto scene = getScene(label);
        if (scene)
        {
            m_active_scene = scene;
        }
        else
        {
            CGX_ERROR("SceneManager::setActiveScene({}): Failed to find scene.", label);
        }
    }

    std::shared_ptr<Scene> SceneManager::getActiveScene() const
    {
        return m_active_scene;
    }

} // namespace cgx::scene