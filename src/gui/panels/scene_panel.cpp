// Copyright © 2024 Jacob Curlin

#include "gui/panels/scene_panel.h"

#include "gui/imgui_manager.h"
#include "gui/panels/dialog_panel.h"
#include "scene/node.h"
#include "scene/scene_manager.h"

#include <filesystem>
#include <imguifiledialog/ImGuiFileDialog.h>


namespace cgx::gui
{
ScenePanel::ScenePanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Node Hierarchy", context, manager, ImGuiWindowFlags_MenuBar) {}

ScenePanel::~ScenePanel() = default;

void ScenePanel::render()
{
    if (ImGui::BeginMenuBar()) {
        render_scene_menu_bar();
        ImGui::EndMenuBar();
    }

    ImDrawList* draw_list       = ImGui::GetWindowDrawList();
    ImVec2      window_pos      = ImGui::GetWindowPos();
    ImVec2      window_size     = ImGui::GetWindowSize();
    ImVec2      separator_start = ImVec2(window_pos.x, window_pos.y + ImGui::GetFrameHeightWithSpacing());
    ImVec2      separator_end   = ImVec2(window_pos.x + window_size.x, separator_start.y);
    draw_list->AddLine(separator_start, separator_end, ImGui::GetColorU32(ImGuiCol_Separator));

    auto root_node = m_context->get_scene_manager()->get_active_scene()->get_root();
    for (auto& node : root_node->get_children()) {
        draw_node(dynamic_cast<scene::Node*>(node.get()));
    }
}

void ScenePanel::render_scene_menu_bar()
{
    if (ImGui::BeginMenu("Add Node")) {
        const auto scene_manager = m_context->get_scene_manager();
        const auto root_node     = scene_manager->get_active_scene()->get_root();

        if (ImGui::MenuItem("\uf6cf  Mesh Node")) {
            auto* new_node = scene_manager->add_node("New Mesh", scene::NodeFlag::Mesh);
            m_context->set_item_to_inspect(new_node);
        }
        if (ImGui::MenuItem("\uf4a1   Light Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node("New Light", scene::NodeFlag::Light);
            m_context->set_item_to_inspect(new_node);
        }
        if (ImGui::MenuItem("\uf03d   Camera Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node("New Camera", scene::NodeFlag::Camera);
            m_context->set_item_to_inspect(new_node);
        }
        ImGui::EndMenu();
    }
}

void ScenePanel::draw_node(scene::Node* node)
{
    CGX_VERIFY(node);

    ImGui::PushID(node);

    std::string node_label = "Empty";
    if (node->is_mesh()) {
        node_label = "\uf6cf  " + node->get_tag();
    }
    if (node->is_light()) {
        node_label = "\uf4a1  " + node->get_tag();
    }
    if (node->is_camera()) {
        node_label = "\uf03d  " + node->get_tag();
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                                    ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap;
    flags |= m_node_states[node->get_id()].is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;
    flags |= node == m_context->get_item_to_inspect() ? ImGuiTreeNodeFlags_Selected : 0;

    const bool node_opened = ImGui::TreeNodeEx(node_label.c_str(), flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        m_context->set_item_to_inspect(node);
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            m_node_states[node->get_id()].is_expanded = !m_node_states[node->get_id()].is_expanded;
        }
    }

    draw_node_context_menu(node);

    if (node_opened) {
        for (const auto& child : node->get_children()) {
            const auto casted_child = dynamic_cast<scene::Node*>(child.get());
            CGX_ASSERT(casted_child, "attempt to draw non-node hierarchy element");
            draw_node(casted_child);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ScenePanel::draw_node_context_menu(scene::Node* node)
{
    CGX_ASSERT(node, "attempt to draw context menu for invalid node");

    const std::string unique_id = "NodeContextMenu##" + std::to_string(node->get_id());
    if (ImGui::BeginPopupContextItem(unique_id.c_str())) {
        if (ImGui::MenuItem("Inspect")) {
            m_context->set_item_to_inspect(node);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::BeginMenu("Add Child")) {
            bool node_added = false;
            if (ImGui::MenuItem("\uf6cf  Mesh Node")) {
                auto* new_node = m_context->get_scene_manager()->add_node(
                    "New Mesh",
                    scene::NodeFlag::Mesh,
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }
            if (ImGui::MenuItem("\uf4a1   Light Node")) {
                auto* new_node = m_context->get_scene_manager()->add_node(
                    "New Light",
                    scene::NodeFlag::Light,
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }
            if (ImGui::MenuItem("\uf03d   Camera Node")) {
                auto* new_node = m_context->get_scene_manager()->add_node(
                    "New Camera",
                    scene::NodeFlag::Camera,
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }

            if (node_added) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Rename")) {
            m_context->set_item_to_rename(node);
            m_context->m_renaming_item = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Import Child")) {
            m_context->set_node_to_birth(node);
            m_context->m_importing_node = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Remove")) {
            if (m_context->get_item_to_inspect() == node) {
                m_context->set_item_to_inspect(nullptr);
            }
            m_context->get_scene_manager()->remove_node(node);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


void ScenePanel::draw_add_scene_popup()
{
    bool success   = false;
    bool exited    = false;
    bool attempted = false;

    ImGui::OpenPopup("Add Scene ##AddSceneMenu");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushFont(m_manager->m_title_font);
    if (ImGui::BeginPopupModal("Add Scene ##AddSceneMenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(m_manager->m_body_font);

        ImGui::TextUnformatted("Enter Label");
        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##AddScene-LabelInput", m_input_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
            attempted = true;
        }
        ImGui::SetItemTooltip("Press enter to add scene, escape to cancel.");

        if (attempted) {
            const auto& scenes = m_context->get_scene_manager()->get_scenes();
            if (std::strlen(m_input_buffer) == 0) {
                m_error_message = "Please specify a valid scene label.";
                m_error_active  = true;
            }
            else if (scenes.find(std::string(m_input_buffer)) != scenes.end()) {
                m_error_message = "Specified scene label is already in use.";
                m_error_active  = true;
            }
            else {
                auto*      scene_manager = m_context->get_scene_manager();
                const auto scene_label   = std::string(m_input_buffer);
                scene_manager->add_scene(scene_label);
                scene_manager->set_active_scene(scene_label);
                success = true;
            }
        }

        if (m_error_active) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextUnformatted(m_error_message.c_str());
            ImGui::PopStyleColor();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            exited = true;
        }

        if (success || exited) {
            m_adding_scene    = false;
            m_error_active    = false;
            m_input_buffer[0] = '\0';
            m_error_message   = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopFont();
        ImGui::EndPopup();
    }
    ImGui::PopFont();
}
}
