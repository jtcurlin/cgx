// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "glad/glad.h"

namespace cgx::scene {
class Node;
}

namespace cgx::gui
{
class ViewportPanel final : public ImGuiPanel
{
public:
    ViewportPanel(GUIContext* context, ImGuiManager* manager);
    ~ViewportPanel() override;

    void set_texture(uint32_t width, uint32_t height, uint32_t texture_id);

    void render() override;
    void on_begin() override;
    void on_end() override;

    void set_camera(std::shared_ptr<scene::Node> node);

protected:
    std::shared_ptr<scene::Node> m_active_camera_node{nullptr};

    float  m_tex_width{0.0f};
    float  m_tex_height{0.0f};
    GLuint m_texture_id{0};
};
}
