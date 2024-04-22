// Copyright © 2024 Jacob Curlin

#include "gui/panels/properties_panel.h"
#include "gui/imgui_manager.h"

#include "asset/model.h"
#include "asset/mesh.h"
#include "asset/material.h"
#include "asset/texture.h"
#include "asset/shader.h"

#include "core/components/render.h"
#include "core/components/rigid_body.h"
#include "core/components/transform.h"
#include "core/components/hierarchy.h"

#include "ecs/ecs_manager.h"
#include "ecs/event_handler.h"
#include "core/events/engine_events.h"

#include "scene/node.h"

namespace cgx::gui
{
PropertiesPanel::PropertiesPanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Properties", context, manager) {}

PropertiesPanel::~PropertiesPanel() = default;

void PropertiesPanel::render()
{
    if (auto* item = m_context->get_item_to_inspect()) {
        switch (item->get_item_type()) {
            case core::ItemType::Node: {
                const auto node = dynamic_cast<scene::Node*>(item);
                CGX_ASSERT(node, "attempt to display node properties for non-node item type");
                draw_node_properties(node);
                break;
            }
            case core::ItemType::Asset: {
                const auto asset = dynamic_cast<asset::Asset*>(item);
                CGX_ASSERT(asset, "attempt to display asset properties for non-asset item type");
                draw_asset_properties(asset);
                break;
            }
            default: {
                CGX_FATAL("attempt to display properties for item of unknown type");
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
    ImGui::PushFont(m_manager->m_title_font);
    ImGui::Text("%s asset", asset->get_asset_typename().c_str());
    ImGui::PopFont();
    ImGui::Separator();
    draw_item_metadata(asset);

    switch (asset->get_asset_type()) {
        case asset::AssetType::Model: {
            const auto model = dynamic_cast<asset::Model*>(asset);
            CGX_ASSERT(model, "attempt to display model properties for non-model asset type");

            draw_model_asset_editor(model);
            break;
        }
        case asset::AssetType::Mesh: {
            const auto mesh = dynamic_cast<asset::Mesh*>(asset);
            CGX_ASSERT(mesh, "attempt to display mesh properties for non-mesh asset type");
            draw_mesh_asset_editor(mesh);
            break;
        }
        case asset::AssetType::Material: {
            const auto material = dynamic_cast<asset::Material*>(asset);
            CGX_ASSERT(material, "attempt to display material properties for non-material asset type");
            draw_material_asset_editor(material);
            break;
        }
        case asset::AssetType::Texture: {
            const auto texture = dynamic_cast<asset::Texture*>(asset);
            CGX_ASSERT(texture, "attempt to display texture properties for non-texture asset type");
            draw_texture_asset_editor(texture);
            break;
        }
        case asset::AssetType::Shader: {
            const auto shader = dynamic_cast<asset::Shader*>(asset);
            CGX_ASSERT(shader, "attempt to display shader properties for non-shader asset type");
            draw_shader_asset_editor(shader);
            break;
        }
        default: {
            CGX_FATAL("attempt to display properties for assedt of unknown type");
            break;
        }
    }
}

void PropertiesPanel::draw_item_metadata(const core::Item* item) const
{
    const std::string title = "Info";

    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
        ImGui::Text("ID: %zu", item->get_id());
        ImGui::Text("Tag: %s", item->get_tag().c_str());
        ImGui::Text("Path: %s", item->get_internal_path().c_str());
        ImGui::PopFont();
        ImGui::Separator();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_hierarchy_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    const bool editor_opened = ImGui::TreeNodeEx("Hierarchy Component", flags);
    if (editor_opened) {
        const auto& hierarchy_component = m_context->get_ecs_manager()->get_component<component::Hierarchy>(
            node->get_entity());

        const std::string parent = std::to_string(hierarchy_component.parent);
        std::string       children;
        if (hierarchy_component.children.size() > 0) {
            for (const auto& child : hierarchy_component.children) {
                children += "[ " + std::to_string(child) + " ] ";
            }
        }
        else {
            children = "[ None ]";
        }
        ImGui::Text("Parent:   %s", parent.c_str());
        ImGui::Text("Children: %s", children.c_str());
        ImGui::Separator();
    }
}

void PropertiesPanel::draw_node_properties(const scene::Node* node)
{
    auto*      ecs_manager = m_context->get_ecs_manager();
    const auto entity      = node->get_entity();

    ImGui::PushFont(m_manager->m_title_font);
    ImGui::Text("Node");
    ImGui::PopFont();

    const float button_width = ImGui::CalcTextSize("Add Component").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    ImGui::SameLine(ImGui::GetWindowSize().x - button_width - ImGui::GetStyle().WindowPadding.x);

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (!ecs_manager->has_component<component::Render>(entity)) {
            if (ImGui::MenuItem("Render Component")) {
                ecs_manager->add_component<component::Render>(entity, component::Render{});
                ImGui::CloseCurrentPopup();
            }

        }
        if (!ecs_manager->has_component<component::Transform>(entity)) {
            if (ImGui::Selectable("Transform Component")) {
                ecs_manager->add_component<component::Transform>(entity, component::Transform{});
                ImGui::CloseCurrentPopup();

            }
        }
        if (!ecs_manager->has_component<component::RigidBody>(entity)) {
            if (ImGui::Selectable("RigidBody Component")) {
                ecs_manager->add_component<component::RigidBody>(entity, component::RigidBody{});
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::Separator();

    draw_item_metadata(node);


    ImGui::PushFont(m_manager->m_header_font);
    std::string entity_id_text = "Entity #" + std::to_string(node->get_entity());

    ImGui::TreeNodeEx(
        entity_id_text.c_str(),
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog);

    if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PopFont();
        if (ecs_manager->has_component<component::Hierarchy>(entity)) {
            draw_hierarchy_component_editor(node);
        }
        if (ecs_manager->has_component<component::Render>(entity)) {
            draw_render_component_editor(node);
        }
        if (ecs_manager->has_component<component::Transform>(entity)) {
            draw_transform_component_editor(node);
        }
        if (ecs_manager->has_component<component::RigidBody>(entity)) {
            draw_rigidbody_component_editor(node);
        }

    }
    else {
        ImGui::PopFont();
    }
}

void PropertiesPanel::draw_render_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Render Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Render Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##RenderComponent")) {
            m_context->get_ecs_manager()->remove_component<component::Render>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##RenderComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::Render>(node->get_entity());
            component.model.reset();
            component.shader.reset();
            updated = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& render_component = m_context->get_ecs_manager()->get_component<component::Render>(node->get_entity());

        // todo: check for update of shader/asset & set 'update' bool
        draw_asset_selector<asset::Model>(asset::AssetType::Model, render_component.model, "Model");
        draw_asset_selector<asset::Shader>(asset::AssetType::Shader, render_component.shader, "Shader");
        ImGui::Separator();
    }

    if (removed) {
        ecs::Event event(events::component::REMOVED);
        event.set_param(events::component::TYPE, m_context->get_ecs_manager()->get_component_type<component::Render>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
    }
    if (updated) {
        ecs::Event event(events::component::UPDATED);
        event.set_param(events::component::TYPE, m_context->get_ecs_manager()->get_component_type<component::Render>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
    }
}

void PropertiesPanel::draw_transform_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Transform Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Transform Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##TransformComponent")) {
            m_context->get_ecs_manager()->remove_component<component::Transform>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##TransformComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::Transform>(node->get_entity());

            component.local_position = glm::vec3(0.0f);
            component.local_rotation = glm::vec3(0.0f);
            component.local_scale    = glm::vec3(1.0f);
            updated                  = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::Transform>(node->get_entity());

        updated |= ImGui::InputFloat3("Position##TransformComponent", &component.local_position[0]);
        updated |= ImGui::InputFloat3("Rotation##TransformComponent", &component.local_rotation[0]);
        updated |= ImGui::InputFloat3("Scale##TransformComponent", &component.local_scale[0]);

        if (updated) {
            component.dirty = true;
        }


    }
    if (removed) {
        ecs::Event event(events::component::REMOVED);
        event.set_param(
            events::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
        ecs::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        ecs::Event event(events::component::UPDATED);
        event.set_param(
            events::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
        ecs::EventHandler::get_instance().send_event(event);
    }
}

void PropertiesPanel::draw_rigidbody_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("RigidBody Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Rigid Body Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##RigidBodyComponent")) {
            m_context->get_ecs_manager()->remove_component<component::RigidBody>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##RigidBodyComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::RigidBody>(node->get_entity());
            component.velocity = glm::vec3(0.0f);
            component.acceleration = glm::vec3(0.0f);
            component.angular_velocity = glm::vec3(0.0f);
            component.scale_rate = glm::vec3(0.0f);
            updated = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::RigidBody>(node->get_entity());

        updated |= ImGui::InputFloat3("Velocity ##RigidBodyComponent", &component.velocity[0]);
        updated |= ImGui::InputFloat3("Angular Velocity ##RigidBodyComponent", &component.angular_velocity[0]);
        updated |= ImGui::InputFloat3("Scale Rate ##RigidBodyComponent", &component.scale_rate[0]);
        updated |= ImGui::InputFloat3("Acceleration ##RigidBodyComponent", &component.acceleration[0]);

        ImGui::Separator();
    }

    if (updated) {
        ecs::Event event(events::component::UPDATED);
        event.set_param(
            events::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::RigidBody>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
    }
    if (removed) {
        ecs::Event event(events::component::REMOVED);
        event.set_param(
            events::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::RigidBody>());
        event.set_param(events::component::ENTITY_ID, node->get_entity());
    }
}

void PropertiesPanel::draw_model_asset_editor(asset::Model* model)
{
    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader("Model Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("todo");
    }
    ImGui::PopFont();

}

void PropertiesPanel::draw_mesh_asset_editor(asset::Mesh* mesh)
{
    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader("Mesh Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
        ImGui::Text("VAO ID: %u", mesh->m_vao);
        ImGui::Text("VBO ID: %u", mesh->m_vbo);
        ImGui::Text("EBO ID: %u", mesh->m_ebo);

        ImGui::Text("Vertex Count: %zu", mesh->m_vertices.size());
        ImGui::Text("Index Count: %zu", mesh->m_indices.size());

        draw_asset_selector(asset::AssetType::Material, mesh->m_material, "Material");
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_material_asset_editor(asset::Material* material)
{
    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader("Material Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
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

void PropertiesPanel::draw_texture_asset_editor(const asset::Texture* texture)
{
    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader("Texture Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
        ImGui::Text("Texture ID: %u", texture->m_texture_id);
        ImGui::Text("Width: %u", texture->m_width);
        ImGui::Text("Height: %u", texture->m_height);
        ImGui::Text("Channels: %u", texture->m_num_channels);
        ImGui::Text("GL Format: %u", texture->m_format);
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_shader_asset_editor(const asset::Shader* shader)
{
    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader("Shader Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
        ImGui::Text("Vertex Shader: %s", shader->m_vert_path.c_str());
        ImGui::Text("Fragment Shader: %s", shader->m_frag_path.c_str());
        ImGui::PopFont();
    }
    ImGui::PopFont();
}
}
