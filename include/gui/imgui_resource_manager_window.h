// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_window.h"
#include "render/resource_manager.h"

#include <string>
#include <vector>

namespace cgx::gui
{
    enum class ResourceType
    {
        model,
        material,
        texture,
        shader
    };



    class ImGuiResourceManagerWindow : public ImGuiWindow
    {
    public:
        ImGuiResourceManagerWindow(
            std::shared_ptr<cgx::render::ResourceManager> resource_manager
        );

        void Render() override;

        void RenderModelList();
        void RenderMaterialList();
        void RenderTextureList();
        void RenderShaderList();

        void RenderModelEditor();
        void RendeMaterialEditor();
        void RenderTextureEditor();
        void RenderShaderEditor();

    private:
        std::shared_ptr<cgx::render::ResourceManager> m_resource_manager;

        std::vector<std::string> m_models;
        std::vector<std::string> m_materials;
        std::vector<std::string> m_textures;
        std::vector<std::string> m_shaders;

        std::string  m_current_resource;
        ResourceType m_current_resource_type;


    }; // class ImGuiResourceManagerWindow

} // namespace cgx::gui