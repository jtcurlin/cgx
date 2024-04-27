// Copyright © 2024 Jacob Curlin

#include "gui/panels/scene_panel.h"

#include "gui/imgui_manager.h"
#include "scene/node.h"
#include "scene/scene_manager.h"

#include <filesystem>
#include <iomanip>

#include "scene/camera_node.h"

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

    auto roots = m_context->get_scene_manager()->get_active_scene()->get_roots();
    for (auto* root_node : roots) {
        draw_node(root_node);
    }

    draw_scene_import_popup();
    if (m_adding_scene) {
        draw_add_scene_popup();
    }
}

void ScenePanel::render_scene_menu_bar()
{
    if (ImGui::BeginMenu("Scene")) {

        if (ImGui::BeginMenu("Select Active Scene")) {
            for (const auto& scene_pair : m_context->get_scene_manager()->get_scenes()) {
                std::string scene_label = scene_pair.first + "##SelectActiveSceneList";
                if (ImGui::MenuItem(scene_label.c_str())) {
                    m_context->get_scene_manager()->set_active_scene(scene_label);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Remove Scene")) {
            for (const auto& scene_pair : m_context->get_scene_manager()->get_scenes()) {
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
        if (ImGui::MenuItem("\uf6cf  Mesh Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node(
                scene::NodeType::Type::Mesh,
                "New Mesh",
                nullptr);
            m_context->set_item_to_inspect(new_node);
        }
        if (ImGui::MenuItem("\uf03d   Camera Node")) {
            auto* new_node = m_context->get_scene_manager()->add_node(
                scene::NodeType::Type::Camera,
                "New Camera",
                nullptr);
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
    if (!node) return;
    ImGui::PushID(node);
    auto& node_state = m_node_states[node->get_id()];

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;
    node_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    node_flags |= node_state.is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;
    node_flags |= node == m_context->get_item_to_inspect() ? ImGuiTreeNodeFlags_Selected : 0;

    std::string icon;
    switch (node->get_node_type()) {
        case scene::NodeType::Type::Mesh: {
            icon = "\uf6cf  " + node->get_tag();
            break;
        }
        case scene::NodeType::Type::Camera: {
            icon = "\uf03d  " + node->get_tag();
            break;
        }
        default: {
            CGX_FATAL("Unrecognized node type");
            std::exit(1);
        }
    }


    const bool node_opened = ImGui::TreeNodeEx(icon.c_str(), node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        node_state.is_expanded = !node_state.is_expanded;
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
    if (ImGui::BeginPopupContextItem("NodeContextMenu")) {
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
            node->remove();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScenePanel::draw_scene_import_popup()
{
    if (m_importing_scene) {
        ImGui::OpenPopup("Add Scene ##ImportSceneMenu");
    }
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Open a modal popup to block interactions with other UI elements
    ImGui::PushFont(m_manager->m_title_font);
    if (ImGui::BeginPopupModal("Add Scene ##ImportSceneMenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        bool success = false;
        bool exited  = false;

        ImGui::PushFont(m_manager->m_body_font);

        ImGui::TextUnformatted("Enter .gltf/.glb path");
        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##ImportScenePathInput", m_input_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {

            const std::filesystem::path base_data_path(std::string(DATA_DIRECTORY));
            const std::filesystem::path full_path = base_data_path / m_input_buffer;
            const std::string           extension = full_path.extension().string();

            if (!std::filesystem::exists(full_path)) {
                m_error_message = "Specified path does not exist.";
                m_error_active  = true;
            }
            else if (extension != ".gltf" && extension != ".glb") {
                m_error_message = "Specified file is not of type .gltf or .glb";
                m_error_active  = true;
            }
            else {
                const auto* scene_manager = m_context->get_scene_manager();
                scene_manager->import_scene(full_path.string(), m_node_to_birth);
                success = true;
            }
        }
        ImGui::SetItemTooltip("Press enter to import path, escape to cancel.");

        if (m_error_active) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextUnformatted(m_error_message.c_str());
            ImGui::PopStyleColor();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            exited = true;
        }

        if (success || exited) {
            m_node_to_birth   = nullptr;
            m_importing_scene = false;
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

void ScenePanel::draw_add_scene_popup()
{
    bool success = false;
    bool exited  = false;

    ImGui::OpenPopup("Add Scene ##AddSceneMenu");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushFont(m_manager->m_title_font);
    if (ImGui::BeginPopupModal("Add Scene ##AddSceneMenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(m_manager->m_body_font);

        ImGui::TextUnformatted("Enter Label");
        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##AddScene-LabelInput", m_input_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {

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
        ImGui::SetItemTooltip("Press enter to add scene, escape to cancel.");

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
