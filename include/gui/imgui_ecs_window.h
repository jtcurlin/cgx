// Copyright © 2024 Jacob Curlin


/* unused

#pragma once

#include "gui/imgui_window.h"
#include "gui/resource_manager_adapter.h"

#include "ecs/system.h"
#include "ecs/common.h"

namespace cgx::gui
{
    class ImGuiECSWindow : public ImGuiWindow
    {
    public:
        ImGuiECSWindow(std::shared_ptr<cgx::ecs::ECSManager> ecs_manager,
                       std::shared_ptr<ResourceManagerAdapter> resource_manager_adapter);
            
        void Render() override;

        void RenderActiveEntitiesSection();

        void DisplayRenderComponentEditor(cgx::ecs::Entity entity);
        void DisplayTransformComponentEditor(cgx::ecs::Entity entity);
        void DisplayLightComponentEditor(cgx::ecs::Entity entity);
        void DisplayRigidBodyEditor(cgx::ecs::Entity entity);

    private:
        std::vector<cgx::ecs::Entity>                       m_entities;
        std::shared_ptr<cgx::ecs::ECSManager>               m_ecs_manager;        
        std::shared_ptr<ResourceManagerAdapter>             m_resource_manager_adapter;
        cgx::ecs::Entity                                    m_current_entity;

    }; // class ImGuiECSWindow

} // namespace cgx::gui

*/