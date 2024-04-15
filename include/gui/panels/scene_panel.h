// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"
#include "scene/node.h"

namespace cgx::gui
{
struct NodeState
{
    bool is_expanded{true};
    bool is_renaming{false};
    char rename_buffer[256];
};

class ScenePanel final : public ImGuiPanel
{
public:
    explicit ScenePanel(const std::shared_ptr<GUIContext>& context);
    ~ScenePanel() override;

    void render() override;

    void draw_node(const std::shared_ptr<scene::Node>& node);
    void add_node(scene::NodeType node_type, scene::Node* parent = nullptr) const;


    void draw_node_context_menu(scene::Node* node);
    void draw_new_node_menu();

    static size_t s_node_counter;

private:
    std::shared_ptr<scene::Node>               m_root{};
    std::unordered_map<std::size_t, NodeState> m_node_states{};

    size_t m_current_node_id;

    bool m_adding_node{false};
    scene::Node* m_new_node_parent{};

    bool is_renaming;
};
}


// U+26BD - soccer ball
// U+F204 - toggle off
// U+F205 - toggle off
// U+1F4A1 - light bulb
// U+F085 - gears
// U+1F441 - eye (visibility)
// U+F070 - eye crossed out (visibility)
// U+1F527 - wrench
// U+F1F8 - trash can
// U+E47B -
// U+F6CF - dice (20 sided)
// U+F6D1 - dice (6 sided)
// U+F03D - video camera
// U+F03e - image
// u_2704