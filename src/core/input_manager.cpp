// Copyright © 2024 Jacob Curlin

#include "core/input_manager.h"
#include "core/event_handler.h"

#include "core/window_manager.h"

namespace cgx::core
{

InputManager::InputManager()  = default;
InputManager::~InputManager() = default;

void InputManager::initialize(const std::shared_ptr<WindowManager>& window_manager)
{
    m_window_manager = window_manager;

    m_window_manager->set_key_callback(
        [this](const Key key, const KeyAction action) {
            this->on_keyboard_input(key, action);
        });

    m_window_manager->set_mouse_button_callback(
        [this](const Key key, const KeyAction action) {
            this->on_mouse_button_input(key, action);
        });

    m_initialized = true;
}

void InputManager::bind_key_input_event(const Key key, const KeyAction action, event::Event event)
{
    KeyInput key_input{key, action};
    m_event_bindings.emplace(key_input, event);
}

void InputManager::on_keyboard_input(const Key key, const KeyAction action)
{
    const KeyInput key_input{key, action};
    if (const auto it = m_event_bindings.find(key_input) ; it != m_event_bindings.end()) {
        EventHandler::get_instance().send_event(it->second);
    }
}

void InputManager::on_mouse_button_input(const Key key, const KeyAction action)
{
    const KeyInput key_input{key, action};
    if (const auto it = m_event_bindings.find(key_input) ; it != m_event_bindings.end()) {
        EventHandler::get_instance().send_event(it->second);
    }
}

bool InputManager::is_key_pressed(const Key key) const
{
    return m_window_manager->is_key_pressed(key);
}

bool InputManager::is_key_released(const Key key) const
{
    return m_window_manager->is_key_released(key);
}

bool InputManager::is_mouse_button_pressed(const Key button) const
{
    return m_window_manager->is_mouse_button_pressed(button);
}

bool InputManager::is_mouse_button_released(const Key button) const
{
    return m_window_manager->is_mouse_button_released(button);
}

void InputManager::get_mouse_position(double& x_pos, double& y_pos) const
{
    m_window_manager->get_mouse_position(x_pos, y_pos);
}

void InputManager::get_mouse_offset(double& x_offset, double& y_offset)
{
    double x_pos, y_pos;
    get_mouse_position(x_pos, y_pos);

    if (m_reset_mouse) {
        m_mouse_x     = x_pos;
        m_mouse_y     = y_pos;
        m_reset_mouse = false;
        x_offset      = 0.0;
        y_offset      = 0.0;
    }
    else {
        x_offset  = x_pos - m_mouse_x;
        y_offset  = m_mouse_y - y_pos;
        m_mouse_x = x_pos;
        m_mouse_y = y_pos;
    }
}

std::unordered_map<KeyInput, event::Event, KeyInputHash>& InputManager::get_event_bindings()
{
    return m_event_bindings;
}

std::vector<Key>& InputManager::get_key_bindings()
{
    return m_key_bindings;
}

void InputManager::reset()
{
    m_reset_mouse = true;
}
}
