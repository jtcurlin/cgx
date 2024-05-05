// Copyright © 2024 Jacob Curlin

#include "gui/panels/render_settings_panel.h"

#include <complex>

#include "asset/asset_manager.h"
#include "asset/cubemap.h"
#include "asset/shader.h"

#include "render/render_system.h"

namespace cgx::gui
{
RenderSettingsPanel::RenderSettingsPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Render Settings", context, manager) {}

RenderSettingsPanel::~RenderSettingsPanel() = default;

void RenderSettingsPanel::render()
{
    auto& render_settings = m_context->get_render_system()->get_render_settings();

    if (ImGui::BeginTable("RenderSettingsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Settings");
        ImGui::TableSetupColumn("G-Buffer");
        ImGui::TableHeadersRow();

        ImGui::TableNextColumn();
        if (ImGui::BeginTable("Settings Table", 1, ImGuiTableFlags_Resizable)) {
            // ImGui::TableNextRow(ImGuiTableRowFlags_None, 100);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Enable MSAA", &render_settings.msaa_enabled);
            ImGui::TableNextColumn();
            ImGui::Checkbox("Enable Rendering Test", &render_settings.m_render_test_enabled);
            ImGui::TableNextColumn();
            ImGui::Checkbox("Enable Default Shader", &render_settings.default_shader_enabled);
            ImGui::TableNextColumn();
            ImGui::Checkbox("Enable Default Model", &render_settings.default_model_enabled);
            ImGui::TableNextColumn();
            ImGui::Checkbox("Draw Colliders", &render_settings.draw_colliders);

            ImGui::EndTable();
        }

        // draw_skybox_menu();

        auto position_texture  = m_context->get_render_system()->get_g_buffer()->get_texture(GL_COLOR_ATTACHMENT0);
        auto normal_texture    = m_context->get_render_system()->get_g_buffer()->get_texture(GL_COLOR_ATTACHMENT1);
        auto albedo_texture    = m_context->get_render_system()->get_g_buffer()->get_texture(GL_COLOR_ATTACHMENT2);
        auto metallic_texture  = m_context->get_render_system()->get_g_buffer()->get_texture(GL_COLOR_ATTACHMENT3);
        auto roughness_texture = m_context->get_render_system()->get_g_buffer()->get_texture(GL_COLOR_ATTACHMENT4);

        position_texture->bind(0);
        normal_texture->bind(1);
        albedo_texture->bind(2);
        metallic_texture->bind(3);
        roughness_texture->bind(4);

        ImGui::TableNextColumn();
        if (ImGui::BeginTable(
            "gBufferViewTable",
            5,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH)) {
            ImGui::TableSetupColumn("Position");
            ImGui::TableSetupColumn("Normal");
            ImGui::TableSetupColumn("Albedo");
            ImGui::TableSetupColumn("Metallic");
            ImGui::TableSetupColumn("Roughness");
            ImGui::TableHeadersRow();

            ImGui::TableNextColumn();
            ImGui::Image((void*) (intptr_t) position_texture->get_texture_id(), ImVec2(256, 144));
            ImGui::TableNextColumn();
            ImGui::Image((void*) (intptr_t) normal_texture->get_texture_id(), ImVec2(256, 144));
            ImGui::TableNextColumn();
            ImGui::Image((void*) (intptr_t) albedo_texture->get_texture_id(), ImVec2(256, 144));
            ImGui::TableNextColumn();
            ImGui::Image((void*) (intptr_t) metallic_texture->get_texture_id(), ImVec2(256, 144));
            ImGui::TableNextColumn();
            ImGui::Image((void*) (intptr_t) roughness_texture->get_texture_id(), ImVec2(256, 144));

            ImGui::EndTable();
        }

        ImGui::EndTable();
    }
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
            auto& cubemap_ids = asset_manager->getAllIDs(asset::AssetType::Type::Cubemap);
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
