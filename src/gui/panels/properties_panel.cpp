// Copyright © 2024 Jacob Curlin

#include "gui/panels/properties_panel.h"
#include "gui/imgui_manager.h"
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
PropertiesPanel::PropertiesPanel(
    const std::shared_ptr<GUIContext>&   context,
    const std::shared_ptr<ImGuiManager>& manager)
    : ImGuiPanel("Properties", context, manager) {}

PropertiesPanel::~PropertiesPanel() = default;

void PropertiesPanel::render()
{
    if (auto* item = m_context->get_selected_item()) {
        switch (item->get_item_type()) {
            case core::ItemType::Node: {
                if (const auto node = dynamic_cast<scene::Node*>(item)) {
                    draw_node_properties(node);
                }
                break;
            }
            case core::ItemType::Asset: {
                if (const auto asset = dynamic_cast<asset::Asset*>(item)) {
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
    ImGui::PushFont(m_manager.lock()->m_title_font);
    ImGui::Text("%s asset", asset->get_asset_typename().c_str());
    ImGui::PopFont();
    ImGui::Separator();
    draw_asset_metadata(asset);

    switch (asset->get_asset_type()) {
        case asset::AssetType::Model: {
            if (const auto model_asset = dynamic_cast<asset::Model*>(asset)) {
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
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Asset Metadata", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("ID: %zu", asset->get_id());
        ImGui::Text("Tag: %s", asset->get_tag().c_str());
        ImGui::Text("Path: %s", asset->get_internal_path().c_str());
        ImGui::Text("Type: %s", asset->get_asset_typename().c_str());
        ImGui::Separator();
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_node_metadata(const scene::Node* node)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Node Metadata", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("ID: %zu", node->get_id());
        ImGui::Text("Tag: %s", node->get_tag().c_str());
        ImGui::Text("Path: %s", node->get_internal_path().c_str());
        ImGui::Text("Type: %s", node->get_node_typename().c_str());
        ImGui::PopFont();
        ImGui::Separator();
    }
    ImGui::PopFont();
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
            if (const auto camera_node = dynamic_cast<scene::CameraNode*>(node)) {
                draw_camera_node_properties(camera_node);
            }
            break;
        }
        case scene::NodeType::Light: {
            if (const auto light_node = dynamic_cast<scene::LightNode*>(node)) {
                draw_light_node_properties(light_node);
            }
            break;
        }
        case scene::NodeType::Unknown: {
            CGX_ERROR("node properties panel drawn for unknown node type");
            std::exit(1);
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

    ImGui::PushFont(m_manager.lock()->m_title_font);
    ImGui::Text("Entity Node");
    ImGui::PopFont();

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

    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PopFont();
        if (scene->has_component<component::Render>(entity_node)) {
            draw_render_component_editor(entity_node, scene.get());
        }
        if (scene->has_component<component::Transform>(entity_node)) {
            draw_transform_component_editor(entity_node, scene.get());
        }
        if (scene->has_component<component::RigidBody>(entity_node)) {
            draw_rigidbody_component_editor(entity_node, scene.get());
        }
    }
    else {
        ImGui::PopFont();
    }
}

void PropertiesPanel::draw_camera_node_properties(scene::CameraNode* camera_node)
{
    ImGui::Text("todo");
}

void PropertiesPanel::draw_light_node_properties(scene::LightNode* light_node)
{
    ImGui::Text("todo");
}


void PropertiesPanel::draw_render_component_editor(scene::EntityNode* entity_node, scene::Scene* scene)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    const bool editor_opened = ImGui::TreeNodeEx("Render Component", flags);

    if (ImGui::BeginPopupContextItem("Render Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##RenderComponent")) {
            scene->remove_component<component::Render>(entity_node);
        }
        if (ImGui::MenuItem("Reset ##RenderComponent")) {
            auto& component = scene->get_component<component::Render>(entity_node);
            component.model.reset();
            component.shader.reset();
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        const auto asset_manager    = m_context->get_asset_manager();
        auto&      render_component = scene->get_component<component::Render>(entity_node);

        draw_asset_selector<asset::Model>(asset::AssetType::Model, render_component.model, "Model");
        draw_asset_selector<asset::Shader>(asset::AssetType::Shader, render_component.shader, "Shader");
        ImGui::Separator();
    }
}

void PropertiesPanel::draw_transform_component_editor(scene::EntityNode* entity_node, scene::Scene* scene)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    const bool editor_opened = ImGui::TreeNodeEx("Transform Component", flags);

    if (ImGui::BeginPopupContextItem("Transform Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##TransformComponent")) {
            scene->remove_component<component::Transform>(entity_node);
        }
        if (ImGui::MenuItem("Reset ##TransformComponent")) {
            auto& component = scene->get_component<component::Transform>(entity_node);

            component.local_position = glm::vec3(0.0f);
            component.local_rotation = glm::vec3(0.0f);
            component.local_scale    = glm::vec3(1.0f);
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        const auto asset_manager = m_context->get_asset_manager();
        auto&      component     = scene->get_component<component::Transform>(entity_node);

        ImGui::InputFloat3("Position##TransformComponent", &component.local_position[0]);
        ImGui::InputFloat3("Rotation##TransformComponent", &component.local_rotation[0]);
        ImGui::InputFloat3("Scale##TransformComponent", &component.local_scale[0]);
        ImGui::Separator();
    }
}

void PropertiesPanel::draw_rigidbody_component_editor(scene::EntityNode* entity_node, scene::Scene* scene)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("RigidBody Component", flags);

    if (ImGui::BeginPopupContextItem("Rigid Body Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##RigidBodyComponent")) {
            scene->remove_component<component::RigidBody>(entity_node);
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##RigidBodyComponent")) {
            auto& component = scene->get_component<component::RigidBody>(entity_node);

            component.velocity     = glm::vec3(0.0f);
            component.acceleration = glm::vec3(0.0f);
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        const auto asset_manager = m_context->get_asset_manager();
        auto&      component     = scene->get_component<component::RigidBody>(entity_node);

        ImGui::InputFloat3("Velocity ##RigidBodyComponent", &component.velocity[0]);
        ImGui::InputFloat3("Acceleration ##RigidBodyComponent", &component.acceleration[0]);
        ImGui::Separator();
    }
}

void PropertiesPanel::draw_model_asset_editor(asset::Model* model)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Model Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("todo");
    }
    ImGui::PopFont();

}

