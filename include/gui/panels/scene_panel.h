// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "scene/node.h"

namespace cgx::gui
{
struct NodeState
{
    bool is_expanded{true};
};

class ScenePanel final : public ImGuiPanel
{
public:
    ScenePanel(GUIContext* context, ImGuiManager* manager);
    ~ScenePanel() override;

    void render() override;

    void draw_node(scene::Node* node);
    void draw_node_context_menu(scene::Node* node);
    void draw_new_node_menu();
    void draw_scene_import_popup();

private:
    std::unordered_map<size_t, NodeState> m_node_states;

    size_t       m_current_node_id{scene::k_invalid_id};
    scene::Node* m_node_to_birth{nullptr};
    bool         m_adding_node{false};

    char m_import_path_buffer[256]{};
    bool m_importing_scene{false};

    void               on_node_added(scene::Node* node);
    static std::string generate_default_node_tag();
};
}
