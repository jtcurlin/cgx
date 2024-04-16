// Copyright © 2024 Jacob Curlin

#include "gui/imgui_panel.h"
#include "gui/gui_context.h"
#include "gui/imgui_manager.h"


namespace cgx::gui
{
void AspectRatioConstraint(ImGuiSizeCallbackData* data)
{
    const float aspect_ratio = *(float*) data->UserData;
    data->DesiredSize.y      = data->DesiredSize.x / aspect_ratio;
}

ImGuiPanel::ImGuiPanel(
    std::string                          title,
    const std::shared_ptr<GUIContext>&   context,
    const std::shared_ptr<ImGuiManager>& manager)
    : m_title(std::move(title))
    , m_context(context)
    , m_manager(manager)
{
    m_window_flags = ImGuiWindowFlags_NoCollapse;
}

void ImGuiPanel::Begin()
{
    on_begin();
    bool keep_visible{true};

    if (m_enforce_aspect_ratio) {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(m_min_size[0], m_min_size[1]),
            ImVec2(m_max_size[0], m_max_size[1]),
            AspectRatioConstraint,
            &m_aspect_ratio);
    }
    else {
        ImGui::SetNextWindowSizeConstraints(ImVec2(m_min_size[0], m_min_size[1]), ImVec2(m_max_size[0], m_max_size[1]));
    }


    // const bool collapsed = (ImGui::Begin(m_title.c_str(), &keep_visible, m_window_flags));
    auto manager = m_manager.lock();

    ImGui::Begin(m_title.c_str(), &keep_visible, m_window_flags);

    if (!keep_visible) {
        hide();
    }
}

void ImGuiPanel::End()
{
    on_end();
    m_is_hovered = ImGui::IsWindowHovered();
    ImGui::End();
}

void ImGuiPanel::on_begin() {} // (child-defined)

void ImGuiPanel::on_end() {} // (child-defined)

void ImGuiPanel::show()
{
    m_is_visible = true;
}

void ImGuiPanel::hide()
{
    m_is_visible = false;
}

bool ImGuiPanel::is_visible() const
{
    return m_is_visible;
}

const std::string& ImGuiPanel::get_title()
{
    return m_title;
}

void ImGuiPanel::set_min_size(float width, float height)
{
    m_min_size[0] = width;
    m_max_size[1] = height;
}

void ImGuiPanel::set_max_size(float width, float height)
{
    m_max_size[0] = width;
    m_max_size[1] = height;
}
}
