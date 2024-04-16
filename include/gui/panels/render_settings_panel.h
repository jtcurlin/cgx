// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"

namespace cgx::gui
{
class RenderSettingsPanel final : public ImGuiPanel
{
public:
    explicit RenderSettingsPanel(const std::shared_ptr<GUIContext>& context, const std::shared_ptr<ImGuiManager>& manager);
    ~RenderSettingsPanel() override;

    void draw_skybox_menu() const;

    void render() override;
};
}
