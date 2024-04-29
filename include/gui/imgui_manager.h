// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "gui/gui_context.h"
#include "imgui.h"

#include <vector>
#include <memory>
#include <__filesystem/path.h>

namespace cgx::gui
{
class ImGuiManager
{
public:
    explicit ImGuiManager(GUIContext* context);
    ~ImGuiManager();

    void shutdown();

    void init();
    void register_event_handlers();

    void register_panel(std::unique_ptr<ImGuiPanel> panel);
    ImGuiPanel* get_panel(const std::string& label);

    void render();
    void begin_render();
    void end_render();

    void draw_editor();
    void draw_fullscreen_render();
    void draw_main_menu_bar();

    void load_fonts();
    void set_style();

    void toggle_interface();
    void enable_imgui_input();
    void disable_imgui_input();

    ImFont* m_title_font{};
    ImFont* m_header_font{};
    ImFont* m_body_font{};
    ImFont* m_small_font{};

private:
    bool m_interface_enabled{true};

    GUIContext*                              m_context{nullptr};
    std::vector<std::unique_ptr<ImGuiPanel>> m_imgui_panels;

    char m_input_buffer[256]{};
    void clear_inputs(ImGuiIO& io);
};
}
