// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_window.h"
#include "gui/resource_manager_adapter.h"

#include "resource/resource.h"
#include "resource/resource_manager.h"

#include <string>
#include <vector>

namespace cgx::gui
{

    struct ResourceEntry
    {
        cgx::resource::RUID ruid;
        std::string tag;
    };

    class ImGuiResourceManagerWindow : public ImGuiWindow
    {
    public:
        ImGuiResourceManagerWindow(
            const std::string& data_dir_path,
            std::shared_ptr<ResourceManagerAdapter> resource_manager_adapter
        );

        void Render() override;

        void RenderModelList();
        void RenderMaterialList();
        void RenderTextureList();
        void RenderShaderList();

        void RenderModelEditor();
        void RenderMaterialEditor();
        void RenderTextureEditor();
        void RenderShaderEditor();

        void RenderImportButton(const std::vector<std::string>& extensions, 
                                std::function<void(const std::string&)> file_selected_cb);
        void PopulateFileList(const std::vector<std::string>& extensions);

    private:

        std::shared_ptr<ResourceManagerAdapter> m_resource_manager_adapter;
        std::string m_data_dir_path;

        cgx::resource::RUID m_current_resource = cgx::resource::k_invalid_id;

        bool                        m_display_file_list;
        std::string                 m_current_file;
        std::vector<std::string>    m_file_list;


    }; // class ImGuiResourceManagerWindow

} // namespace cgx::gui