// Copyright © 2024 Jacob Curlin

#include "gui/panels/properties_panel.h"

#include <imgui_internal.h>

#include "gui/imgui_manager.h"

#include "asset/model.h"
#include "asset/mesh.h"
#include "asset/pbr_material.h"
#include "asset/phong_material.h"
#include "asset/texture.h"
#include "asset/shader.h"

#include "core/components/render.h"
#include "core/components/rigid_body.h"
#include "core/components/transform.h"
#include "core/components/hierarchy.h"
#include "core/components/camera.h"
#include "core/components/controllable.h"
#include "core/components/collider.h"

#include "ecs/ecs_manager.h"

#include "core/event_handler.h"
#include "core/components/point_light.h"
#include "core/events/ecs_events.h"

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

void PropertiesPanel::draw_node_properties(scene::Node* node)
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
        if (!ecs_manager->has_component<component::Collider>(entity)) {
            if (ImGui::Selectable("Collider Component")) {
                ecs_manager->add_component<component::Collider>(entity, component::Collider{});
                ImGui::CloseCurrentPopup();
            }
        }
        if (!ecs_manager->has_component<component::Camera>(entity)) {
            if (ImGui::Selectable("Camera Component")) {
                ecs_manager->add_component<component::Camera>(entity, component::Camera{});
                ImGui::CloseCurrentPopup();
            }
        }
        if (!ecs_manager->has_component<component::Controllable>(entity)) {
            if (ImGui::Selectable("Controllable Component")) {
                ecs_manager->add_component<component::Controllable>(entity, component::Controllable{});
                ImGui::CloseCurrentPopup();
            }
        }
        if (!ecs_manager->has_component<component::PointLight>(entity)) {
            if (ImGui::Selectable("Point Light Component")) {
                ecs_manager->add_component<component::PointLight>(entity, component::PointLight{});
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::Separator();

    draw_item_metadata(node);

    ImGui::PushFont(m_manager->m_header_font);

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
        if (ecs_manager->has_component<component::Camera>(entity)) {
            draw_camera_component_editor(node);
        }
        if (ecs_manager->has_component<component::Controllable>(entity)) {
            draw_controllable_component_editor(node);
        }
        if (ecs_manager->has_component<component::RigidBody>(entity)) {
            draw_rigidbody_component_editor(node);
        }
        if (ecs_manager->has_component<component::Collider>(entity)) {
            draw_collider_component_editor(node);
        }
        if (ecs_manager->has_component<component::PointLight>(entity)) {
            draw_point_light_component_editor(node);
        }
    }
    else {
        ImGui::PopFont();
    }
}

