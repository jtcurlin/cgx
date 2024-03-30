// Copyright © 2024 Jacob Curlin

#include "gui/imgui_ecs_window.h"

#include "core/common.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/render_component.h"
#include "ecs/components/light_component.h"
#include "ecs/components/rigid_body.h"
#include "ecs/events/engine_events.h"
#include "resource/resource.h"
#include "resource/resource_manager.h"
#include "utility/logging.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <string>
#include <vector>


namespace cgx::gui 
{
    ImGuiECSWindow::ImGuiECSWindow(
        std::shared_ptr<cgx::ecs::ECSManager> ecs_manager, 
        std::shared_ptr<ResourceManagerAdapter> resource_manager_adapter)
        : ImGuiWindow("ECS Management") 
        , m_ecs_manager(ecs_manager)
        , m_resource_manager_adapter(resource_manager_adapter)
        , m_current_entity(cgx::ecs::MAX_ENTITIES)
    {
        m_ecs_manager->AddEventListener(cgx::events::ecs::ENTITY_CREATED, [this](cgx::ecs::Event& event) {
            m_entities.push_back(event.GetParam<cgx::ecs::Entity>(cgx::events::ecs::ENTITY_ID));
        });

        m_ecs_manager->AddEventListener(cgx::events::ecs::ENTITY_DESTROYED, [this](cgx::ecs::Event& event) {
            auto entity = event.GetParam<cgx::ecs::Entity>(cgx::events::ecs::ENTITY_ID);
            m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
        });
    }

    void ImGuiECSWindow::Render()
    {
        // ImGui::Begin("ECS Management Panel");

        RenderActiveEntitiesSection();
         
        if (m_current_entity != cgx::ecs::MAX_ENTITIES)
        {
            ImGui::Separator();
            DisplayRenderComponentEditor(m_current_entity);
            DisplayTransformComponentEditor(m_current_entity);
            DisplayRigidBodyEditor(m_current_entity);
            DisplayLightComponentEditor(m_current_entity);
        }

        // ImGui::End();
    }

    void ImGuiECSWindow::RenderActiveEntitiesSection()
    {
        const char* count_field_text = "Count: ###";
        float count_field_width = ImGui::CalcTextSize(count_field_text).x + ImGui::GetStyle().FramePadding.x;

        ImGui::Text("Active Entities");

        ImGui::SameLine();

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - count_field_width);

        ImGui::Text("Count: %zu", m_entities.size());

        if (ImGui::BeginChild("ActiveEntityList##ActiveEntityList", ImVec2(0, 150), true))
        {
            for (const auto& entity : m_entities)
            {
                if (ImGui::Selectable(("Entity_" + std::to_string(entity)).c_str(), entity == m_current_entity))
                {
                    m_current_entity = entity;
                }
            }
            ImGui::EndChild();
        }

        float button_width = ImGui::GetContentRegionAvail().x * 0.45f;
        ImGui::NewLine();
        ImGui::SameLine((ImGui::GetWindowWidth() - 2 * button_width - ImGui::GetStyle().ItemSpacing.x) / 2);

        if (ImGui::Button("Create Entity##ActiveEntityList", ImVec2(button_width, 0)))
        {
            cgx::ecs::Entity new_entity = m_ecs_manager->CreateEntity();
            // m_entities.insert(new_entity);
            m_current_entity = new_entity;
        }

        ImGui::SameLine();
        
