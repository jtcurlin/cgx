// Copyright © 2024 Jacob Curlin

#include "core/window_manager.h"

#include "ecs/common.h"
#include "core/event_handler.h"
#include "core/events/master_events.h"

#include <glad/glad.h>
#include <iostream>

namespace cgx::core
{
WindowManager::WindowManager(const uint32_t width, const uint32_t height, std::string label)
    : m_window(nullptr)
    , m_config{height, width, std::move(label)}
{
    init();

    auto& event_handler = EventHandler::get_instance();
    event_handler.add_listener(
        event::master::ACTIVATE_GUI_CONTROL_MODE,
        [this](event::Event& event) {
            this->enable_cursor();
        });

    event_handler.add_listener(
        event::master::ACTIVATE_GAME_CONTROL_MODE,
        [this](event::Event& event) {
            this->disable_cursor();
        });
}

WindowManager::~WindowManager()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowManager::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(
        static_cast<int>(m_config.width),
        static_cast<int>(m_config.height),
        m_config.label.c_str(),
        nullptr,
        nullptr);
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

    CGX_INFO("WindowManager initialized [window '{}' : {} x {}]", m_config.label, m_config.width, m_config.height);
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

bool WindowManager::is_key_pressed(const Key key) const
{
    return glfwGetKey(m_window, translate_key_code_to_glfw(key)) == GLFW_PRESS;
}

bool WindowManager::is_key_released(const Key key) const
{
    return glfwGetKey(m_window, translate_key_code_to_glfw(key)) == GLFW_RELEASE;
}

bool WindowManager::is_mouse_button_pressed(const Key button) const
{
    return glfwGetKey(m_window, translate_mouse_button_to_glfw(button)) == GLFW_PRESS;
}

bool WindowManager::is_mouse_button_released(const Key button) const
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

KeyAction WindowManager::translate_key_action_to_internal(const int glfw_action)
{
    switch (glfw_action) {
        case GLFW_PRESS:
            return KeyAction::press;
        case GLFW_RELEASE:
            return KeyAction::release;
        default:
            return KeyAction::undefined_key_action;
    };
}

Key WindowManager::translate_mouse_button_to_internal(const int glfw_mouse_button)
{
    switch (glfw_mouse_button) {
        case GLFW_MOUSE_BUTTON_1:
            return Key::mouse_button_1;
        case GLFW_MOUSE_BUTTON_2:
            return Key::mouse_button_2;
        case GLFW_MOUSE_BUTTON_3:
            return Key::mouse_button_3;
        case GLFW_MOUSE_BUTTON_4:
            return Key::mouse_button_4;
        case GLFW_MOUSE_BUTTON_5:
            return Key::mouse_button_5;
        case GLFW_MOUSE_BUTTON_6:
            return Key::mouse_button_6;
        case GLFW_MOUSE_BUTTON_7:
            return Key::mouse_button_7;
        case GLFW_MOUSE_BUTTON_8:
            return Key::mouse_button_8;
        default:
            return Key::undefined_key;
    };
}

Key WindowManager::translate_key_code_to_internal(const int glfw_key)
{
    switch (glfw_key) {
        case GLFW_KEY_0:
            return Key::key_0;
        case GLFW_KEY_1:
            return Key::key_1;
        case GLFW_KEY_2:
            return Key::key_2;
        case GLFW_KEY_3:
            return Key::key_3;
        case GLFW_KEY_4:
            return Key::key_4;
        case GLFW_KEY_5:
            return Key::key_5;
        case GLFW_KEY_6:
            return Key::key_6;
        case GLFW_KEY_7:
            return Key::key_7;
        case GLFW_KEY_8:
            return Key::key_8;
        case GLFW_KEY_9:
            return Key::key_9;

        case GLFW_KEY_A:
            return Key::key_a;
        case GLFW_KEY_B:
            return Key::key_b;
        case GLFW_KEY_C:
            return Key::key_c;
        case GLFW_KEY_D:
            return Key::key_d;
        case GLFW_KEY_E:
            return Key::key_e;
        case GLFW_KEY_F:
            return Key::key_f;
        case GLFW_KEY_G:
            return Key::key_g;
        case GLFW_KEY_H:
            return Key::key_h;
        case GLFW_KEY_I:
            return Key::key_i;
        case GLFW_KEY_J:
            return Key::key_j;
        case GLFW_KEY_K:
            return Key::key_k;
        case GLFW_KEY_L:
            return Key::key_l;
        case GLFW_KEY_M:
            return Key::key_m;
        case GLFW_KEY_N:
            return Key::key_n;
        case GLFW_KEY_O:
            return Key::key_o;
        case GLFW_KEY_P:
            return Key::key_p;
        case GLFW_KEY_Q:
            return Key::key_q;
        case GLFW_KEY_R:
            return Key::key_r;
        case GLFW_KEY_S:
            return Key::key_s;
        case GLFW_KEY_T:
            return Key::key_t;
        case GLFW_KEY_U:
            return Key::key_u;
        case GLFW_KEY_V:
            return Key::key_v;
        case GLFW_KEY_W:
            return Key::key_w;
        case GLFW_KEY_X:
            return Key::key_x;
        case GLFW_KEY_Y:
            return Key::key_y;
        case GLFW_KEY_Z:
            return Key::key_z;

        case GLFW_KEY_COMMA:
            return Key::key_comma;
        case GLFW_KEY_PERIOD:
            return Key::key_period;
        case GLFW_KEY_SLASH:
            return Key::key_fslash;
        case GLFW_KEY_APOSTROPHE:
            return Key::key_apostrophe;

        case GLFW_KEY_SPACE:
            return Key::key_space;
        case GLFW_KEY_ESCAPE:
            return Key::key_escape;
        case GLFW_KEY_ENTER:
            return Key::key_enter;
        case GLFW_KEY_TAB:
            return Key::key_tab;
        case GLFW_KEY_BACKSPACE:
            return Key::key_backspace;
        case GLFW_KEY_INSERT:
            return Key::key_insert;
        case GLFW_KEY_DELETE:
            return Key::key_delete;
        case GLFW_KEY_RIGHT:
            return Key::key_right;
        case GLFW_KEY_LEFT:
            return Key::key_left;
        case GLFW_KEY_DOWN:
            return Key::key_down;
        case GLFW_KEY_UP:
            return Key::key_up;
        case GLFW_KEY_PAGE_UP:
            return Key::key_page_up;
        case GLFW_KEY_PAGE_DOWN:
            return Key::key_page_down;
        case GLFW_KEY_HOME:
            return Key::key_home;
        case GLFW_KEY_END:
            return Key::key_end;
        case GLFW_KEY_CAPS_LOCK:
            return Key::key_caps_lock;
        case GLFW_KEY_SCROLL_LOCK:
            return Key::key_scroll_lock;
        case GLFW_KEY_NUM_LOCK:
            return Key::key_num_lock;
        case GLFW_KEY_PRINT_SCREEN:
            return Key::key_print_screen;
        case GLFW_KEY_PAUSE:
            return Key::key_pause;
        case GLFW_KEY_F1:
            return Key::key_f1;
        case GLFW_KEY_F2:
            return Key::key_f2;
        case GLFW_KEY_F3:
            return Key::key_f3;
        case GLFW_KEY_F4:
            return Key::key_f4;
        case GLFW_KEY_F5:
            return Key::key_f5;
        case GLFW_KEY_F6:
            return Key::key_f6;
        case GLFW_KEY_F7:
            return Key::key_f7;
        case GLFW_KEY_F8:
            return Key::key_f8;
        case GLFW_KEY_F9:
            return Key::key_f9;
        case GLFW_KEY_F10:
            return Key::key_f10;
        case GLFW_KEY_F11:
            return Key::key_f11;
        case GLFW_KEY_F12:
            return Key::key_f12;

        case GLFW_KEY_LEFT_CONTROL:
            return Key::key_left_ctrl;

        default:
            return Key::undefined_key;
    };
}

int WindowManager::translate_key_action_to_glfw(const KeyAction action)
{
    switch (action) {
        case KeyAction::press:
            return GLFW_PRESS;
        case KeyAction::release:
            return GLFW_RELEASE;
        default: CGX_ERROR("Invalid GLFW Action")
            return GLFW_RELEASE;
    };
}

int WindowManager::translate_mouse_button_to_glfw(const Key key)
{
    switch (key) {
        case Key::mouse_button_1:
            return GLFW_MOUSE_BUTTON_1;
        case Key::mouse_button_2:
            return GLFW_MOUSE_BUTTON_2;
        case Key::mouse_button_3:
            return GLFW_MOUSE_BUTTON_3;
        case Key::mouse_button_4:
            return GLFW_MOUSE_BUTTON_4;
        case Key::mouse_button_5:
            return GLFW_MOUSE_BUTTON_5;
        case Key::mouse_button_6:
            return GLFW_MOUSE_BUTTON_6;
        case Key::mouse_button_7:
            return GLFW_MOUSE_BUTTON_7;
        case Key::mouse_button_8:
            return GLFW_MOUSE_BUTTON_8;
        default: CGX_ERROR("Invalid GLFW key");
            return GLFW_KEY_0; //placeholder
    };
}

int WindowManager::translate_key_code_to_glfw(const Key key)
{
    switch (key) {
        case Key::key_0:
            return GLFW_KEY_0;
        case Key::key_1:
            return GLFW_KEY_1;
        case Key::key_2:
            return GLFW_KEY_2;
        case Key::key_3:
            return GLFW_KEY_3;
        case Key::key_4:
            return GLFW_KEY_4;
        case Key::key_5:
            return GLFW_KEY_5;
        case Key::key_6:
            return GLFW_KEY_6;
        case Key::key_7:
            return GLFW_KEY_7;
        case Key::key_8:
            return GLFW_KEY_8;
        case Key::key_9:
            return GLFW_KEY_9;

        case Key::key_a:
            return GLFW_KEY_A;
        case Key::key_b:
            return GLFW_KEY_B;
        case Key::key_c:
            return GLFW_KEY_C;
        case Key::key_d:
            return GLFW_KEY_D;
        case Key::key_e:
            return GLFW_KEY_E;
        case Key::key_f:
            return GLFW_KEY_F;
        case Key::key_g:
            return GLFW_KEY_G;
        case Key::key_h:
            return GLFW_KEY_H;
        case Key::key_i:
            return GLFW_KEY_I;
        case Key::key_j:
            return GLFW_KEY_J;
        case Key::key_k:
            return GLFW_KEY_K;
        case Key::key_l:
            return GLFW_KEY_L;
        case Key::key_m:
            return GLFW_KEY_M;
        case Key::key_n:
            return GLFW_KEY_N;
        case Key::key_o:
            return GLFW_KEY_O;
        case Key::key_p:
            return GLFW_KEY_P;
        case Key::key_q:
            return GLFW_KEY_Q;
        case Key::key_r:
            return GLFW_KEY_R;
        case Key::key_s:
            return GLFW_KEY_S;
        case Key::key_t:
            return GLFW_KEY_T;
        case Key::key_u:
            return GLFW_KEY_U;
        case Key::key_v:
            return GLFW_KEY_V;
        case Key::key_w:
            return GLFW_KEY_W;
        case Key::key_x:
            return GLFW_KEY_X;
        case Key::key_y:
            return GLFW_KEY_Y;
        case Key::key_z:
            return GLFW_KEY_Z;

        case Key::key_comma:
            return GLFW_KEY_COMMA;
        case Key::key_period:
            return GLFW_KEY_PERIOD;
        case Key::key_fslash:
            return GLFW_KEY_SLASH;
        case Key::key_apostrophe:
            return GLFW_KEY_APOSTROPHE;

        case Key::key_space:
            return GLFW_KEY_SPACE;
        case Key::key_escape:
            return GLFW_KEY_ESCAPE;
        case Key::key_enter:
            return GLFW_KEY_ENTER;
        case Key::key_tab:
            return GLFW_KEY_TAB;
        case Key::key_backspace:
            return GLFW_KEY_BACKSPACE;
        case Key::key_insert:
            return GLFW_KEY_INSERT;
        case Key::key_delete:
            return GLFW_KEY_DELETE;
        case Key::key_right:
            return GLFW_KEY_RIGHT;
        case Key::key_left:
            return GLFW_KEY_LEFT;
        case Key::key_down:
            return GLFW_KEY_DOWN;
        case Key::key_up:
            return GLFW_KEY_UP;
        case Key::key_page_up:
            return GLFW_KEY_PAGE_UP;
        case Key::key_page_down:
            return GLFW_KEY_PAGE_DOWN;
        case Key::key_home:
            return GLFW_KEY_HOME;
        case Key::key_end:
            return GLFW_KEY_END;
        case Key::key_caps_lock:
            return GLFW_KEY_CAPS_LOCK;
        case Key::key_num_lock:
            return GLFW_KEY_NUM_LOCK;
        case Key::key_print_screen:
            return GLFW_KEY_PRINT_SCREEN;
        case Key::key_pause:
            return GLFW_KEY_PAUSE;
        case Key::key_f1:
            return GLFW_KEY_F1;
        case Key::key_f2:
            return GLFW_KEY_F2;
        case Key::key_f3:
            return GLFW_KEY_F3;
        case Key::key_f4:
            return GLFW_KEY_F4;
        case Key::key_f5:
            return GLFW_KEY_F5;
        case Key::key_f6:
            return GLFW_KEY_F6;
        case Key::key_f7:
            return GLFW_KEY_F7;
        case Key::key_f8:
            return GLFW_KEY_F8;
        case Key::key_f9:
            return GLFW_KEY_F9;
        case Key::key_f10:
            return GLFW_KEY_F10;
        case Key::key_f11:
            return GLFW_KEY_F11;
        case Key::key_f12:
            return GLFW_KEY_F12;
        case Key::key_left_ctrl:
            return GLFW_KEY_LEFT_CONTROL;

        default: CGX_ERROR("Invalid GLFW key")
            return GLFW_KEY_0; //placeholder
    };
}
}