void PropertiesPanel::draw_item_metadata(core::Item* item)
{
    const std::string title = "Info";

    ImGui::PushFont(m_manager->m_header_font);
    if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::PushFont(m_manager->m_body_font);
        if (ImGui::BeginTable(
            "Editor##ItemMetadata",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {

            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("ID");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%zu", item->get_id());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Tag");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%s", item->get_tag().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Internal Path");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%s", item->get_internal_path().c_str());

            if (item->get_item_type() == core::ItemType::Node) {
                const auto entity = dynamic_cast<scene::Node*>(item)->get_entity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Entity ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Text("%zu", static_cast<size_t>(entity));
            }
            else if (item->get_item_type() == core::ItemType::Asset) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("External Path");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Text("%s", item->get_external_path().c_str());
            }
            ImGui::EndTable();
        }

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
        const auto& component = m_context->get_ecs_manager()->get_component<component::Hierarchy>(node->get_entity());

        const std::string parent = std::to_string(component.parent);
        std::string       children;
        if (component.children.size() > 0) {
            for (const auto& child : component.children) {
                children += "[ " + std::to_string(child) + " ] ";
            }
        }
        else {
            children = "[ None ]";
        }

        if (ImGui::BeginTable(
            "Editor##HierarchyEditor",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Parent");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%s", parent.c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Children");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%s", children.c_str());

            ImGui::EndTable();
        }
        ImGui::Separator();
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
        auto& component = m_context->get_ecs_manager()->get_component<component::Render>(node->get_entity());

        // todo: check for update of shader/asset & set 'update' bool

        if (ImGui::BeginTable(
            "Editor##RenderComponent",
            2,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable)) {

            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Mesh");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            draw_asset_selector<asset::Model>(asset::AssetType::Model, component.model, "##ModelDropdown");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Shader");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            draw_asset_selector<asset::Shader>(asset::AssetType::Shader, component.shader, "##ShaderDropdown");

            ImGui::EndTable();
        }

        ImGui::Separator();
    }

    if (removed) {
        core::event::Event event(core::event::component::REMOVED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Render>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Render>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
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

            component.translation = glm::vec3(0.0f);
            component.rotation    = glm::vec3(0.0f);
            component.scale       = glm::vec3(1.0f);
            component.dirty       = true;
            updated               = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::Transform>(node->get_entity());

        if (ImGui::BeginTable("TransformEditor", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            /*
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });



            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(m_manager->m_header_font);
            if (ImGui::Button("X")) {
                component.translation.x = 0;
                updated = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            updated |= ImGui::DragFloat("##X", &component.translation.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            */
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Translation");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##PositionSlider", &component.translation[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Rotation");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##RotationSlider", &component.rotation[0]);

            /*
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(m_manager->m_header_font);
            if (ImGui::Button("Y")) {
                component.translation.y = 0;
                updated = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            updated |= ImGui::DragFloat("##Y", &component.translation.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            */

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##ScaleSlider", &component.scale[0]);

            /*
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(m_manager->m_header_font);
            if (ImGui::Button("Z")) {
                component.translation.z = 0;
                updated = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            updated |= ImGui::DragFloat("##Z", &component.translation.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            */

            ImGui::EndTable();
        }

        if (updated) {
            component.dirty = true;
        }

        ImGui::Separator();
    }
    if (removed) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
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

        if (ImGui::BeginTable(
            "Editor##RenderComponent",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {

            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Velocity");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##VelocitySlider", &component.velocity[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Angular Velocity");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##AngularVelocitySlider", &component.angular_velocity[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Scale Rate");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##ScaleSlider", &component.scale_rate[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Acceleration");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##AccelerationSlider", &component.acceleration[0]);

            ImGui::EndTable();
            ImGui::Separator();
        }
    }

    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::RigidBody>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
    }
    if (removed) {
        core::event::Event event(core::event::component::REMOVED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::RigidBody>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
    }
}

void PropertiesPanel::draw_collider_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Collider Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Collider Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##ColliderComponent")) {
            m_context->get_ecs_manager()->remove_component<component::Collider>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##ColliderComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::Collider>(node->get_entity());

            component.size = glm::vec3(1.0f);
            updated        = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::Collider>(node->get_entity());

        if (ImGui::BeginTable("ColliderEditor", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Camera Type");
            ImGui::TableSetColumnIndex(1);
            const char* camera_types[] = {"AABB", "Sphere"};
            int         current_type   = static_cast<int>(component.type);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##CameraType", &current_type, camera_types, IM_ARRAYSIZE(camera_types))) {
                component.type = static_cast<component::Collider::Type>(current_type);
                updated        = true;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Size");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##Size", &component.size[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Physical Type");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Button(
                component.is_static ? "STATIC##Collider" : "DYNAMIC##Collider",
                ImVec2(-FLT_MIN, 0.0f))) {
                component.is_static = !component.is_static;
                updated                         = true;
            }


            ImGui::EndTable();
        }
        ImGui::Separator();
    }
    if (removed) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Transform>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
}

void PropertiesPanel::draw_camera_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Camera Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Camera Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##CameraComponent")) {
            m_context->get_ecs_manager()->remove_component<component::Camera>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##CameraComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::Camera>(node->get_entity());

            component = component::Camera{};
            updated   = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::Camera>(node->get_entity());

        if (ImGui::BeginTable(
            "Editor##CameraComponent",
            2,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Camera Type");
            ImGui::TableSetColumnIndex(1);
            const char* camera_types[] = {"Perspective", "Orthographic"};
            int         current_item   = static_cast<int>(component.type);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##CameraType", &current_item, camera_types, IM_ARRAYSIZE(camera_types))) {
                component.type = static_cast<component::Camera::Type>(current_item);
                updated        = true;
            }

            if (component.type == component::Camera::Perspective) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Field of View");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                updated |= ImGui::SliderFloat("##FOV", &component.fov, 1.0f, 180.0f, "%.0f degrees");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Aspect Ratio");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                updated |= ImGui::SliderFloat("##AspectRatio", &component.aspect_ratio, 0.1f, 4.0f, "%.2f");
            }
            else if (component.type == component::Camera::Orthographic) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("X Magnification");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                updated |= ImGui::SliderFloat("##XMag", &component.x_mag, 0.1f, 10.0f, "%.1f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Y Magnification");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                updated |= ImGui::SliderFloat("##YMag", &component.y_mag, 0.1f, 10.0f, "%.1f");
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Near Plane");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::SliderFloat("##NearPlane", &component.near_plane, 0.01f, 10.0f, "%.2f");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Far Plane");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::SliderFloat("##FarPlane", &component.far_plane, 0.1f, 1000.0f, "%.0f");

            ImGui::EndTable();
        }

        ImGui::Separator();
    }

    if (removed) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Camera>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Camera>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
}

