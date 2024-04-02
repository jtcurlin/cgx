// Copyright © 2024 Jacob Curlin

#include "input/input_manager.h"
#include "event/events/engine_events.h"

namespace cgx::input
{
    void InputManager::Initialize(
        std::shared_ptr<cgx::event::EventHandler> event_handler,
        std::shared_ptr<cgx::core::WindowManager> window_manager)
    {
        m_event_handler = event_handler;
        m_window_manager = window_manager;

        m_window_manager->setKeyCallback([this](Key key, KeyAction action)
        {
            this->onKeyboardInput(key, action);
        });

        m_window_manager->setMouseButtonCallback([this](Key key, KeyAction action)
        {
            this->onMouseButtonInput(key, action);
        });

        
        m_initialized = true;
    }

    void InputManager::BindKeyInputEvent(Key key, KeyAction action, cgx::event::Event event)
    {
        KeyInput key_input { key, action };
        m_event_bindings.emplace(key_input, event);
    }

    void InputManager::onKeyboardInput(Key key, KeyAction action) 
    {
        KeyInput key_input { key, action };
        auto it = m_event_bindings.find(key_input);
        if (it != m_event_bindings.end())
        {
            m_event_handler->SendEvent(it->second);
        }
    }

    void InputManager::onMouseButtonInput(Key key, KeyAction action)
    {
        KeyInput key_input { key, action };
        auto it = m_event_bindings.find(key_input);
        if (it != m_event_bindings.end())
        {
            m_event_handler->SendEvent(it->second);
        }
    }

    bool InputManager::isKeyPressed(Key key) const
    {
        return m_window_manager->isKeyPressed(key);
    }

    bool InputManager::isKeyReleased(Key key) const
    {
        return m_window_manager->isKeyReleased(key);
    }

    bool InputManager::isMouseButtonPressed(Key button) const
    {
        return m_window_manager->isMouseButtonPressed(button);
    }

    bool InputManager::isMouseButtonReleased(Key button) const
    {
        return m_window_manager->isMouseButtonReleased(button);
    }

    void InputManager::getMousePosition(double &x_pos, double &y_pos)
    {
        m_window_manager->getMousePosition(x_pos, y_pos);
    }

    void InputManager::getMouseOffset(double &x_offset, double &y_offset)
    {
        double x_pos, y_pos;
        getMousePosition(x_pos, y_pos);

        if (m_first_mouse)
        {
            m_mouse_x = x_pos;
            m_mouse_y = y_pos;
            m_first_mouse = false;
        }

        x_offset = x_pos - m_mouse_x;
        y_offset = m_mouse_y - y_pos;

        m_mouse_x = x_pos;
        m_mouse_y = y_pos;
    }

} // namespace cgx::input