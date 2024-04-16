// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "glad/glad.h"

namespace cgx::gui
{
class ViewportPanel final : public ImGuiPanel
{
public:
    explicit ViewportPanel(const std::shared_ptr<GUIContext>& context, const std::shared_ptr<ImGuiManager>& manager);
    ~ViewportPanel() override;

    void set_texture(uint32_t width, uint32_t height, uint32_t texture_id);

    void render() override;
    void on_begin() override;
    void on_end() override;

protected:
    std::shared_ptr<GUIContext> m_context;

    float  m_tex_width{0.0f};
    float  m_tex_height{0.0f};
    GLuint m_texture_id{0};
};
}