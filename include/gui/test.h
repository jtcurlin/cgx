
#pragma once

#include "gui/imgui_window.h"
#include "scene/scene_manager.h"

namespace cgx::gui
{

    class ImGuiSceneManagerWindow : public ImGuiWindow
    {
        ImGuiSceneManagerWindow(std::shared_ptr<cgx::scene::SceneManager> scene_manager);

        void Render() override;

    private:
        std::shared_ptr<cgx::scene::SceneManager> m_scene_manager;

    }


}

// scenemanagerwindow.cpp

namespace cgx::gui
{

    ImGuiSceneManagerWindow::ImGuiSceneManagerWindow(std::Shared_ptr<cgx::scene::SceneMangaer scene_manager)
        : m_scene_manager(scene_manager)
    {

    }

    void Render()
    {
        ImGui::text("This is the scene manager menu");
    }

}