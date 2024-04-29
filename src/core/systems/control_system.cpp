// Copyright © 2024 Jacob Curlin

#define GLM_ENABLE_EXPERIMENTAL

#include "core/systems/control_system.h"

#include "core/components/controllable.h"
#include "core/components/transform.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "core/input_manager.h"
#include "core/events/master_events.h"

namespace cgx::core
{

ControlSystem::ControlSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager)
{
    auto& event_handler = EventHandler::get_instance();
    event_handler.add_listener(
        event::master::ACTIVATE_GUI_CONTROL_MODE,
        [this](event::Event& event) {
            this->m_enabled = false;
        });
    event_handler.add_listener(
        event::master::ACTIVATE_GAME_CONTROL_MODE,
        [this](event::Event& event) {
            InputManager::get_instance().reset();
            this->m_enabled = true;
        });
}

ControlSystem::~ControlSystem() = default;

void ControlSystem::update(const float dt)
{
    if (!m_enabled) { return; } // if control system disabled, no action

    for (auto& entity : m_entities) {

        if (m_ecs_manager->has_component<component::Transform>(entity) && m_ecs_manager->has_component<
                component::Controllable>(entity)) {
            auto& controllable = m_ecs_manager->get_component<component::Controllable>(entity);
            auto& transform    = m_ecs_manager->get_component<component::Transform>(entity);
            if (controllable.enable_translation) {
                update_position(transform, controllable.movement_speed, dt, controllable.use_relative_movement);
            }
            if (controllable.enable_rotation) {
                update_orientation(transform, controllable.rotation_speed, dt);
            }
        }
    }
}

void ControlSystem::update_position(component::Transform& transform, const glm::vec3& movement_speed, const float dt, bool relative)
{
    const auto& input_manager = InputManager::get_instance();

    glm::vec3 movement(0.0f);

    if (input_manager.is_key_pressed(Key::key_w)) {
        movement.z -= movement_speed.z * dt;
    }
    if (input_manager.is_key_pressed(Key::key_s)) {
        movement.z += movement_speed.z * dt;
    }
    if (input_manager.is_key_pressed(Key::key_a)) {
        movement.x -= movement_speed.x * dt;
    }
    if (input_manager.is_key_pressed(Key::key_d)) {
        movement.x += movement_speed.x * dt;
    }

    if (relative) {
        glm::mat4 rotation_matrix = glm::toMat4(glm::quat(glm::radians(transform.rotation)));
        movement = glm::vec3(rotation_matrix * glm::vec4(movement, 0.0f));
    }

    if (input_manager.is_key_pressed(Key::key_space)) {
        movement.y += movement_speed.y * dt;
    }
    if (input_manager.is_key_pressed(Key::key_left_ctrl)) {
        movement.y -= movement_speed.y * dt;
    }

    transform.dirty = true;
    transform.translation += movement;
}

void ControlSystem::update_orientation(component::Transform& transform, const glm::vec2& rotation_speed, const float dt)
{
    auto& input_manager = InputManager::get_instance();

    double x_offset, y_offset;
    input_manager.get_mouse_offset(x_offset, y_offset);

    transform.rotation.y -= static_cast<float>(x_offset * rotation_speed.x);
    transform.rotation.x += static_cast<float>(y_offset * rotation_speed.y);

    transform.dirty      = true;
    transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);
}

void ControlSystem::on_entity_added(ecs::Entity entity) {}
void ControlSystem::on_entity_removed(ecs::Entity entity) {}
}
