// Copyright © 2024 Jacob Curlin

#include "core/window_manager.h"

#include "ecs/common.h"
#include "glad/glad.h"

#include <iostream>

namespace cgx::core
{
WindowManager::WindowManager(uint32_t width, uint32_t height, const std::string& label)
    : m_window(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), label.c_str(), nullptr, nullptr);
    if (m_window == nullptr) {
        glfwTerminate();
        CGX_CRITICAL("Failed to create GLFW window.");
        throw std::runtime_error("[window] Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, this);
    setup_glfw_callback();

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        CGX_CRITICAL("Failed to initialize GLAD.");
        exit(1);
    }


    CGX_INFO("WindowManager initialized [window '{}' : {} x {}]", label, width, height);
}

WindowManager::~WindowManager()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowManager::update() const
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void WindowManager::enable_cursor() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowManager::disable_cursor() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::get_mouse_position(double& x_pos, double& y_pos) const
{
    glfwGetCursorPos(m_window, &x_pos, &y_pos);
}

bool WindowManager::is_key_pressed(const core::Key key) const
{
    return glfwGetKey(m_window, translate_key_code_to_glfw(key)) == GLFW_PRESS;
}

bool WindowManager::is_key_released(const core::Key key) const
{
    return glfwGetKey(m_window, translate_key_code_to_glfw(key)) == GLFW_RELEASE;
}

bool WindowManager::is_mouse_button_pressed(const core::Key button) const
{
    return glfwGetKey(m_window, translate_mouse_button_to_glfw(button)) == GLFW_PRESS;
}

bool WindowManager::is_mouse_button_released(const core::Key button) const
{
    return glfwGetKey(m_window, translate_mouse_button_to_glfw(button)) == GLFW_RELEASE;
}

void WindowManager::set_key_callback(const KeyCallback& cb)
{
    m_key_callback = cb;
}

void WindowManager::set_cursor_pos_callback(const MouseMoveCallback& cb)
{
    m_mouse_move_callback = cb;
}

void WindowManager::set_mouse_button_callback(const MouseButtonCallback& cb)
{
    m_mouse_button_callback = cb;
}

void WindowManager::set_scroll_callback(const ScrollCallback& cb)
{
    m_scroll_callback = cb;
}

void WindowManager::setup_glfw_callback() const
{
    glfwSetKeyCallback(m_window, glfw_key_callback);
    glfwSetCursorPosCallback(m_window, glfw_cursor_pos_callback);
    glfwSetMouseButtonCallback(m_window, glfw_mouse_button_callback);
    glfwSetScrollCallback(m_window, glfw_scroll_callback);
}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void WindowManager::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* instance = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (instance && instance->m_key_callback) {
        instance->m_key_callback(translate_key_code_to_internal(key), translate_key_action_to_internal(action));
    }
}

void WindowManager::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto* instance = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (instance && instance->m_mouse_button_callback) {
        instance->m_mouse_button_callback(
            translate_key_code_to_internal(button),
            translate_key_action_to_internal(action));
    }
}

void WindowManager::glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto* instance = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (instance && instance->m_mouse_move_callback) {
        instance->m_mouse_move_callback(xpos, ypos);
    }
}

void WindowManager::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* instance = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (instance && instance->m_scroll_callback) {
        instance->m_scroll_callback(xoffset, yoffset);
    }
}

core::KeyAction WindowManager::translate_key_action_to_internal(const int glfw_action)
{
    switch (glfw_action) {
        case GLFW_PRESS:
            return core::KeyAction::press;
        case GLFW_RELEASE:
            return core::KeyAction::release;
        default:
            return core::KeyAction::undefined_key_action;
    };
}

