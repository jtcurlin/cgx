// Copyright © 2024 Jacob Curlin

#include "gui/panels/asset_panel.h"
#include "asset/asset_manager.h"
#include "asset/asset.h"

#include "asset/shader.h"

namespace cgx::gui
{
AssetPanel::AssetPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Asset Management", context, manager) {}

AssetPanel::~AssetPanel() = default;

void AssetPanel::render()
{
    render_importers_list();
    render_asset_list(asset::AssetType::Model);
    render_asset_list(asset::AssetType::Material);
    render_asset_list(asset::AssetType::Texture);
    render_asset_list(asset::AssetType::Shader);
    render_asset_list(asset::AssetType::Mesh);
}

void AssetPanel::draw_asset_context_menu(asset::Asset* asset)
{
    CGX_ASSERT(asset, "attempt to draw context menu for invalid node");
    if (ImGui::BeginPopupContextItem("AssetContextMenu")) {

        if (ImGui::MenuItem("Inspect")) {
            m_context->set_item_to_inspect(asset);
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("Rename")) {
            m_context->set_item_to_rename(asset);
            ImGui::CloseCurrentPopup();
        }
        /* todo: implement removal of assets
        else if (ImGui::MenuItem("Remove")) {

            ImGui::CloseCurrentPopup();
        }
        */
        ImGui::EndPopup();
    }
}

void AssetPanel::render_importers_list()
{
    const auto&       importers = m_context->get_asset_manager()->get_importers();
    const std::string title     = "Importers [" + std::to_string(importers.size()) + "]";

    ImGui::Text("%s", title.c_str());
    if (ImGui::BeginChild("RegisteredImportersList##ImporterList", ImVec2(0, 100), true)) {
        for (const auto& importer : importers) {
            auto& label = importer->get_label();
            if (ImGui::Selectable(label.c_str(), label == m_current_importer_label)) {
                m_current_importer_label = label;
            }
            if (ImGui::BeginPopupContextItem()) {
                m_current_importer_label = label;

                // format extensions into str
                std::ostringstream oss;
                for (const auto& ext : importer->get_supported_file_extensions()) {
                    oss << "[" << ext << "] ";
                }
                std::string extensions = oss.str();
                ImGui::Text("Importer Label : %s", label.c_str());
                ImGui::Text("Importer Extensions: %s", extensions.c_str());
                ImGui::EndPopup();
            }
            ImGui::SetItemTooltip("Right Click for Info");
        }
    }
    ImGui::EndChild();
}

void AssetPanel::render_asset_list(const asset::AssetType asset_type)
{
    auto& asset_ids = m_context->get_asset_manager()->getAllIDs(asset_type);
    auto& assets    = m_context->get_asset_manager()->get_assets();

    const std::string type_str = asset::translate_asset_typename(asset_type) + "s";
    const std::string title    = type_str + " [" + std::to_string(asset_ids.size()) + "]";
    const std::string list_id  = type_str + "##asset_list";

    ImGui::Text("%s", title.c_str());
    if (ImGui::BeginChild(list_id.c_str(), ImVec2(0, 150), true)) {
        for (auto& id : asset_ids) {
            auto& asset = assets.at(id);
            auto& name  = asset->get_tag();

            if (ImGui::Selectable(name.c_str(), id == m_current_asset_id)) {
                m_current_asset_id = id;
            }
            ImGui::SetItemTooltip("Right Click for Info");

            draw_asset_context_menu(asset.get());
        }
    }
    ImGui::EndChild();
}
}