void PropertiesPanel::draw_controllable_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Controllable Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Controllable Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##ControllableComponent")) {
            m_context->get_ecs_manager()->remove_component<component::Controllable>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##ControllableComponent")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::Controllable>(node->get_entity());

            component = component::Controllable{};
            updated   = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::Controllable>(node->get_entity());

        if (ImGui::BeginTable(
            "Editor##ControllableComponent",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Movement Speed");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##MovementSpeedSlider", &component.movement_speed[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Rotation Speed");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::InputFloat3("##RotationSpeedSlider", &component.rotation_speed[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position Control");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::Button(
                component.enable_translation ? "ENABLED##Translation" : "DISABLED##Translation",
                ImVec2(-FLT_MIN, 0.0f))) {
                component.enable_translation = !component.enable_translation;
                updated                      = true;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Orientation Control");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::Button(
                component.enable_rotation ? "ENABLED##Orientation" : "DISABLED##Orientation",
                ImVec2(-FLT_MIN, 0.0f))) {
                component.enable_rotation = !component.enable_rotation;
                updated                   = true;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Relative Movement");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::Button(
                component.use_relative_movement ? "ENABLED##RelativeMode" : "DISABLED##RelativeMode",
                ImVec2(-FLT_MIN, 0.0f))) {
                component.use_relative_movement = !component.use_relative_movement;
                updated                         = true;
            }

            ImGui::EndTable();
        }

        ImGui::Separator();
    }
    if (removed) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Controllable>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::Controllable>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
}

