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
    : ImGuiPanel("Scene", context, manager, ImGuiWindowFlags_MenuBar) {}

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

    if (m_importing_scene) {
        std::string selected_file = "none";
        if (DialogPanel::draw_file_import_dialog("Select Scene File", ".glb,.gltf", selected_file)) {
            if (selected_file != "none") {
                m_context->get_scene_manager()->import_scene(selected_file, m_node_to_birth);
                m_node_to_birth = nullptr;
            }
            m_importing_scene = false;
        }
    }

    if (m_adding_scene) {
        draw_add_scene_popup();
    }
}

void ScenePanel::render_scene_menu_bar()
{
    if (ImGui::BeginMenu("Scene")) {
        auto scene_manager = m_context->get_scene_manager();

        if (ImGui::BeginMenu("Select Active Scene")) {
            for (const auto& scene_pair : scene_manager->get_scenes()) {
                std::string scene_label = scene_pair.first + "##SelectActiveSceneList";
                if (ImGui::MenuItem(scene_label.c_str())) {
                    m_context->get_scene_manager()->set_active_scene(scene_label);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Remove Scene")) {
            for (const auto& scene_pair : scene_manager->get_scenes()) {
                std::string scene_label = scene_pair.first + "##RemoveSceneList";
                if (ImGui::MenuItem(scene_label.c_str())) {
                    // todo: remove selected scene (scene_pair.second)
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Import Scene")) {
            if (ImGui::MenuItem("As New Scene")) {
                // todo
            }
            if (ImGui::MenuItem("Into Active Scene")) {
                m_importing_scene = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Add New Scene")) {
            m_adding_scene = true;
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Add Node")) {
        const auto scene_manager = m_context->get_scene_manager();
        const auto root_node     = scene_manager->get_active_scene()->get_root();

        if (ImGui::MenuItem("\uf6cf  Mesh Node")) {
            auto* new_node = scene_manager->add_node(scene::NodeType::Type::Mesh, "New Mesh", root_node);
            m_context->set_item_to_inspect(new_node);
        }
        if (ImGui::MenuItem("\uf03d   Camera Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node(
                scene::NodeType::Type::Camera,
                "New Camera",
                root_node);
            m_context->set_item_to_inspect(new_node);
        }
        /*
        if (ImGui::MenuItem("\uf4a1   Light Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node(
                scene::NodeType::Type::Light,
                scene::Node::get_default_tag(),
                nullptr);
            m_context->set_item_to_inspect(new_node);
        }
        */
        ImGui::EndMenu();
    }

}

void ScenePanel::draw_node(scene::Node* node)
{
    CGX_VERIFY(node);
    CGX_VERIFY(node->get_node_type() != scene::NodeType::Type::Root);

    ImGui::PushID(node);
    auto& node_state = m_node_states[node->get_id()];

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;
    node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap;
    node_flags |= node_state.is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;
    node_flags |= node == m_context->get_item_to_inspect() ? ImGuiTreeNodeFlags_Selected : 0;

    bool node_opened = false;
    switch (node->get_node_type()) {
        case scene::NodeType::Type::Mesh: {
            const std::string icon = "\uf6cf  " + node->get_tag();
            node_opened            = ImGui::TreeNodeEx(icon.c_str(), node_flags);
            break;
        }
        case scene::NodeType::Type::Camera: {
            const std::string icon = "\uf03d  " + node->get_tag();
            node_opened            = ImGui::TreeNodeEx(icon.c_str(), node_flags);
            break;
        }
        case scene::NodeType::Type::Root: {
            CGX_FATAL("attempting to render root");
        }
        default: {
            CGX_FATAL("Unrecognized node type");
        }
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) ) {
        m_context->set_item_to_inspect(node);
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            node_state.is_expanded = !node_state.is_expanded;
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
                    scene::NodeType::Type::Mesh,
                    "New Mesh",
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }
            if (ImGui::MenuItem("\uf03d   Camera Node")) {
                auto* new_node = m_context->get_scene_manager()->add_node(
                    scene::NodeType::Type::Camera,
                    "New Camera",
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }
            /*
            if (ImGui::MenuItem("\uf4a1   Light Node")) {
                auto* new_node = m_context->get_scene_manager()->add_node(
                    scene::NodeType::Type::Light,
                    scene::Node::get_default_tag(),
                    node);
                m_context->set_item_to_inspect(new_node);
                node_added = true;
            }
            */
            if (node_added) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Rename")) {
            m_context->set_item_to_rename(node);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Import Child")) {
            m_node_to_birth   = node;
            m_importing_scene = true;
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
