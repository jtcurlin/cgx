// Copyright © 2024 Jacob Curlin

#pragma once
#include "gui/gui_context.h"

#include <imgui.h>
#include <string>

namespace cgx::gui
{
class ImGuiManager;

class ImGuiPanel
{
public:
    ImGuiPanel(std::string title, GUIContext* context, ImGuiManager* manager);
    virtual  ~ImGuiPanel() = default;

    void Begin(); // ImGui::Begin() called (start actual ImGui window)
    void End();   // ImGui::End() called (end actual ImGui window)

    virtual void render() = 0; // child-defined, actual ImGui window UI logic
    virtual void on_begin();   // child-defined operations before Begin() calls ImGui::Begin()
    virtual void on_end();     // child-defined operations before End() calls ImGui::End()

    void show();
    void hide();

    [[nodiscard]] const std::string& get_title();
    [[nodiscard]] bool               is_visible() const;

    void set_min_size(float width, float height);
    void set_max_size(float width, float height);

protected:
    std::string   m_title{"Window"};
    GUIContext*   m_context{nullptr};
    ImGuiManager* m_manager{nullptr};

    bool m_is_visible{true};
    bool m_is_hovered{false};
    bool m_enforce_aspect_ratio{false};

    ImGuiWindowFlags m_window_flags;

    float m_aspect_ratio{1};
    float m_min_size[2]{240.0f, 180.0f};   // minimum window size
    float m_max_size[2]{1920.0f, 1080.0f}; // maximum window size
};
}
