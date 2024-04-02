// Copyright © 2024 Jacob Curlin

#pragma once

#include "gui/imgui_window.h"

namespace cgx::scene
{
    class SceneManager;
}

namespace cgx::gui
{

    class ImGuiSceneWindow : public ImGuiWindow
    {
    public:
        ImGuiSceneWindow(std::shared_ptr<cgx::scene::SceneManager> scene_manager);

        void Render() override;

    private:
        std::shared_ptr<cgx::scene::SceneManager> m_scene_manager;

    }; // Class ImGuiWindow



} // namespace cgx::gui