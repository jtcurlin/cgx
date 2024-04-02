// Copyright © 2024 Jacob Curlin

#pragma once

#include "input/input_types.h"
#include "event/event.h"
#include "event/event_handler.h"

#include "core/common.h"
#include "core/window_manager.h"

namespace cgx::input
{
    // ! SINGLETON

    class InputManager
    {
    public:
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        ~InputManager() = default;

        static InputManager& GetSingleton()
        {
            static InputManager s_instance;
            return s_instance;
        }

        void Initialize(
            std::shared_ptr<cgx::event::EventHandler> event_handler,
            std::shared_ptr<cgx::core::WindowManager> window_manager
        );

        // bind an event to a specific input
        void BindKeyInputEvent(Key key, KeyAction action, cgx::event::Event event);

        // for glfw to call
        void onKeyboardInput(Key key, KeyAction action);
        void onMouseButtonInput(Key key, KeyAction action);

        // keyboard polling
        bool isKeyPressed(Key key) const;
        bool isKeyReleased(Key key) const;
        
        // mouse button polling
        bool isMouseButtonPressed(Key key) const;
        bool isMouseButtonReleased(Key key) const;

        // mouse movement polling
        void getMousePosition(double &x_pos, double &y_pos);
        void getMouseOffset(double &x_offset, double &y_offset);

    private:
        InputManager() = default;   // default constructor (singleton)

        bool m_initialized { false };

        std::shared_ptr<cgx::event::EventHandler> m_event_handler;
        std::shared_ptr<cgx::core::WindowManager> m_window_manager;

        std::unordered_map<KeyInput, cgx::event::Event, KeyInputHash> m_event_bindings;

        double  m_mouse_x       = 0.0;
        double  m_mouse_y       = 0.0;
        bool    m_first_mouse   = true;

    }; // class InputManager

} // namespace cgx::input

