// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"

namespace cgx::scene
{
class Node;
class EntityNode;
}

namespace cgx::asset
{
class Model;
class Mesh;
class Material;
class Texture;
class Shader;
}

namespace cgx::gui
{
class PropertiesPanel final : public ImGuiPanel
{
public:
    explicit PropertiesPanel (const std::shared_ptr<GUIContext>& context);
    ~PropertiesPanel () override;

    void render () override;

    void draw_asset_properties (asset::Asset* asset);
    void draw_node_properties (scene::Node* node);

    void draw_node_metadata (const scene::Node* node);
    void draw_asset_metadata (const asset::Asset* asset);

    void draw_entity_node_properties (scene::EntityNode* entity_node);

    void draw_render_component_editor (scene::EntityNode* node);
    void draw_transform_component_editor (scene::EntityNode* node);
    void draw_rigidbody_component_editor (scene::EntityNode* node);

    void draw_model_asset_editor(asset::Model* model);
    void draw_mesh_asset_editor(asset::Mesh* mesh);
    void draw_material_asset_editor(asset::Material* material);
    void draw_texture_asset_editor(asset::Texture* texture);
    void draw_shader_asset_editor(asset::Shader* shader);

};
}