core::Key WindowManager::translate_mouse_button_to_internal(const int glfw_mouse_button)
{
    switch (glfw_mouse_button) {
        case GLFW_MOUSE_BUTTON_1:
            return core::Key::mouse_button_1;
        case GLFW_MOUSE_BUTTON_2:
            return core::Key::mouse_button_2;
        case GLFW_MOUSE_BUTTON_3:
            return core::Key::mouse_button_3;
        case GLFW_MOUSE_BUTTON_4:
            return core::Key::mouse_button_4;
        case GLFW_MOUSE_BUTTON_5:
            return core::Key::mouse_button_5;
        case GLFW_MOUSE_BUTTON_6:
            return core::Key::mouse_button_6;
        case GLFW_MOUSE_BUTTON_7:
            return core::Key::mouse_button_7;
        case GLFW_MOUSE_BUTTON_8:
            return core::Key::mouse_button_8;
        default:
            return core::Key::undefined_key;
    };
}

core::Key WindowManager::translate_key_code_to_internal(const int glfw_key)
{
    switch (glfw_key) {
        case GLFW_KEY_0:
            return core::Key::key_0;
        case GLFW_KEY_1:
            return core::Key::key_1;
        case GLFW_KEY_2:
            return core::Key::key_2;
        case GLFW_KEY_3:
            return core::Key::key_3;
        case GLFW_KEY_4:
            return core::Key::key_4;
        case GLFW_KEY_5:
            return core::Key::key_5;
        case GLFW_KEY_6:
            return core::Key::key_6;
        case GLFW_KEY_7:
            return core::Key::key_7;
        case GLFW_KEY_8:
            return core::Key::key_8;
        case GLFW_KEY_9:
            return core::Key::key_9;

        case GLFW_KEY_A:
            return core::Key::key_a;
        case GLFW_KEY_B:
            return core::Key::key_b;
        case GLFW_KEY_C:
            return core::Key::key_c;
        case GLFW_KEY_D:
            return core::Key::key_d;
        case GLFW_KEY_E:
            return core::Key::key_e;
        case GLFW_KEY_F:
            return core::Key::key_f;
        case GLFW_KEY_G:
            return core::Key::key_g;
        case GLFW_KEY_H:
            return core::Key::key_h;
        case GLFW_KEY_I:
            return core::Key::key_i;
        case GLFW_KEY_J:
            return core::Key::key_j;
        case GLFW_KEY_K:
            return core::Key::key_k;
        case GLFW_KEY_L:
            return core::Key::key_l;
        case GLFW_KEY_M:
            return core::Key::key_m;
        case GLFW_KEY_N:
            return core::Key::key_n;
        case GLFW_KEY_O:
            return core::Key::key_o;
        case GLFW_KEY_P:
            return core::Key::key_p;
        case GLFW_KEY_Q:
            return core::Key::key_q;
        case GLFW_KEY_R:
            return core::Key::key_r;
        case GLFW_KEY_S:
            return core::Key::key_s;
        case GLFW_KEY_T:
            return core::Key::key_t;
        case GLFW_KEY_U:
            return core::Key::key_u;
        case GLFW_KEY_V:
            return core::Key::key_v;
        case GLFW_KEY_W:
            return core::Key::key_w;
        case GLFW_KEY_X:
            return core::Key::key_x;
        case GLFW_KEY_Y:
            return core::Key::key_y;
        case GLFW_KEY_Z:
            return core::Key::key_z;

        case GLFW_KEY_SPACE:
            return core::Key::key_space;
        case GLFW_KEY_ESCAPE:
            return core::Key::key_escape;
        case GLFW_KEY_ENTER:
            return core::Key::key_enter;
        case GLFW_KEY_TAB:
            return core::Key::key_tab;
        case GLFW_KEY_BACKSPACE:
            return core::Key::key_backspace;
        case GLFW_KEY_INSERT:
            return core::Key::key_insert;
        case GLFW_KEY_DELETE:
            return core::Key::key_delete;
        case GLFW_KEY_RIGHT:
            return core::Key::key_right;
        case GLFW_KEY_LEFT:
            return core::Key::key_left;
        case GLFW_KEY_DOWN:
            return core::Key::key_down;
        case GLFW_KEY_UP:
            return core::Key::key_up;
        case GLFW_KEY_PAGE_UP:
            return core::Key::key_page_up;
        case GLFW_KEY_PAGE_DOWN:
            return core::Key::key_page_down;
        case GLFW_KEY_HOME:
            return core::Key::key_home;
        case GLFW_KEY_END:
            return core::Key::key_end;
        case GLFW_KEY_CAPS_LOCK:
            return core::Key::key_caps_lock;
        case GLFW_KEY_SCROLL_LOCK:
            return core::Key::key_scroll_lock;
        case GLFW_KEY_NUM_LOCK:
            return core::Key::key_num_lock;
        case GLFW_KEY_PRINT_SCREEN:
            return core::Key::key_print_screen;
        case GLFW_KEY_PAUSE:
            return core::Key::key_pause;
        case GLFW_KEY_F1:
            return core::Key::key_f1;
        case GLFW_KEY_F2:
            return core::Key::key_f2;
        case GLFW_KEY_F3:
            return core::Key::key_f3;
        case GLFW_KEY_F4:
            return core::Key::key_f4;
        case GLFW_KEY_F5:
            return core::Key::key_f5;
        case GLFW_KEY_F6:
            return core::Key::key_f6;
        case GLFW_KEY_F7:
            return core::Key::key_f7;
        case GLFW_KEY_F8:
            return core::Key::key_f8;
        case GLFW_KEY_F9:
            return core::Key::key_f9;
        case GLFW_KEY_F10:
            return core::Key::key_f10;
        case GLFW_KEY_F11:
            return core::Key::key_f11;
        case GLFW_KEY_F12:
            return core::Key::key_f12;

        case GLFW_KEY_LEFT_CONTROL:
            return core::Key::key_left_ctrl;

        default:
            return core::Key::undefined_key;
    };
}

