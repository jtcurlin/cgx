// Copyright © 2024 Jacob Curlin

#include "gui/panels/viewport_panel.h"
#include "scene/scene_manager.h"
#include "scene/scene.h"
#include "render/render_system.h"
#include "utility/error.h"

namespace cgx::gui
{
ViewportPanel::ViewportPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Viewport", context, manager, ImGuiWindowFlags_MenuBar)
{
    m_enforce_aspect_ratio = true;
    m_window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    const auto framebuffer = m_context->get_render_system()->getFramebuffer();
    uint32_t   width, height;
    framebuffer->getSize(width, height);
    CGX_ASSERT(glIsTexture(framebuffer->getTextureID()), "Invalid Texture ID in Render Framebuffer");
    set_texture(width, height, framebuffer->getTextureID());
}

ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::set_texture(const uint32_t width, const uint32_t height, const uint32_t texture_id)
{
    m_tex_width    = static_cast<float>(width);
    m_tex_height   = static_cast<float>(height);
    m_aspect_ratio = m_tex_height != 0 ? m_tex_width / m_tex_height : 1.0f;

    m_texture_id = texture_id;
}

void ViewportPanel::on_begin()
{
    ImGuiPanel::on_begin();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{2.0f, 2.0f});
}

void ViewportPanel::on_end()
{
    ImGui::PopStyleVar(1);
}

void ViewportPanel::set_camera(std::shared_ptr<scene::CameraNode> camera_node)
{
    m_active_camera_node = std::move(camera_node);
    m_context->get_render_system()->set_camera(m_active_camera_node->get_entity());
}

void ViewportPanel::render()
{
    if (ImGui::BeginMenuBar()) {
        const std::string active_node_tag = m_active_camera_node ? m_active_camera_node->get_tag() : "[No Camera]";
        const std::string label = "Camera";
        if (ImGui::BeginMenu(label.c_str())) {
            auto root_node = m_context->get_scene_manager()->get_active_scene()->get_root();
            for (const auto& child : root_node->get_children()) {
                auto child_node = std::dynamic_pointer_cast<scene::Node>(child);
                if (child_node->get_node_type() == scene::NodeType::Type::Camera) {
                    auto child_camera_node = std::dynamic_pointer_cast<scene::CameraNode>(child_node);
                    if (ImGui::MenuItem(child_camera_node->get_tag().c_str(), "", m_active_camera_node.get() == child_camera_node.get())) {
                        set_camera(child_camera_node);
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    const ImVec2 window_size = ImGui::GetContentRegionAvail();

    const float desired_aspect_ratio = m_tex_width / m_tex_height;
    const float window_aspect_ratio  = window_size.x / window_size.y;

    ImVec2 render_size;
    if (window_aspect_ratio > desired_aspect_ratio) {
        render_size.y = window_size.y;
        render_size.x = render_size.y * desired_aspect_ratio;
    }
    else {
        render_size.x = window_size.x;
        render_size.y = render_size.x / desired_aspect_ratio;
    }

    render_size.x = std::round(render_size.x);
    render_size.y = std::round(render_size.y);

    const ImVec2 cursor_pos = ImGui::GetCursorPos();

    const float offset_x = cursor_pos.x + (window_size.x - render_size.x) * 0.5f;
    const float offset_y = cursor_pos.y + (window_size.y - render_size.y) * 0.5f;

    ImGui::SetCursorPos(ImVec2(offset_x, offset_y));

    ImGui::Image((void*) (intptr_t) m_texture_id, render_size, ImVec2(0, 1), ImVec2(1, 0));
}

}