        bool delete_button_active = (m_current_entity == cgx::ecs::MAX_ENTITIES);
        if (delete_button_active) { ImGui::BeginDisabled(); }
        if (ImGui::Button("Destroy Entity##ActiveEntityList", ImVec2(button_width, 0)))
        {
            m_ecs_manager->DestroyEntity(m_current_entity);
            m_current_entity = cgx::ecs::MAX_ENTITIES;
        }
        if (delete_button_active) { ImGui::EndDisabled(); }
    }


    void ImGuiECSWindow::DisplayRenderComponentEditor(cgx::ecs::Entity entity)
    {
        bool has_render_component = m_ecs_manager->HasComponent<RenderComponent>(entity);

        const char* button_text = has_render_component ? "Remove" : "Add";
        float button_width = ImGui::CalcTextSize(button_text).x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::Text("Render Component");

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - button_width);

        ImGui::PushID(static_cast<int>(entity));

        if (has_render_component)
        {
            if (ImGui::Button("Remove##RemoveRenderComponent"))
            {
                m_ecs_manager->RemoveComponent<RenderComponent>(entity);
                has_render_component = false;
            }
        }
        else
        {
            if (ImGui::Button("Add##AddRenderComponent"))
            {
                m_ecs_manager->AddComponent(
                    entity,
                    RenderComponent {
                        .model = nullptr,
                        .shader = nullptr
                });
                has_render_component = true;
            }
        }

        if (has_render_component)
        {
            auto& render_component = m_ecs_manager->GetComponent<RenderComponent>(entity);

            if (ImGui::BeginCombo(
                "Model##RenderComponentModel", 
                render_component.model ? 
                render_component.model->getTag().c_str() : "No Model Selected"
            ))
            {
                auto model_entries = m_resource_manager_adapter->GetResourceEntries(cgx::resource::ResourceType::Model);
                for (const auto& entry : model_entries)
                {

                    bool is_selected = render_component.model != nullptr ? (render_component.model->getID() == entry.ruid) : false;
                    if (ImGui::Selectable(entry.tag.c_str(), is_selected))
                    {
                        render_component.model = cgx::resource::ResourceManager::getSingleton().getResource<cgx::resource::Model>(entry.ruid);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo(
                "Shader##RenderComponentShader", 
                render_component.shader ? 
                render_component.shader->getTag().c_str() : "No Shader Selected"
            ))
            {
                auto shader_entries = m_resource_manager_adapter->GetResourceEntries(cgx::resource::ResourceType::Shader);
                for (const auto& entry : shader_entries)
                {
                    bool is_selected = render_component.shader != nullptr ? (render_component.shader->getID() == entry.ruid) : false;
                    if (ImGui::Selectable(entry.tag.c_str(), is_selected))
                    {
                        render_component.shader = cgx::resource::ResourceManager::getSingleton().getResource<cgx::resource::Shader>(entry.ruid);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        ImGui::Separator();

        ImGui::PopID();
    }

    void ImGuiECSWindow::DisplayTransformComponentEditor(cgx::ecs::Entity entity)
    {
        bool has_transform_component = m_ecs_manager->HasComponent<TransformComponent>(entity);

        const char* button_text = has_transform_component ? "Remove" : "Add";
        float button_width = ImGui::CalcTextSize(button_text).x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::Text("Transform Component");

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - button_width);

        ImGui::PushID(static_cast<int>(entity));

        if (has_transform_component)
        {
            if (ImGui::Button("Remove##RemoveTransformComponent"))
            {
                m_ecs_manager->RemoveComponent<TransformComponent>(entity);
                has_transform_component = false;
            }
        }
        else
        {
            if (ImGui::Button("Add##AddTransformComponent"))
            {
                m_ecs_manager->AddComponent(
                    entity, 
                    TransformComponent {
                        .position = glm::vec3(0.0f, 0.0f, 0.0f),
                        .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                        .scale = glm::vec3(1.0f, 1.0f, 1.0f)
                    });
                has_transform_component = true;
            }
        }

        if (has_transform_component)
        {
            auto& component = m_ecs_manager->GetComponent<TransformComponent>(entity);
            
            ImGui::SliderFloat3("Position##TransformComponentPosition", &component.position[0], -25.0f, 25.0f); 
            ImGui::SliderFloat3("Rotation##TransformComponentRotation", &component.rotation[0], -180.0f, 180.0f);
            ImGui::SliderFloat3("Scale##TransformComponentScale", &component.scale[0], -10.0f, 10.0f); 
        }

        ImGui::Separator();

        ImGui::PopID();
        
    }

    void ImGuiECSWindow::DisplayRigidBodyEditor(cgx::ecs::Entity entity)
    {
        bool has_rigid_body = m_ecs_manager->HasComponent<RigidBody>(entity);  

        const char* button_text = has_rigid_body ? "Remove" : "Add";

        float button_width = ImGui::CalcTextSize(button_text).x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::Text("Rigid Body Component");

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - button_width);

        ImGui::PushID(static_cast<int>(entity));

        if (has_rigid_body)
        {
            if (ImGui::Button("Remove##RemoveRigidBody"))
            {
                m_ecs_manager->RemoveComponent<RigidBody>(entity);
                has_rigid_body = false;
            }
        }
        else
        {
            if (ImGui::Button("Add##AddRigidBodyComponent"))
            {
                m_ecs_manager->AddComponent(
                    entity,
                    RigidBody {
                        .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                        .acceleration = glm::vec3(0.0f, 0.0f, 0.0f),
                });
                has_rigid_body = true;
            }
        }

        if (has_rigid_body)
        {
            auto& component = m_ecs_manager->GetComponent<RigidBody>(entity);
            if (ImGui::SliderFloat3("Velocity##RigidBodyPosition", &component.velocity[0], -100.0f, 100.0f));
            if (ImGui::SliderFloat3("Acceleration##RigidBodyAcceleration", &component.acceleration[0], 0.0f, 1.0f));
        }

        ImGui::Separator();

        ImGui::PopID();
    }

    void ImGuiECSWindow::DisplayLightComponentEditor(cgx::ecs::Entity entity)
    {
        bool has_light_component = m_ecs_manager->HasComponent<LightComponent>(entity);

        const char* button_text = has_light_component ? "Remove" : "Add";
        float button_width = ImGui::CalcTextSize(button_text).x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::Text("Light Component (unimplemented)");

        float available_space = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(available_space - button_width);

        ImGui::PushID(static_cast<int>(entity));

        if (has_light_component)
        {
            if (ImGui::Button("Remove##RemoveLightComponent"))
            {
                m_ecs_manager->RemoveComponent<LightComponent>(entity);
                has_light_component = false;
            }
        }
        else
        {
            if (ImGui::Button("Add##AddLightComponent"))
            {
                m_ecs_manager->AddComponent(
                    entity,
                    LightComponent {
                        .position = glm::vec3(0.0f, 0.0f, 0.0f),
                        .color = glm::vec3(0.0f, 0.0f, 0.0f),
                        .intensity = 1.0f
                });
                has_light_component = true;
            }
        }

        if (has_light_component)
        {
            auto& component = m_ecs_manager->GetComponent<LightComponent>(entity);
            ImGui::SliderFloat3("Position##LightPosition", &component.position[0], -100.0f, 100.0f);
            ImGui::SliderFloat3("Color##LightColor", &component.color[0], 0.0f, 1.0f);
            ImGui::SliderFloat("Intensity##LightIntensity", &component.intensity, 0.0f, 100.0f);
        }

        ImGui::Separator();

        ImGui::PopID();
    }

}