int WindowManager::translate_key_action_to_glfw(const core::KeyAction action)
{
    switch (action) {
        case core::KeyAction::press:
            return GLFW_PRESS;
        case core::KeyAction::release:
            return GLFW_RELEASE;
        default: CGX_ERROR("Invalid GLFW Action")
            return GLFW_RELEASE;
    };
}

int WindowManager::translate_mouse_button_to_glfw(const core::Key key)
{
    switch (key) {
        case core::Key::mouse_button_1:
            return GLFW_MOUSE_BUTTON_1;
        case core::Key::mouse_button_2:
            return GLFW_MOUSE_BUTTON_2;
        case core::Key::mouse_button_3:
            return GLFW_MOUSE_BUTTON_3;
        case core::Key::mouse_button_4:
            return GLFW_MOUSE_BUTTON_4;
        case core::Key::mouse_button_5:
            return GLFW_MOUSE_BUTTON_5;
        case core::Key::mouse_button_6:
            return GLFW_MOUSE_BUTTON_6;
        case core::Key::mouse_button_7:
            return GLFW_MOUSE_BUTTON_7;
        case core::Key::mouse_button_8:
            return GLFW_MOUSE_BUTTON_8;
        default: CGX_ERROR("Invalid GLFW key");
            return GLFW_KEY_0; //placeholder
    };
}

