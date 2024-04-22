// Copyright © 2024 Jacob Curlin

#include "gui/panels/input_panel.h"

namespace cgx::gui
{
InputPanel::InputPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Input", context, manager) {}

InputPanel::~InputPanel() = default;

void InputPanel::render()
{
    ImGui::Text("Toggle Camera Control On: M");
    ImGui::Text("Toggle Camera Control Off: G");
    ImGui::Text("Movement: WASD");
    ImGui::Text("Sprint: Left Shift");
    ImGui::Text("Movement Up: Space");
    ImGui::Text("Movement Down: Left Ctrl");
}
}
