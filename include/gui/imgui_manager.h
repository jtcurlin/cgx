// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "gui/gui_context.h"
#include "imgui/imgui.h"

namespace cgx::gui
{
class ImGuiManager : public std::enable_shared_from_this<ImGuiManager>
{
public:
    explicit ImGuiManager(std::shared_ptr<GUIContext> context);
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
    std::shared_ptr<GUIContext>              m_context;
    std::vector<std::unique_ptr<ImGuiPanel>> m_imgui_panels;



    void clear_inputs(ImGuiIO& io);
};
}