int WindowManager::translate_key_code_to_glfw(const core::Key key)
{
    switch (key) {
        case core::Key::key_0:
            return GLFW_KEY_0;
        case core::Key::key_1:
            return GLFW_KEY_1;
        case core::Key::key_2:
            return GLFW_KEY_2;
        case core::Key::key_3:
            return GLFW_KEY_3;
        case core::Key::key_4:
            return GLFW_KEY_4;
        case core::Key::key_5:
            return GLFW_KEY_5;
        case core::Key::key_6:
            return GLFW_KEY_6;
        case core::Key::key_7:
            return GLFW_KEY_7;
        case core::Key::key_8:
            return GLFW_KEY_8;
        case core::Key::key_9:
            return GLFW_KEY_9;

        case core::Key::key_a:
            return GLFW_KEY_A;
        case core::Key::key_b:
            return GLFW_KEY_B;
        case core::Key::key_c:
            return GLFW_KEY_C;
        case core::Key::key_d:
            return GLFW_KEY_D;
        case core::Key::key_e:
            return GLFW_KEY_E;
        case core::Key::key_f:
            return GLFW_KEY_F;
        case core::Key::key_g:
            return GLFW_KEY_G;
        case core::Key::key_h:
            return GLFW_KEY_H;
        case core::Key::key_i:
            return GLFW_KEY_I;
        case core::Key::key_j:
            return GLFW_KEY_J;
        case core::Key::key_k:
            return GLFW_KEY_K;
        case core::Key::key_l:
            return GLFW_KEY_L;
        case core::Key::key_m:
            return GLFW_KEY_M;
        case core::Key::key_n:
            return GLFW_KEY_N;
        case core::Key::key_o:
            return GLFW_KEY_O;
        case core::Key::key_p:
            return GLFW_KEY_P;
        case core::Key::key_q:
            return GLFW_KEY_Q;
        case core::Key::key_r:
            return GLFW_KEY_R;
        case core::Key::key_s:
            return GLFW_KEY_S;
        case core::Key::key_t:
            return GLFW_KEY_T;
        case core::Key::key_u:
            return GLFW_KEY_U;
        case core::Key::key_v:
            return GLFW_KEY_V;
        case core::Key::key_w:
            return GLFW_KEY_W;
        case core::Key::key_x:
            return GLFW_KEY_X;
        case core::Key::key_y:
            return GLFW_KEY_Y;
        case core::Key::key_z:
            return GLFW_KEY_Z;

        case core::Key::key_space:
            return GLFW_KEY_SPACE;
        case core::Key::key_escape:
            return GLFW_KEY_ESCAPE;
        case core::Key::key_enter:
            return GLFW_KEY_ENTER;
        case core::Key::key_tab:
            return GLFW_KEY_TAB;
        case core::Key::key_backspace:
            return GLFW_KEY_BACKSPACE;
        case core::Key::key_insert:
            return GLFW_KEY_INSERT;
        case core::Key::key_delete:
            return GLFW_KEY_DELETE;
        case core::Key::key_right:
            return GLFW_KEY_RIGHT;
        case core::Key::key_left:
            return GLFW_KEY_LEFT;
        case core::Key::key_down:
            return GLFW_KEY_DOWN;
        case core::Key::key_up:
            return GLFW_KEY_UP;
        case core::Key::key_page_up:
            return GLFW_KEY_PAGE_UP;
        case core::Key::key_page_down:
            return GLFW_KEY_PAGE_DOWN;
        case core::Key::key_home:
            return GLFW_KEY_HOME;
        case core::Key::key_end:
            return GLFW_KEY_END;
        case core::Key::key_caps_lock:
            return GLFW_KEY_CAPS_LOCK;
        case core::Key::key_num_lock:
            return GLFW_KEY_NUM_LOCK;
        case core::Key::key_print_screen:
            return GLFW_KEY_PRINT_SCREEN;
        case core::Key::key_pause:
            return GLFW_KEY_PAUSE;
        case core::Key::key_f1:
            return GLFW_KEY_F1;
        case core::Key::key_f2:
            return GLFW_KEY_F2;
        case core::Key::key_f3:
            return GLFW_KEY_F3;
        case core::Key::key_f4:
            return GLFW_KEY_F4;
        case core::Key::key_f5:
            return GLFW_KEY_F5;
        case core::Key::key_f6:
            return GLFW_KEY_F6;
        case core::Key::key_f7:
            return GLFW_KEY_F7;
        case core::Key::key_f8:
            return GLFW_KEY_F8;
        case core::Key::key_f9:
            return GLFW_KEY_F9;
        case core::Key::key_f10:
            return GLFW_KEY_F10;
        case core::Key::key_f11:
            return GLFW_KEY_F11;
        case core::Key::key_f12:
            return GLFW_KEY_F12;
        case core::Key::key_left_ctrl:
            return GLFW_KEY_LEFT_CONTROL;

        default: CGX_ERROR("Invalid GLFW key")
            return GLFW_KEY_0; //placeholder
    };
}
}
