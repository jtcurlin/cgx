// Copyright © 2024 Jacob Curlin

#include "gui/panels/scene_panel.h"

#include "gui/imgui_manager.h"
#include "scene/node.h"
#include "scene/scene_manager.h"

namespace cgx::gui
{
ScenePanel::ScenePanel(GUIContext* context, ImGuiManager* manager)
    : ImGuiPanel("Scene", context, manager) {}

ScenePanel::~ScenePanel() = default;

void ScenePanel::render()
{
    ImGui::SetWindowFontScale(1.0f);
    if (ImGui::Button("\uf0fe  Add Root Node")) {
        m_adding_node = true;
    }
    ImGui::SetWindowFontScale(1.0f);

    auto roots = m_context->get_scene_manager()->get_active_scene()->get_roots();
    for (auto* root_node : roots) {
        draw_node(root_node);
    }

    draw_new_node_menu();
}

void ScenePanel::draw_node(scene::Node* node)
{
    if (!node) return;
    ImGui::PushID(node);
    auto& node_state = m_node_states[node->get_id()];

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;
    node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    node_flags |= node_state.is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;

    /*
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
    */
    const std::string icon = "\uf6cf " + node->get_tag();

    const bool node_opened = ImGui::TreeNodeEx(icon.c_str(), node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        node_state.is_expanded = !node_state.is_expanded;
    }

    draw_node_context_menu(node);

    if (node_opened) {
        for (const auto& child : node->get_children()) {
            auto casted_child = dynamic_cast<scene::Node*>(child.get());
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
    if (ImGui::BeginPopupContextItem("NodeContextMenu")) {

        if (ImGui::MenuItem("Inspect")) {
            m_context->set_item_to_inspect(node);
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("Rename")) {
            m_context->set_item_to_rename(node);
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("Add Child")) {
            m_node_to_birth = node;
            m_adding_node   = true;
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("Remove")) {
            node->remove();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScenePanel::draw_new_node_menu()
{
    if (m_adding_node) {
        ImGui::OpenPopup("Add Node ##NewNodeMenu");
    }
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Open a modal popup to block interactions with other UI elements
    ImGui::PushFont(m_manager->m_title_font);
    if (ImGui::BeginPopupModal("Add Node ##NewNodeMenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display buttons for each type of node
        ImGui::PushFont(m_manager->m_header_font);
        if (ImGui::MenuItem("\uf6cf   Entity Node")) {
            scene::Node* new_node = nullptr;
            new_node = m_context->get_scene_manager()->add_node(m_node_to_birth, generate_default_node_tag());
            on_node_added(new_node);
            m_node_to_birth = nullptr;
            m_adding_node   = false;
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("\uf03d   Camera Node")) {
            scene::Node* new_node = nullptr;
            new_node = m_context->get_scene_manager()->add_node(m_node_to_birth, generate_default_node_tag());
            on_node_added(new_node);
            m_node_to_birth = nullptr;
            m_adding_node   = false;
            ImGui::CloseCurrentPopup();
        }
        else if (ImGui::MenuItem("\uf4a1   Light Node")) {
            scene::Node* new_node = nullptr;
            new_node = m_context->get_scene_manager()->add_node(m_node_to_birth, generate_default_node_tag());
            on_node_added(new_node);
            m_node_to_birth = nullptr;
            m_adding_node   = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cancel")) {
            m_node_to_birth = nullptr;
            m_adding_node   = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopFont();

        ImGui::EndPopup();
    }

    ImGui::PopFont();
}

void ScenePanel::on_node_added(scene::Node* node)
{
    m_context->set_item_to_inspect(node);
    m_node_states[node->get_id()] = NodeState{};
}

std::string ScenePanel::generate_default_node_tag()
{
    static size_t s_node_counter = 0;
    CGX_ASSERT(s_node_counter <= 999, "exceeded default node tag count");

    std::stringstream name_ss;
    name_ss << "node_" << std::setw(3) << std::setfill('0') << std::to_string(s_node_counter++);
    return name_ss.str();
}

}
