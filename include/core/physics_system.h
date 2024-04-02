// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"
#include "scene/scene.h"

namespace cgx::ecs { class ComponentManager; }

namespace cgx::core
{

    class PhysicsSystem : public cgx::ecs::System
    {
    public:
        PhysicsSystem(std::shared_ptr<cgx::ecs::ComponentManager> component_registry);

        void Update(float dt) override;

    private:
        std::shared_ptr<cgx::scene::Scene> m_scene;

    }; // class PhysicsSystem

} // namespace cgx::core