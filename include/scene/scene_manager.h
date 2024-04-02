// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "scene/scene.h"

namespace cgx::ecs { class EntityManager; class ComponentManager; class SystemManager; }
namespace cgx::event { class EventHandler; }

namespace cgx::scene
{
    class SceneManager 
    {
    public:
        SceneManager() = default;
        
        std::shared_ptr<Scene> AddScene(
            std::string label,
            std::shared_ptr<cgx::ecs::EntityManager> entity_registry,
            std::shared_ptr<cgx::ecs::ComponentManager> component_registry,
            std::shared_ptr<cgx::ecs::SystemManager> system_registry,
            std::shared_ptr<cgx::event::EventHandler> event_handler
        );
        bool RemoveScene(const std::string& label);

        std::shared_ptr<Scene> getScene(const std::string& label); 

        void setActiveScene(const std::string& label); ;

        std::shared_ptr<Scene> getActiveScene() const; 

    private:
        std::shared_ptr<Scene> m_active_scene; 
        std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

    }; // class SceneManager

} // namespace cgx::scene