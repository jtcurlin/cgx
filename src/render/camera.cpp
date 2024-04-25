// Copyright © 2024 Jacob Curlin

#include "render/camera.h"
#include "core/events/master_events.h"
#include "core/event_handler.h"
#include "core/input_manager.h"

namespace cgx::render
{
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position)
    , m_up(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
{
    updateCameraVectors();

    auto& event_handler = core::EventHandler::get_instance();
    event_handler.add_listener(
        core::event::master::ACTIVATE_GUI_CONTROL_MODE,
        [this](core::event::Event& event) {
            this->m_control_enabled = false;
        });
    event_handler.add_listener(
        core::event::master::ACTIVATE_GAME_CONTROL_MODE,
        [this](core::event::Event& event) {
            this->m_control_enabled = true;
        });
}

void Camera::update(const double dt)
{
    if (m_control_enabled) {
        auto& input_manager = core::InputManager::GetSingleton();

        double x_offset, y_offset;
        input_manager.get_mouse_offset(x_offset, y_offset);
        look(x_offset, y_offset, true);

        if (input_manager.is_key_pressed(core::Key::key_w)) {
            translate(kForward, dt);
        }
        if (input_manager.is_key_pressed(core::Key::key_s)) {
            translate(kBackward, dt);
        }
        if (input_manager.is_key_pressed(core::Key::key_a)) {
            translate(kLeft, dt);
        }
        if (input_manager.is_key_pressed(core::Key::key_d)) {
            translate(kRight, dt);
        }
        if (input_manager.is_key_pressed(core::Key::key_space)) {
            translate(kUp, dt);
        }
        if (input_manager.is_key_pressed(core::Key::key_left_ctrl)) {
            translate(kDown, dt);
        }
    }
    updateCameraVectors();
}

glm::mat4 Camera::get_view_matrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::get_zoom() const
{
    return m_zoom;
}

void Camera::translate(const TranslateDirection dir, const double dt)
{
    const float velocity = m_movement_speed * static_cast<float>(dt);
    switch (dir) {
        case kForward:
            m_position += m_front * velocity;
            break;
        case kBackward:
            m_position -= m_front * velocity;
            break;
        case kLeft:
            m_position -= m_right * velocity;
            break;
        case kRight:
            m_position += m_right * velocity;
            break;
        case kUp:
            m_position += m_up * velocity;
            break;
        case kDown:
            m_position -= m_up * velocity;
            break;
        default:
            break;
    };
}

void Camera::look(const double x_offset, const double y_offset, const GLboolean constrain_pitch = true)
{
    m_yaw += static_cast<float>(x_offset * m_mouse_sensitivity);
    m_pitch += static_cast<float>(y_offset * m_mouse_sensitivity);

    if (constrain_pitch) {
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::enable_control()
{
    m_control_enabled = true;

    auto& input_manager = core::InputManager::GetSingleton();

    // extra pre-call to getMouseOffset to 'reset' the current offset created by mouse movements
    // while camera control disabled
    double x_offset, y_offset;
    input_manager.get_mouse_offset(x_offset, y_offset);
}

void Camera::disable_control()
{
    m_control_enabled = false;
}

glm::vec3 Camera::get_cam_pos() const
{
    return m_position;
}
}
