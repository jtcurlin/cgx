// Copyright © 2024 Jacob Curlin

#include "gui/panels/asset_panel.h"
#include "gui/imgui_manager.h"
#include "asset/import/asset_importer.h"
#include "asset/asset_manager.h"
#include "asset/asset.h"


namespace cgx::gui
{
AssetPanel::AssetPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Asset Management", context, manager) {}

AssetPanel::~AssetPanel() = default;

void AssetPanel::render()
{
    ImGui::PushFont(m_manager->m_header_font);
    ImGui::Text("Registered Importers");
    ImGui::PopFont();
    render_importers_list();

    ImGui::Separator();

    ImGui::PushFont(m_manager->m_header_font);
    ImGui::Text("Assets");
    ImGui::PopFont();
    render_asset_lists();
}

void AssetPanel::render_asset_lists()
{
    render_asset_list(asset::AssetType::Type::Model);
    render_asset_list(asset::AssetType::Type::Material);
    render_asset_list(asset::AssetType::Type::Texture);
    render_asset_list(asset::AssetType::Type::Shader);
    render_asset_list(asset::AssetType::Type::Mesh);
    render_asset_list(asset::AssetType::Type::Cubemap);
}

void AssetPanel::render_importers_list()
{
    const auto& importers = m_context->get_asset_manager()->get_importers();
    if (ImGui::BeginChild("Registered Importer List ##AssetImporterList", ImVec2(0, 100), true)) {
        for (const auto& importer : importers) {
            auto& label = importer->get_label();
            if (ImGui::Selectable(label.c_str(), label == m_current_importer_label)) {
                m_current_importer_label = label;
            }
            if (ImGui::BeginPopupContextItem()) {
                m_current_importer_label = label;

                std::ostringstream oss;
                for (const auto& ext : importer->get_supported_file_extensions()) {
                    oss << "[" << ext << "] ";
                }
                std::string extensions = oss.str();
                ImGui::Text("Label : %s", label.c_str());
                ImGui::Text("Extensions: %s", extensions.c_str());
                ImGui::EndPopup();
            }
            ImGui::SetItemTooltip("Right Click for Info");
        }
    }
    ImGui::EndChild();
}

void AssetPanel::render_asset_list(const asset::AssetType::Type asset_type)
{
    auto& asset_ids = m_context->get_asset_manager()->getAllIDs(asset_type);
    auto& assets    = m_context->get_asset_manager()->get_assets();

    const std::string asset_list_label = asset::AssetType::get_typename(asset_type) + "s";
    const std::string asset_list_id    = asset_list_label + "##asset_list";
    const std::string count_str        = "Count: " + std::to_string(asset_ids.size());

    ImGui::PushFont(m_manager->m_header_font);
    const bool list_opened = ImGui::CollapsingHeader(asset_list_label.c_str());
    ImGui::PopFont();

    ImGui::PushFont(m_manager->m_body_font);
    const float offset = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(count_str.c_str()).x;
    if (offset > 0) {
        ImGui::SameLine(offset);
    }
    ImGui::Text("%s", count_str.c_str());

    if (list_opened) {
        if (ImGui::BeginChild(asset_list_id.c_str(), ImVec2(0, 150), true)) {
            for (auto& id : asset_ids) {
                auto& asset = assets.at(id);

                std::string unique_label = asset->get_tag() + "##" + std::to_string(asset->get_id());
                if (ImGui::Selectable(unique_label.c_str(), id == m_current_asset_id)) {
                    m_current_asset_id = id;
                }

                if (ImGui::BeginPopupContextItem(unique_label.c_str())) {
                    if (ImGui::MenuItem("Inspect Asset")) {
                        m_context->set_item_to_inspect(asset.get());
                        ImGui::CloseCurrentPopup();
                    }
                    else if (ImGui::MenuItem("Rename Asset")) {
                        m_context->set_item_to_rename(asset.get());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::SetItemTooltip("Right click for options.");
            }
        }
        ImGui::EndChild();
        ImGui::Separator();
    }
    ImGui::PopFont();
}
}
