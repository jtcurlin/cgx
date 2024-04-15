// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/gui_context.h"
#include <imgui.h>
#include <string>

namespace cgx::gui
{
class ImGuiPanel
{
public:
    explicit ImGuiPanel (const std::string& title, const std::shared_ptr<GUIContext>& context);
    virtual  ~ImGuiPanel () = default;

    void Begin (); // ImGui::Begin() called (start actual ImGui window)
    void End ();   // ImGui::End() called (end actual ImGui window)

    virtual void render () = 0; // child-defined, actual ImGui window UI logic
    virtual void on_begin ();   // child-defined operations before Begin() calls ImGui::Begin()
    virtual void on_end ();     // child-defined operations before End() calls ImGui::End()

    void show ();
    void hide ();

    [[nodiscard]] const std::string& get_title ();
    [[nodiscard]] bool               is_visible ();

    void set_min_size (float width, float height);
    void set_max_size (float width, float height);

protected:
    std::shared_ptr<GUIContext> m_context;

    std::string m_title{"Window"};
    bool        m_is_visible{true};
    bool        m_is_hovered{false};
    bool        m_enforce_aspect_ratio{false};

    ImGuiWindowFlags m_window_flags;

    float m_aspect_ratio{1};
    float m_min_size[2]{240.0f, 180.0f};   // minimum window size
    float m_max_size[2]{1920.0f, 1080.0f}; // maximum window size
};
}
