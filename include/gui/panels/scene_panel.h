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

    void render_scene_menu_bar();

    void draw_node(scene::Node* node);
    void draw_node_context_menu(scene::Node* node);

    void draw_add_scene_popup();

private:
    std::unordered_map<size_t, NodeState> m_node_states;

    bool m_adding_scene{false};
    bool m_importing_scene{false};
    bool m_error_active{false};

    scene::Node* m_node_to_birth{nullptr};
    size_t       m_current_node_id{scene::k_invalid_id};

    std::string m_error_message{};
    char        m_input_buffer[256]{};

};
}
