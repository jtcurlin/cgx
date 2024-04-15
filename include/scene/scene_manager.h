// Copyright © 2024 Jacob Curlin

#pragma once

#include "scene/scene.h"

namespace cgx::ecs
{
class EntityRegistry;
class ComponentRegistry;
class SystemRegistry;
}

namespace cgx::event
{
class EventHandler;
}

namespace cgx::scene
{
class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    std::shared_ptr<Scene> add_scene(
        std::string                                   label,
        const std::shared_ptr<ecs::EntityRegistry> &   entity_registry,
        const std::shared_ptr<ecs::ComponentRegistry> &component_registry,
        const std::shared_ptr<ecs::SystemRegistry> &   system_registry
    );

    [[nodiscard]] std::shared_ptr<Scene> get_active_scene() const;
    [[nodiscard]] std::shared_ptr<Scene> get_scene(const std::string &label) const;

    void set_active_scene(const std::string &label); ;
    bool remove_scene(const std::string &label);

private:
    std::shared_ptr<Scene>                                  m_active_scene;
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
};
}
