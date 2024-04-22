// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "gui/gui_context.h"
#include "imgui.h"

#include <vector>

namespace cgx::gui
{
class ImGuiManager
{
public:
    explicit ImGuiManager(GUIContext* context);
    ~ImGuiManager();

    void initialize();
    void shutdown();

    void register_panel(std::unique_ptr<ImGuiPanel> panel);

    void render();
    void begin_render();
    void end_render();

    void render_core_menu() const;

    void load_fonts();
    void set_style();

    void enable_input();
    void disable_input();

    ImFont* m_title_font{};
    ImFont* m_header_font{};
    ImFont* m_body_font{};
    ImFont* m_small_font{};

private:
    GUIContext* m_context{nullptr};
    std::vector<std::unique_ptr<ImGuiPanel>> m_imgui_panels;

    char m_input_buffer[256]{};

    void clear_inputs(ImGuiIO& io);
};
}
