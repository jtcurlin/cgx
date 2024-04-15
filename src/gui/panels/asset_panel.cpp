// Copyright © 2024 Jacob Curlin

#include "gui/panels/asset_panel.h"
#include "asset/asset.h"

#include "asset/model.h"
#include "asset/material.h"
#include "asset/mesh.h"
#include "asset/texture.h"
#include "asset/shader.h"

#include <filesystem>

namespace cgx::gui
{
AssetPanel::AssetPanel(const std::shared_ptr<GUIContext> &context)
    : ImGuiPanel("Resource Management", context)
{
}

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

void AssetPanel::render_importers_list()
{
    const auto& importers = m_context->get_asset_manager()->get_importers();
    const std::string title = "Importers [" + std::to_string(importers.size()) + "]";

    ImGui::Text("%s", title.c_str());
    if (ImGui::BeginChild("RegisteredImportersList", ImVec2(0, 100), true)) {
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
    auto& assets = m_context->get_asset_manager()->get_assets();

    std::string type_str = asset::translate_asset_typename(asset_type) + "s";
    const std::string title = type_str + " [" + std::to_string(asset_ids.size()) + "]";
    const std::string list_id = type_str + "##asset_list";

    ImGui::Text("%s", title.c_str());
    if (ImGui::BeginChild(list_id.c_str(), ImVec2(0, 150), true)) {
        for (auto& id : asset_ids) {
            auto& asset = assets.at(id);
            auto& name = asset->get_tag();

            if (ImGui::Selectable(name.c_str(), id == m_current_asset_id)) {
                m_current_asset_id = id;
            }

            if (ImGui::BeginPopupContextItem()) {
                m_current_asset_id = id;

                auto& path = asset->get_path();
                auto& source_path = asset->get_source_path();
                auto type = asset->get_asset_typename();

                ImGui::Text("%s Asset", asset->get_asset_typename().c_str());

                ImVec2 windowSize = ImGui::GetWindowSize();
                float buttonWidth = ImGui::CalcTextSize("Inspect").x + ImGui::GetStyle().FramePadding.x * 2.0f;
                ImGui::SameLine(windowSize.x - buttonWidth - ImGui::GetStyle().WindowPadding.x);

                if (ImGui::Button("Inspect")) {
                    m_context->set_selected_item(asset.get());
                    ImGui::CloseCurrentPopup();
                }

                ImGui::Text("Asset ID: %zu", id);
                ImGui::Text("Asset Name: %s", name.c_str());
                ImGui::Text("Asset Path: %s", path.c_str());
                ImGui::Text("Asset Type: %s", type.c_str());
                ImGui::Text("Asset Source Path: %s", source_path.c_str());
                ImGui::EndPopup();
            }
            ImGui::SetItemTooltip("Right Click for Info");
        }
    }
    ImGui::EndChild();
}

std::filesystem::path render_import_menu()
{
    // todo
}

}
