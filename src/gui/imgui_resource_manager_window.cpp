// Copyright © 2024 Jacob Curlin

#include "gui/imgui_resource_manager_window.h"

namespace cgx::gui
{
    ImGuiResourceManagerWindow::ImGuiResourceManagerWindow(
        std::shared_ptr<cgx::render::ResourceManager> resource_manager
    )
        : ImGuiWindow("Resource Management")
        , m_resource_manager(resource_manager)
    {

    }


    void ImGuiResourceManagerWindow::RenderModelList()
    {
        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Loaded Models");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", m_models.size());


        if (ImGui::BeginChild("LoadedModelsList##LoadedModelsList", ImVec2(0, 150), true))
        {
            for (const auto& model: m_models)
            {
                if (ImGui::Selectable(model.c_str(), (m_current_resource_type == ResourceType::model) && (m_current_resource == model)))
                {
                    m_current_resource_type = ResourceType::model;
                    m_current_resource = model;
                    CGX_DEBUG("> ResourceManagerMenu: Selected Model {}", m_current_resource);
                }

            }
            ImGui::EndChild();
        }
        
        float button_width = ImGui::GetContentRegionAvail().x * 0.45f;
        ImGui::NewLine();
        ImGui::SameLine((ImGui::GetWindowWidth() - 2 * button_width - ImGui::GetStyle().ItemSpacing.x) / 2);

        if (ImGui::Button("Load Model##ActiveModelsList", ImVec2(button_width, 0)))
        {
            cgx::render::Entity new_entity = m_ecs_manager->CreateEntity();
            // m_entities.insert(new_entity);
            m_current_entity = new_entity;
            CGX_INFO("> Created Entity {}", m_current_entity);
        }

        ImGui::SameLine();
        
        bool delete_button_active = (m_current_resource.length() == 0);
        if (delete_button_active) { ImGui::BeginDisabled(); }
        if (ImGui::Button("Remove Model##ActiveModelsList", ImVec2(button_width, 0)))
        {
            

            m_ecs_manager->DestroyEntity(m_current_entity);
            CGX_TRACE("> Destroyed Entity {}", m_current_entity);
            m_current_entity = cgx::ecs::MAX_ENTITIES;
        }
        if (delete_button_active) { ImGui::EndDisabled(); }


    }

    void ImGuiResourceManagerWindow::RenderMaterialList()
    {
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

    }
    
    void ImGuiResourceManagerWindow::RenderTextureList()
    {
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


    }

    void ImGuiResourceManagerWindow::RenderShaderList()
    {
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
    }
    
    void ImGuiResourceManagerWindow::RenderModelEditor()
    {

    }
    
    void ImGuiResourceManagerWindow::RenderMaterialEditor()
    {

    }
    
    void ImGuiResourceManagerWindow::RenderTextureEditor()
    {

    }
    
    void ImGuiResourceManagerWindow::RenderShaderEditor()
    {

    }

} // namespace cgx::gui