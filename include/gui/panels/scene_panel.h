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
    explicit ScenePanel(const std::shared_ptr<GUIContext>& context, const std::shared_ptr<ImGuiManager>& manager);
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

    char m_input_buffer[256];
    scene::Node* m_node_being_renamed;

    size_t m_current_node_id;

    bool         m_adding_node{false};
    scene::Node* m_new_node_parent{};

    bool is_renaming;
};
}