void PropertiesPanel::draw_point_light_component_editor(const scene::Node* node)
{
    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool editor_opened = ImGui::TreeNodeEx("Point Light Component", flags);
    bool updated       = false;
    bool removed       = false;

    if (ImGui::BeginPopupContextItem("Point Light Component Context Popup")) {
        if (ImGui::MenuItem("Remove ##PointLightComponent")) {
            m_context->get_ecs_manager()->remove_component<component::PointLight>(node->get_entity());
            removed       = true;
            editor_opened = false;
        }
        if (ImGui::MenuItem("Reset ##PointLight")) {
            auto& component = m_context->get_ecs_manager()->get_component<component::PointLight>(node->get_entity());
            component = component::PointLight{};
            updated   = true;
        }
        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip("right click for options");

    if (editor_opened) {
        auto& component = m_context->get_ecs_manager()->get_component<component::PointLight>(node->get_entity());

        if (ImGui::BeginTable(
            "Editor##PointLightComponent",
            3,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Color");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::ColorEdit3("PointLight##BaseColorFactorSlider", &component.color[0]);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Intensity");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::SliderFloat("##PointLightIntensity", &component.intensity, 0.01f, 100.0f, "%.2f");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Range");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::SliderFloat("##PointLightRange", &component.range, 0.1f, 100.0f, "%.0f", ImGuiSliderFlags_Logarithmic);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Cutoff");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            updated |= ImGui::SliderFloat("PointLight##Cutoff", &component.cutoff, 0.1f, 30.0f, "%.0f", ImGuiSliderFlags_Logarithmic);

            ImGui::EndTable();
        }

        ImGui::Separator();
    }

    if (removed) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::PointLight>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
    }
    if (updated) {
        core::event::Event event(core::event::component::MODIFIED);
        event.set_param(
            core::event::component::TYPE,
            m_context->get_ecs_manager()->get_component_type<component::PointLight>());
        event.set_param(core::event::component::ENTITY_ID, node->get_entity());
        core::EventHandler::get_instance().send_event(event);
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

        ImGui::Text("Vertex Count: %zu", mesh->get_vertex_count());
        ImGui::Text("Index Count: %zu", mesh->get_index_count());

        draw_asset_selector(asset::AssetType::Material, mesh->m_material, "Material");
        ImGui::PopFont();
    }
    ImGui::PopFont();
}

void PropertiesPanel::draw_material_asset_editor(asset::Material* material)
{
    ImGui::PushFont(m_manager->m_header_font);
    auto material_type = material->get_material_type();

    if (ImGui::CollapsingHeader("Material Details", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushFont(m_manager->m_body_font);
        if (material_type == asset::MaterialType::Phong) {
            auto* phong_material = dynamic_cast<asset::PhongMaterial*>(material);
            CGX_ASSERT(phong_material, "invalid phong material ptr");
            ImGui::Text("Material Type: Phong");

            if (ImGui::BeginTable(
                "Editor##MaterialAsset",
                2,
                ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {

                ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Ambient Color");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit3("##AmbientColor", &phong_material->m_ambient_color[0]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Diffuse Color");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit3("##DiffuseColor", &phong_material->m_diffuse_color[0]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Specular Color");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit3("##SpecularColor", &phong_material->m_specular_color[0]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Shininess");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::SliderFloat("##Shininess", &phong_material->m_shininess, 0.0f, 128.0f);


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Ambient Map");
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    phong_material->m_ambient_map,
                    "##AmbientMap");
                ImGui::TableSetColumnIndex(1);
                if (phong_material->m_ambient_map) {
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::Image((ImTextureID) phong_material->m_ambient_map->get_id(), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Diffuse Map");
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    phong_material->m_diffuse_map,
                    "##DiffuseMap");
                ImGui::TableSetColumnIndex(1);
                if (phong_material->m_diffuse_map) {
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::Image((ImTextureID) phong_material->m_diffuse_map->get_id(), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Specular Map");
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    phong_material->m_specular_map,
                    "##SpecularMap");
                ImGui::TableSetColumnIndex(1);
                if (phong_material->m_specular_map) {
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::Image((ImTextureID) phong_material->m_specular_map->get_id(), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Normal Map");
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    phong_material->m_normal_map,
                    "##NormalMap");
                ImGui::TableSetColumnIndex(1);
                if (phong_material->m_normal_map) {
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::Image((ImTextureID) phong_material->m_normal_map->get_id(), ImVec2(64, 64));
                }
                ImGui::EndTable();
            }
        }
        if (material_type == asset::MaterialType::PBR) {
            auto* pbr_material = dynamic_cast<asset::PBRMaterial*>(material);
            CGX_ASSERT(pbr_material, "invalid pbr material ptr");
            ImGui::Text("Material Type: PBR");

            if (ImGui::BeginTable("PBRTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Base Colors");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit4("##BaseColorFactorSlider", &pbr_material->m_base_color_factor[0]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Metallic Factor");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::SliderFloat("##MetallicFactorSlider", &pbr_material->m_metallic_factor, 0.0f, 1.0f);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Roughness Factor");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::SliderFloat("##RoughnessFactorSlider", &pbr_material->m_roughness_factor, 0.0f, 1.0f);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Base Color Map");
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    pbr_material->m_base_color_map,
                    "##BaseColorMap");
                ImGui::TableSetColumnIndex(1);
                if (pbr_material->m_base_color_map) {
                    pbr_material->m_base_color_map->bind(0);
                    ImGui::Image(reinterpret_cast<ImTextureID>(pbr_material->m_base_color_map->get_texture_id()), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Metallic Roughness Map");
                ImGui::TableSetColumnIndex(1);
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    pbr_material->m_metallic_roughness_map,
                    "##MetallicRoughnessMap");
                if (pbr_material->m_metallic_roughness_map) {
                    pbr_material->m_metallic_roughness_map->bind(0);
                    ImGui::Image(
                        reinterpret_cast<ImTextureID>(pbr_material->m_metallic_roughness_map->get_texture_id()),
                        ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Normal Map");
                ImGui::TableSetColumnIndex(1);
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    pbr_material->m_normal_map,
                    "##NormalMap");
                if (pbr_material->m_normal_map) {
                    pbr_material->m_normal_map->bind(0);
                    ImGui::Image((void*) (intptr_t) pbr_material->m_normal_map->get_texture_id(), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Occlusion Map");
                ImGui::TableSetColumnIndex(1);
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    pbr_material->m_occlusion_map,
                    "##OcclusionMap");
                if (pbr_material->m_occlusion_map) {
                    pbr_material->m_occlusion_map->bind(0);
                    ImGui::Image((void*) (intptr_t) pbr_material->m_occlusion_map->get_texture_id(), ImVec2(64, 64));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Emissive Map");
                ImGui::TableSetColumnIndex(1);
                draw_asset_selector<asset::Texture>(
                    asset::AssetType::Texture,
                    pbr_material->m_emissive_map,
                    "##EmissiveMap");
                if (pbr_material->m_emissive_map) {
                    pbr_material->m_emissive_map->bind(0);
                    ImGui::Image((void*) (intptr_t) pbr_material->m_emissive_map->get_texture_id(), ImVec2(64, 64));
                }
                ImGui::EndTable();
            }
        }
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
        if (ImGui::BeginTable(
            "Editor##ControllableComponent",
            2,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##Slider", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Texture ID");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%u", texture->m_texture_id);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Dimensions");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%u, %u", texture->m_width, texture->m_height);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("# Channels");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%u", texture->m_num_channels);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("# Channels");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%u", texture->m_format);

            ImGui::EndMenu();
        }
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
