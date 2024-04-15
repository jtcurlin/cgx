// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"

namespace cgx::gui
{
class RenderSettingsPanel final : public ImGuiPanel
{
public:
    explicit RenderSettingsPanel (const std::shared_ptr<GUIContext>& context);
    ~RenderSettingsPanel () override;

    void draw_skybox_menu();

    void render () override;
};
}
