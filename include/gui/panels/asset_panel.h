// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_panel.h"

#include "asset/asset.h"

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
class AssetPanel final : public ImGuiPanel
{
public:
    explicit AssetPanel(const std::shared_ptr<GUIContext>& context, const std::shared_ptr<ImGuiManager>& manager);
    ~AssetPanel() override;

    void render() override;

    void render_importers_list();

    void render_asset_list(asset::AssetType type);

private:
    asset::AssetID m_current_asset_id{asset::k_invalid_id};
    std::string    m_current_importer_label{};
};
}