// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "asset/asset_manager.h"

namespace cgx::scene
{
class Node;
class CameraNode;
class EntityNode;
class LightNode;
}

namespace cgx::asset
{
class Asset;
class Model;
class Mesh;
class Material;
class Texture;
class Shader;
}

namespace cgx::scene
{
class Scene;
}

namespace cgx::gui
{
class PropertiesPanel final : public ImGuiPanel
{
public:
    explicit PropertiesPanel(GUIContext* context, ImGuiManager* manager);
    ~PropertiesPanel() override;

    void render() override;

    void draw_asset_properties(asset::Asset* asset);
    void draw_node_properties(const scene::Node* node);

    void draw_item_metadata(const core::Item* item) const;

    void draw_hierarchy_component_editor(const scene::Node* node);
    void draw_render_component_editor(const scene::Node* node);
    void draw_transform_component_editor(const scene::Node* node);
    void draw_rigidbody_component_editor(const scene::Node* node);
    void draw_camera_component_editor(const scene::Node* node);
    void draw_controllable_component_editor(const scene::Node* node);

    void draw_model_asset_editor(asset::Model* model);
    void draw_mesh_asset_editor(asset::Mesh* mesh);
    void draw_material_asset_editor(asset::Material* material);
    void draw_texture_asset_editor(const asset::Texture* texture);
    void draw_shader_asset_editor(const asset::Shader* shader);


    template<typename AssetType>
    void draw_asset_selector(asset::AssetType::Type asset_type, std::shared_ptr<AssetType>& current_asset, const std::string& label) const
    {
        if (ImGui::BeginCombo(label.c_str(), current_asset ? current_asset->get_tag().c_str() : "[None]")) {
            const auto asset_manager = m_context->get_asset_manager();

            for (const auto& asset_id : asset_manager->getAllIDs(asset_type)) {
                auto asset = asset_manager->get_asset(asset_id);
                if (asset) {
                    const bool is_selected = (current_asset && asset->get_id() == current_asset->get_id());
                    if (ImGui::Selectable(asset->get_tag().c_str(), is_selected)) {
                        current_asset = std::static_pointer_cast<AssetType>(asset);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }
    }
};
}
