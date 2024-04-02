// Copyright © 2024 Jacob Curlin

#pragma once

#include "cgx.h"
#include "cube_map.h"
#include "gui/imgui_render_window.h"

#include <string>


#define AMBIENT_MAP_BIT 1
#define DIFFUSE_MAP_BIT 2
#define SPECULAR_MAP_BIT 4
#define NORMAL_MAP_BIT 8


class Sandbox : public cgx::core::Engine
{

public:
    Sandbox();
    ~Sandbox();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;

    void LoadAssets();
    void SkyboxRender();

private:

    std::vector<std::string> model_filenames;
    std::vector<std::string> shader_names;

    std::unique_ptr<cgx::render::CubeMap> m_skybox;
};
