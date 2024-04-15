// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "gui/imgui_panel.h"
#include "gui/gui_context.h"

#include <imgui/imgui.h>

namespace cgx::gui
{
class ImGuiManager
{
public:
    explicit ImGuiManager (std::shared_ptr<GUIContext> context);
    ~ImGuiManager ();

    void initialize ();
    void shutdown ();

    void register_panel (std::unique_ptr<ImGuiPanel> panel);

    void render ();
    void begin_render ();
    void end_render ();

    void render_core_menu ();

    void load_fonts();

    void enable_input ();
    void disable_input ();

    void set_style (const char* font_path);

private:
    std::shared_ptr<GUIContext>              m_context;
    std::vector<std::unique_ptr<ImGuiPanel>> m_imgui_panels;

    void clear_inputs (ImGuiIO& io);
};
}
