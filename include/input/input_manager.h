// Copyright © 2024 Jacob Curlin

#pragma once

#include "input/input_types.h"
#include "event/event.h"

#include "core/common.h"

namespace cgx::core
{
class WindowManager;
}

namespace cgx::input
{
// ! SINGLETON

class InputManager
{
public:
    InputManager(const InputManager&)            = delete;
    InputManager& operator=(const InputManager&) = delete;

    static InputManager& GetSingleton()
    {
        static InputManager s_instance;
        return s_instance;
    }

    void initialize(const std::shared_ptr<core::WindowManager>& window_manager);

    // bind an event to a specific input
    void bind_key_input_event(Key key, KeyAction action, event::Event event);

    // for glfw to call
    void on_keyboard_input(Key key, KeyAction action);
    void on_mouse_button_input(Key key, KeyAction action);

    // keyboard polling
    [[nodiscard]] bool is_key_pressed(Key key) const;
    [[nodiscard]] bool is_key_released(Key key) const;

    // mouse button polling
    [[nodiscard]] bool is_mouse_button_pressed(Key button) const;
    [[nodiscard]] bool is_mouse_button_released(Key button) const;

    // mouse movement polling
    void get_mouse_position(double& x_pos, double& y_pos) const;
    void get_mouse_offset(double& x_offset, double& y_offset);

private:
    InputManager(); // default constructor (singleton)
    ~InputManager();

    bool m_initialized{false};

    std::shared_ptr<core::WindowManager>                     m_window_manager{};
    std::unordered_map<KeyInput, event::Event, KeyInputHash> m_event_bindings{};

    double m_mouse_x{0.0};
    double m_mouse_y{0.0};
    bool   m_first_mouse{true};
};
}