void PropertiesPanel::draw_mesh_asset_editor(asset::Mesh* mesh)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Mesh Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("VAO ID: %zu", mesh->m_vao);
        ImGui::Text("VBO ID: %zu", mesh->m_vbo);
        ImGui::Text("EBO ID: %zu", mesh->m_ebo);

        ImGui::Text("Vertex Count: %zu", mesh->m_vertices.size());
        ImGui::Text("Index Count: %zu", mesh->m_indices.size());

        draw_asset_selector(asset::AssetType::Material, mesh->m_material, "Material");
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_material_asset_editor(asset::Material* material)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Material Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("Base Colors");
        ImGui::ColorEdit3("Ambient Color", &material->m_ambient_color[0]);
        ImGui::ColorEdit3("Diffuse Color", &material->m_diffuse_color[0]);
        ImGui::ColorEdit3("Specular Color", &material->m_specular_color[0]);
        ImGui::SliderFloat("Shininess", &material->m_shininess, 0.0f, 128.0f);
        ImGui::Separator();

        ImGui::Text("Texture Maps");
        draw_asset_selector<asset::Texture>(asset::AssetType::Texture, material->m_ambient_map, "Ambient Map");
        draw_asset_selector<asset::Texture>(asset::AssetType::Texture, material->m_diffuse_map, "Diffuse Map");
        draw_asset_selector<asset::Texture>(asset::AssetType::Texture, material->m_specular_map, "Specular Map");
        draw_asset_selector<asset::Texture>(asset::AssetType::Texture, material->m_normal_map, "Normal Map");
        ImGui::Separator();
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_texture_asset_editor(asset::Texture* texture)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Texture Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("Texture ID: %zu", texture->m_texture_id);
        ImGui::Text("Width: %zu", texture->m_width);
        ImGui::Text("Height: %zu", texture->m_height);
        ImGui::Text("Channels: %zu", texture->m_num_channels);
        ImGui::Text("GL Format: %zu", texture->m_format);
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_shader_asset_editor(asset::Shader* shader)
{
    ImGui::PushFont(m_manager.lock()->m_header_font);
    if (ImGui::CollapsingHeader("Shader Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager.lock()->m_body_font);
        ImGui::Text("Vertex Shader: %s", shader->m_vert_path.c_str());
        ImGui::Text("Fragment Shader: %s", shader->m_frag_path.c_str());
        ImGui::PopFont();
    }
    ImGui::PopFont();
}
}
