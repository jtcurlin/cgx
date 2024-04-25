// Copyright © 2024 Jacob Curlin

#pragma once

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE

#include "core/common.h"
#include "GLFW/glfw3.h"
#include <functional>

namespace cgx::core
{
struct WindowConfig
{
    uint32_t    height;
    uint32_t    width;
    std::string label;
};

using KeyCallback         = std::function<void(Key, KeyAction)>;
using MouseButtonCallback = std::function<void(Key, KeyAction)>;
using MouseMoveCallback   = std::function<void(double x_pos, double y_pos)>;
using ScrollCallback      = std::function<void(double x_offset, double y_offset)>;

class WindowManager
{
public:
    WindowManager(uint32_t width, uint32_t height, std::string label);
    ~WindowManager();

    void init();
    void update() const;

    [[nodiscard]] GLFWwindow* get_glfw_window() const
    {
        return m_window;
    }

    [[nodiscard]] bool is_key_pressed(Key key) const;
    [[nodiscard]] bool is_key_released(Key key) const;
    [[nodiscard]] bool is_mouse_button_pressed(Key button) const;
    [[nodiscard]] bool is_mouse_button_released(Key button) const;

    void get_mouse_position(double& x_pos, double& y_pos) const;

    void set_key_callback(const KeyCallback& cb);
    void set_cursor_pos_callback(const MouseMoveCallback& cb);
    void set_mouse_button_callback(const MouseButtonCallback& cb);
    void set_scroll_callback(const ScrollCallback& cb);

    void enable_cursor() const;
    void disable_cursor() const;

private:
    GLFWwindow*  m_window;
    WindowConfig m_config{};

    KeyCallback         m_key_callback;
    MouseMoveCallback   m_mouse_move_callback;
    MouseButtonCallback m_mouse_button_callback;
    ScrollCallback      m_scroll_callback;

    void        setup_glfw_callback() const;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static KeyAction translate_key_action_to_internal(int glfw_action);
    static Key       translate_key_code_to_internal(int glfw_key);
    static Key       translate_mouse_button_to_internal(int glfw_mouse_button);

    static int translate_key_action_to_glfw(KeyAction action);
    static int translate_key_code_to_glfw(Key key);
    static int translate_mouse_button_to_glfw(Key key);

    friend class InputPanel;
};
}
