// Copyright © 2024 Jacob Curlin

#include "gui/panels/scene_panel.h"
#include "scene/node.h"

namespace cgx::gui
{

size_t ScenePanel::s_node_counter = 0;

ScenePanel::ScenePanel(const std::shared_ptr<GUIContext>& context)
    : ImGuiPanel("Scene", context)
{
    m_root = m_context->get_scene_manager()->get_active_scene()->get_root();
}

ScenePanel::~ScenePanel() = default;

void ScenePanel::render()
{
    if (!m_root) return;


    ImGui::SetWindowFontScale(1.0f);
    if (ImGui::Button("\uf0fe  Add Root Node")) {
        m_new_node_parent = m_root.get();
        m_adding_node = true;
    }
    ImGui::SetWindowFontScale(1.0f);

    for (const auto& child : m_root->get_children()) {
        draw_node(std::dynamic_pointer_cast<scene::Node>(child));
    }

    draw_new_node_menu();
}

void ScenePanel::draw_node(const std::shared_ptr<scene::Node>& node)
{
    if (!node) return;
    ImGui::PushID(node.get());
    // const auto id    = node->get_id();
    auto& state = m_node_states[node->get_id()];

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;
    node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    node_flags |= state.is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;

    const char* icon;
    switch (node->get_node_type()) {
        case scene::NodeType::Camera: icon = "\uf03d  Camera Node";
            break;
        case scene::NodeType::Entity: icon = "\uf6cf  Entity Node";
            break;
        case scene::NodeType::Light: icon = "\uf4a1  Light Node";
            break;
        default: icon = "\ue47b Unknown Node Type";
    }

    bool node_opened = ImGui::TreeNodeEx(icon, node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        state.is_expanded = !state.is_expanded;
    }

    draw_node_context_menu(node.get());


    if (node_opened) {
        for (const auto& child : node->get_children()) {
            draw_node(std::dynamic_pointer_cast<scene::Node>(child));
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void ScenePanel::add_node(const scene::NodeType node_type, scene::Node* parent) const
{
    std::stringstream name_ss;
    name_ss << "Node " << std::setw(3) << std::setfill('0') << std::to_string(s_node_counter++);

    m_context->get_scene_manager()->get_active_scene()->add_node(name_ss.str(), node_type, parent);
}

void ScenePanel::draw_node_context_menu(scene::Node* node)
{
    if (ImGui::BeginPopupContextItem("NodeContextMenu")) {
        if (ImGui::MenuItem("Inspect")) {
            m_context->set_selected_item(node);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Add Child")) {
            m_new_node_parent = node;
            m_adding_node = true;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Remove")) {
            node->remove();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScenePanel::draw_new_node_menu()
{
    if (m_adding_node) {
        ImGui::OpenPopup("Add New Node");
    }
    // Define the size and position of the popup
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Open a modal popup to block interactions with other UI elements
    if (ImGui::BeginPopupModal("Add New Node", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display buttons for each type of node
        if (ImGui::MenuItem("\uf6cf  Entity Node")) {
            add_node(scene::NodeType::Entity, m_new_node_parent);
            m_adding_node = false;
            m_new_node_parent = nullptr;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("\uf03d  Camera Node")) {
            add_node(scene::NodeType::Camera, m_new_node_parent);
            m_adding_node = false;
            m_new_node_parent = nullptr;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("\uf4a1  Light Node")) {
            add_node(scene::NodeType::Light, m_new_node_parent);
            m_adding_node = false;
            m_new_node_parent = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}


}
