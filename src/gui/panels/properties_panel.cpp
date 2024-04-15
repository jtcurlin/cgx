// Copyright © 2024 Jacob Curlin

#include "gui/panels/properties_panel.h"
#include "scene/node.h"

#include "asset/model.h"
#include "asset/mesh.h"
#include "asset/material.h"
#include "asset/texture.h"
#include "asset/shader.h"

#include "ecs/components/render.h"
#include "ecs/components/rigid_body.h"
#include "ecs/components/transform.h"

namespace cgx::gui
{
PropertiesPanel::PropertiesPanel(const std::shared_ptr<GUIContext>& context)
    : ImGuiPanel("Properties", context) {}

PropertiesPanel::~PropertiesPanel() = default;

void PropertiesPanel::render()
{
    if (auto* item = m_context->get_selected_item()) {
        switch (item->get_item_type()) {
            case core::ItemType::Node: {
                if (auto node = dynamic_cast<scene::Node*>(item)) {
                    draw_node_properties(node);
                }
                break;
            }
            case core::ItemType::Asset: {
                if (auto asset = dynamic_cast<asset::Asset*>(item)) {
                    draw_asset_properties(asset);
                }
                break;
            }
            default: {
                ImGui::Text("Unknown Item Type");
                break;
            }
        }
    }
    else {
        ImGui::Text("No Item Selected");
    }
}

void PropertiesPanel::draw_asset_properties(asset::Asset* asset)
{
    ImGui::Text("%s Asset", asset->get_asset_typename().c_str());
    ImGui::Separator();
    draw_asset_metadata(asset);

    switch (asset->get_asset_type()) {
        case asset::AssetType::Model: {
            if (const const auto model_asset = dynamic_cast<asset::Model*>(asset)) {
                draw_model_asset_editor(model_asset);
            }
            break;
        }
        case asset::AssetType::Mesh: {
            if (const auto mesh_asset = dynamic_cast<asset::Mesh*>(asset)) {
                draw_mesh_asset_editor(mesh_asset);
            }
            break;
        }
        case asset::AssetType::Material: {
            if (const auto material_asset = dynamic_cast<asset::Material*>(asset)) {
                draw_material_asset_editor(material_asset);
            }
            break;
        }
        case asset::AssetType::Texture: {
            if (const auto texture_asset = dynamic_cast<asset::Texture*>(asset)) {
                draw_texture_asset_editor(texture_asset);
            }
            break;
        }
        case asset::AssetType::Shader: {
            if (const auto shader_asset = dynamic_cast<asset::Shader*>(asset)) {
                draw_shader_asset_editor(shader_asset);
            }
            break;
        }
        default: {
            ImGui::Text("Unknown Node Type");
            break;
        }
    }


}

void PropertiesPanel::draw_asset_metadata(const asset::Asset* asset)
{
    ImGui::Text("Asset ID: %zu", asset->get_id());
    ImGui::Text("Asset Tag: %s", asset->get_tag().c_str());
    ImGui::Text("Asset Path: %s", asset->get_path().c_str());
    ImGui::Text("Asset Type: %s", asset->get_asset_typename().c_str());
    ImGui::Separator();
}

void PropertiesPanel::draw_node_metadata(const scene::Node* node)
{
    ImGui::Text("Node ID: %zu", node->get_id());
    ImGui::Text("Node Tag: %s", node->get_tag().c_str());
    ImGui::Text("Node Path: %s", node->get_path().c_str());
    ImGui::Text("Node Type: %s", node->get_node_typename().c_str());
    ImGui::Separator();
}

void PropertiesPanel::draw_node_properties(scene::Node* node)
{
    switch (node->get_node_type()) {
        case scene::NodeType::Entity: {
            if (const auto entity_node = dynamic_cast<scene::EntityNode*>(node)) {

                draw_entity_node_properties(entity_node);
            }
            break;
        }
        case scene::NodeType::Camera: {
            if (auto camera_node = dynamic_cast<scene::EntityNode*>(node)) {

                // todo
            }
            break;
        }
        case scene::NodeType::Light: {
            if (auto light_node = dynamic_cast<scene::EntityNode*>(node)) {
                // todo
            }
            break;
        }
        case scene::NodeType::Unknown: {
            if (auto unknown_node = dynamic_cast<scene::EntityNode*>(node)) {
                // todo
            }
            break;
        }
        default: {
            ImGui::Text("Unknown Node Type");
            break;
        }
    }
}

void PropertiesPanel::draw_entity_node_properties(scene::EntityNode* entity_node)
{
    const auto scene = m_context->get_scene_manager()->get_active_scene();

    ImGui::Text("Entity Node");

    const float button_width = ImGui::CalcTextSize("Add Component").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    ImGui::SameLine(ImGui::GetWindowSize().x - button_width - ImGui::GetStyle().WindowPadding.x);

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (!scene->has_component<component::Render>(entity_node)) {
            if (ImGui::Selectable("Render Component")) {
                scene->add_component<component::Render>(entity_node, component::Render{});
                ImGui::CloseCurrentPopup();
            }
        }
        if (!scene->has_component<component::Transform>(entity_node)) {
            if (ImGui::Selectable("Transform Component")) {
                scene->add_component<component::Transform>(entity_node, component::Transform{});
                ImGui::CloseCurrentPopup();
            }
        }
        if (!scene->has_component<component::RigidBody>(entity_node)) {
            if (ImGui::Selectable("RigidBody Component")) {
                scene->add_component<component::RigidBody>(entity_node, component::RigidBody{});
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::Separator();

    draw_node_metadata(entity_node);


    if (scene->has_component<component::Render>(entity_node)) {
        draw_render_component_editor(entity_node);
        ImGui::Separator();
    }
    if (scene->has_component<component::Transform>(entity_node)) {
        draw_transform_component_editor(entity_node);
        ImGui::Separator();
    }
    if (scene->has_component<component::RigidBody>(entity_node)) {
        draw_rigidbody_component_editor(entity_node);
        ImGui::Separator();
    }
}


void PropertiesPanel::draw_render_component_editor(scene::EntityNode* node)
{
    const auto scene = m_context->get_scene_manager()->get_active_scene();
    if (ImGui::Button("Remove")) {
        scene->remove_component<component::Render>(node);
    }
    else {
        const auto asset_manager    = m_context->get_asset_manager();
        auto&      render_component = scene->get_component<component::Render>(node);

        if (ImGui::BeginCombo("Model", render_component.model ? render_component.model->get_tag().c_str() : "[None]")) {
            auto& model_ids = asset_manager->getAllIDs(asset::AssetType::Model);
            for (const auto& model_id : model_ids) {
                const auto model       = asset_manager->get_asset(model_id);
                const bool is_selected = render_component.model != nullptr
                                       ? (render_component.model->get_id() == model_id)
                                       : false;
                if (ImGui::Selectable(model->get_tag().c_str(), is_selected)) {
                    render_component.model = std::dynamic_pointer_cast<asset::Model>(
                        asset_manager->get_asset(model_id));
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo(
            "Shader",
            render_component.shader ? render_component.shader->get_tag().c_str() : "[None]")) {
            auto& shader_ids = asset_manager->getAllIDs(asset::AssetType::Shader);
            for (const auto& shader_id : shader_ids) {
                const auto shader      = asset_manager->get_asset(shader_id);
                const bool is_selected = render_component.shader != nullptr
                                       ? (render_component.model->get_id() == shader_id)
                                       : false;
                if (ImGui::Selectable(shader->get_tag().c_str(), is_selected)) {
                    render_component.shader = std::dynamic_pointer_cast<asset::Shader>(
                        asset_manager->get_asset(shader_id));
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}

void PropertiesPanel::draw_transform_component_editor(scene::EntityNode* node)
{
    const auto scene = m_context->get_scene_manager()->get_active_scene();
    if (ImGui::Button("Remove")) {
        scene->remove_component<component::Transform>(node);
        return;
    }

    const auto asset_manager = m_context->get_asset_manager();
    auto&      component     = scene->get_component<component::Transform>(node);

    ImGui::InputFloat3("Position##TransformComponent", &component.local_position[0]);
    ImGui::InputFloat3("Rotation##TransformComponent", &component.local_rotation[0]);
    ImGui::InputFloat3("Scale##TransformComponent", &component.local_scale[0]);
}

void PropertiesPanel::draw_rigidbody_component_editor(scene::EntityNode* entity_node)
{
    const auto scene = m_context->get_scene_manager()->get_active_scene();
    if (ImGui::Button("Remove")) {
        scene->remove_component<component::Transform>(entity_node);
        return;
    }

    const auto asset_manager = m_context->get_asset_manager();
    auto&      component     = scene->get_component<component::RigidBody>(entity_node);

    ImGui::InputFloat3("Velocity##RigidBodyComponent", &component.velocity[0]);
    ImGui::InputFloat3("Acceleration##RigidBodyComponent", &component.acceleration[0]);
}

void PropertiesPanel::draw_model_asset_editor(asset::Model* model)
{

}

void PropertiesPanel::draw_mesh_asset_editor(asset::Mesh* mesh)
{

}

void PropertiesPanel::draw_material_asset_editor(asset::Material* material)
{

    ImGui::ColorEdit3("Ambient Color", &material->m_ambient_color[0]);
    ImGui::ColorEdit3("Diffuse Color", &material->m_diffuse_color[0]);
    ImGui::ColorEdit3("Specular Color", &material->m_specular_color[0]);
    ImGui::SliderFloat("Shininess", &material->m_shininess, 0.0f, 128.0f);
}

void PropertiesPanel::draw_texture_asset_editor(asset::Texture* texture)
{
    ImGui::Text("Hi");
}

void PropertiesPanel::draw_shader_asset_editor(asset::Shader* shader)
{

}


}
