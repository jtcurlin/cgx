// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"

#include <glm/glm.hpp>

namespace cgx::component
{
struct Transform;
struct Camera;
}

namespace cgx::core
{
class ControlSystem final : public ecs::System
{
public:
    explicit ControlSystem(ecs::ECSManager* ecs_manager);
    ~ControlSystem() override;

    void frame_update(float dt) override;
    void fixed_update(float dt) override;

    void update_position(component::Transform& transform, const glm::vec3& movement_speed, float dt, bool relative);
    void update_orientation(component::Transform& transform, const glm::vec2& rotation_speed, float dt);

    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;

private:
    bool m_enabled{false};

    bool m_forward{false};
    bool m_backward{false};
    bool m_left{false};
    bool m_right{false};
    bool m_up{false};
    bool m_down{false};

};
}