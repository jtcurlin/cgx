// Copyright © 2024 Jacob Curlin

#include "gui/imgui_resource_manager_window.h"
#include "event/events/engine_events.h"
#include "resource/model.h"

#include <filesystem>

namespace cgx::gui
{
    ImGuiResourceManagerWindow::ImGuiResourceManagerWindow(
        const std::string& data_dir_path,
        std::shared_ptr<ResourceManagerAdapter> resource_manager_adapter
    )
        : ImGuiWindow("Resource Management")
        , m_data_dir_path(data_dir_path)
        , m_resource_manager_adapter(resource_manager_adapter) 
    {}


    void ImGuiResourceManagerWindow::Render() 
    {
        RenderModelList();
    }

    void ImGuiResourceManagerWindow::RenderModelList()
    {
        auto& models = m_resource_manager_adapter->GetResourceEntries(cgx::resource::ResourceType::Model);

        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Loaded Models");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", models.size());

        if (ImGui::BeginChild("LoadedModelsList##LoadedModelsList", ImVec2(0, 150), true))
        {
            for (const auto& [ruid, tag]: models)
            {
                if (ImGui::Selectable(tag.c_str(), ruid == m_current_resource))
                {
                    m_current_resource = ruid;
                    CGX_DEBUG("> ResourceManagerMenu: Selected Model {}", tag);
                }

            }
            ImGui::EndChild();
        }
        
        float button_width = ImGui::GetContentRegionAvail().x * 0.45f;
        ImGui::NewLine();
        ImGui::SameLine((ImGui::GetWindowWidth() - 2 * button_width - ImGui::GetStyle().ItemSpacing.x) / 2);

        static std::vector<std::string> model_extensions {".obj"};
        RenderImportButton(model_extensions, [&](const std::string& file_path) {
            cgx::resource::ResourceManager::getSingleton().ImportResource<cgx::resource::Model>(file_path);
        }); 
        
        /*
        ImGui::SameLine();

        bool delete_button_active = (...);
        if (delete_button_active) { ImGui::BeginDisabled(); }
        if (ImGui::Button("Remove Model##ActiveModelsList", ImVec2(button_width, 0)))
        {
            //todo

            CGX_DEBUG("> Removed Model {}", ...);
            m_current_resource = cgx::resource::k_invalid_id;
        }
        if (delete_button_active) { ImGui::EndDisabled(); }
        */


    }

    void ImGuiResourceManagerWindow::RenderMaterialList()
    {
        /* todo
        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Loaded Materials");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", m_materials.size());

        if (ImGui::BeginChild("LoadedMaterialsList##LoadedMaterialsList", ImVec2(0, 150), true))
        {
            for (const auto& material: m_materials)
            {
                if (ImGui::Selectable(material.c_str(), (m_current_resource_type == ResourceType::material) && (m_current_resource == material)))
                {
                    m_current_resource_type = ResourceType::material;
                    m_current_resource = material;
                    CGX_DEBUG("> ResourceManagerMenu: Selected Material {}", m_current_resource);
                }

            }
            ImGui::EndChild();
        }
        */
    }
    
    void ImGuiResourceManagerWindow::RenderTextureList()
    {
        /* todo
        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Loaded Textures");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", m_textures.size());

        if (ImGui::BeginChild("LoadedTexturesList##LoadedTexturesList", ImVec2(0, 150), true))
        {
            for (const auto& texture: m_textures)
            {
                if (ImGui::Selectable(texture.c_str(), (m_current_resource_type == ResourceType::texture) && (m_current_resource == texture)))
                {
                    m_current_resource_type = ResourceType::texture;
                    m_current_resource = texture;
                    CGX_DEBUG("> ResourceManagerMenu: Selected Texture {}", m_current_resource);
                }

            }
            ImGui::EndChild();

        }
        */
    }

    void ImGuiResourceManagerWindow::RenderShaderList()
    {
        /* todo
        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Loaded Shaders");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", m_shaders.size());

        if (ImGui::BeginChild("LoadedShadersList##LoadedShadersList", ImVec2(0, 150), true))
        {
            for (const auto& shader: m_shaders)
            {
                if (ImGui::Selectable(shader.c_str(), (m_current_resource_type == ResourceType::shader) && (m_current_resource == shader)))
                {
                    m_current_resource_type = ResourceType::shader;
                    m_current_resource = shader;
                    CGX_DEBUG("> ResourceManagerMenu: Selected Shader {}", m_current_resource);
                }

            }
            ImGui::EndChild();
        }
        */
    }
    
    void ImGuiResourceManagerWindow::RenderModelEditor()
    {
        // todo
    }
    
    void ImGuiResourceManagerWindow::RenderMaterialEditor()
    {
        // todo
    }
    
    void ImGuiResourceManagerWindow::RenderTextureEditor()
    {
        // todo
    }
    
    void ImGuiResourceManagerWindow::RenderShaderEditor()
    {
        // todo
    }

    void ImGuiResourceManagerWindow::RenderImportButton(
        const std::vector<std::string>& extensions, 
        std::function<void(const std::string&)> file_selected_cb)
    {
        if (ImGui::Button("Import File"))
        {
            PopulateFileList(extensions);
            m_display_file_list = true;
        }

        if (m_display_file_list)
        {
            if (ImGui::BeginCombo("##files", m_current_file.empty() ? "Select File" : m_current_file.c_str()))
            {
                for (auto& file : m_file_list)
                {
                    bool is_selected = (m_current_file == file);
                    {
                        if (ImGui::Selectable(file.c_str(), is_selected))
                        {
                            m_current_file = file;
                            m_display_file_list = false;
                            file_selected_cb(std::filesystem::path(m_data_dir_path) / file);
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    void ImGuiResourceManagerWindow::PopulateFileList(const std::vector<std::string>& extensions)
    {
        m_file_list.clear();
        for (const auto& entry : std::filesystem::recursive_directory_iterator(m_data_dir_path))
        {
            if (entry.is_regular_file())
            {
                const auto& file_ext = entry.path().extension().string();
                if (std::find(extensions.begin(), extensions.end(), file_ext) != extensions.end())
                {
                    std::filesystem::path relative_path = std::filesystem::relative(entry.path(), m_data_dir_path);
                    m_file_list.push_back(relative_path.string());
                }
            }
        }
    }
    
} // namespace cgx::gui