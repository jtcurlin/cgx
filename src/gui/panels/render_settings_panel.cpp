// Copyright © 2024 Jacob Curlin

#include "gui/panels/render_settings_panel.h"

#include "asset/cubemap.h"
#include "asset/model.h"
#include "asset/shader.h"

namespace cgx::gui
{
RenderSettingsPanel::RenderSettingsPanel(
    const std::shared_ptr<GUIContext>&   context,
    const std::shared_ptr<ImGuiManager>& manager)
    : ImGuiPanel("Render Settings", context, manager) {}

RenderSettingsPanel::~RenderSettingsPanel() = default;

void RenderSettingsPanel::render()
{
    auto& render_settings = m_context->get_render_system()->get_render_settings();

    draw_skybox_menu();
    ImGui::Checkbox("Enable MSAA", &render_settings.msaa_enabled);
    ImGui::Checkbox("Enable Rendering Test", &render_settings.m_render_test_enabled);

    float label_width = ImGui::CalcTextSize("Camera Movement Speed").x;
    float slider_width = ImGui::GetContentRegionAvail().x - label_width - ImGui::GetStyle().ItemInnerSpacing.x * 2; // Adjust spacing as necessary
    ImGui::SetNextItemWidth(slider_width);
    ImGui::SliderFloat("Camera Movement Speed", &m_context->get_render_system()->get_camera()->m_movement_speed, 0.0f, 10.0f);
}

void RenderSettingsPanel::draw_skybox_menu() const
{
    const auto render_system   = m_context->get_render_system();
    auto&      render_settings = render_system->get_render_settings();

    ImGui::Checkbox("Enable Skybox", &render_settings.skybox_enabled);


    const float button_width = ImGui::CalcTextSize("\uf1de").x + ImGui::GetStyle().ItemInnerSpacing.x * 2;
    ImGui::SameLine(ImGui::GetWindowWidth() - button_width - ImGui::GetStyle().WindowPadding.x);

    if (ImGui::Button("\uf1de")) {
        ImGui::OpenPopup("Skybox Settings");
    }

    if (ImGui::BeginPopup("Skybox Settings")) {
        const auto asset_manager = m_context->get_asset_manager();
        ImGui::Text("Skybox Settings");

        std::string preview_text;
        preview_text = render_system->get_skybox_cubemap()
                           ? render_system->get_skybox_cubemap()->get_tag()
                           : "Select Cubemap";
        if (ImGui::BeginCombo("Cubemap", preview_text.c_str())) {
            auto& cubemap_ids = asset_manager->getAllIDs(asset::AssetType::Cubemap);
            for (const auto& cubemap_id : cubemap_ids) {
                auto       cubemap     = asset_manager->get_asset(cubemap_id);
                const bool is_selected = cubemap != nullptr ? (cubemap->get_id() == cubemap_id) : false;
                if (ImGui::Selectable(cubemap->get_tag().c_str(), is_selected)) {
                    render_system->set_skybox_cubemap(dynamic_pointer_cast<asset::Cubemap>(cubemap));
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::EndPopup();
    }
}
}
