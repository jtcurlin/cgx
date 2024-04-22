// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"

namespace cgx::gui
{
class RenderSettingsPanel final : public ImGuiPanel
{
public:
    RenderSettingsPanel(GUIContext* context, ImGuiManager* manager);
    ~RenderSettingsPanel() override;

    void draw_skybox_menu() const;

    void render() override;
};
}
