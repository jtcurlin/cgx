// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"


namespace cgx::gui
{
    class InputPanel : public ImGuiPanel
    {
        InputPanel(const std::shared_ptr<GUIContext>& context, const std::shared_ptr<ImGuiManager>& manager);
        ~InputPanel() override;

        void render() override;
    };
}