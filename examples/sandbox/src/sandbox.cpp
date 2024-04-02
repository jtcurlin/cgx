// Copyright © 2024 Jacob Curlin

#include "sandbox.h"
#include <imgui.h>

Sandbox::Sandbox() {}

Sandbox::~Sandbox() {}

void Sandbox::Initialize()
{
    Engine::Initialize();
    LoadAssets();   // load models / textures etc. 
}

void Sandbox::Update()
{
    Engine::Update();
}

void Sandbox::Render()
{
    Engine::Render();
}


void Sandbox::LoadAssets()
{
    model_filenames = {
        "soccerball/ball.obj", 
        "light_cube/light_cube.obj", 
        "sponza/sponza.obj", 
        // "backpack/backpack.obj",
        // "holodeck/holodeck.obj"
    };
    shader_names = {"model", "lighting"};   // i.e. "model" -> fetches 'cgx/cgx/shaders/model.vs' and 'cgx/cgx/shaders/model.fs'

    /*
    std::vector<std::string> face_paths = {
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/right.jpg",   // right
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/left.jpg",    // left
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/top.jpg",     // top
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/bottom.jpg",  // bottom
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/front.jpg",   // front
        "/Users/curlin/dev/cgx/build/cgx_debug/data/assets/skybox_mountains/back.jpg",    // back
    };

    m_skybox = std::make_unique<cgx::render::CubeMap>(face_paths, m_resource_manager->loadShader("skybox", m_settings.shader_dir.string()));
    */

    for (const auto& filename : model_filenames)
    {
        std::filesystem::path model_path = m_settings.asset_dir / filename;
        cgx::resource::ResourceManager::getSingleton().ImportResource<cgx::resource::Model>(model_path);
    }

    for (const auto& name : shader_names)
    {
        auto shader = std::make_shared<cgx::resource::Shader>((m_settings.shader_dir / name).string(), name);
        cgx::resource::ResourceManager::getSingleton().RegisterResource<cgx::resource::Shader>(shader, false);
    }
}

/* fuck this 
void Sandbox::SkyboxRender()
{
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera->getZoom()), 
        (float)m_settings.render_width / (float)m_settings.render_height, 
        0.1f, 100.0f
    );
    m_skybox->Draw(view, projection);
}
*/

void Sandbox::Shutdown() 
{
    Engine::Shutdown();
}

int main()
{
    Sandbox app;
    app.Run();
    return 0;
}