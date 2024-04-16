// Copyright © 2024 Jacob Curlin

#include "sandbox.h"

Sandbox::Sandbox() = default;

Sandbox::~Sandbox() = default;

void Sandbox::initialize()
{
    Engine::initialize();
    load_assets();
}

void Sandbox::update()
{
    Engine::update();
}

void Sandbox::render()
{
    Engine::render();
}

void Sandbox::load_assets() const
{
    // load models
    const std::vector<std::string> model_filenames{
        "soccerball/ball.obj", "light_cube/light_cube.obj", "sponza/sponza.obj",
        // "backpack/backpack.obj",
        // "holodeck/holodeck.obj"
    };
    for (const auto& filename : model_filenames) {
        std::filesystem::path model_path = m_settings.asset_dir / filename;
        m_asset_manager->import_asset(model_path.string());
    }

    // load model, lighting shaders
    const std::vector<std::string> shader_names = {"model", "lighting"};
    for (const auto& name : shader_names) {
        std::filesystem::path shader_path = m_settings.shader_dir / name;
        auto                  shader      = std::make_shared<cgx::asset::Shader>(shader_path.string(), name);
        m_asset_manager->add_asset(shader);
    }

    // load skybox 1
    const std::vector<std::string> skybox_1_face_paths = {
        (m_settings.asset_dir / "skybox_mountains/right.jpg").string(),
        (m_settings.asset_dir / "skybox_mountains/left.jpg").string(),
        (m_settings.asset_dir / "skybox_mountains/top.jpg").string(),
        (m_settings.asset_dir / "skybox_mountains/bottom.jpg").string(),
        (m_settings.asset_dir / "skybox_mountains/back.jpg").string(),
        (m_settings.asset_dir / "skybox_mountains/front.jpg").string()
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("cgx://asset/cubemap/skybox01", "skybox01", skybox_1_face_paths));

    // load skybox 2
    const std::vector<std::string> skybox_2_face_paths = {
        (m_settings.asset_dir / "skybox_2/px.png").string(), (m_settings.asset_dir / "skybox_2/nx.png").string(),
        (m_settings.asset_dir / "skybox_2/py.png").string(), (m_settings.asset_dir / "skybox_2/ny.png").string(),
        (m_settings.asset_dir / "skybox_2/pz.png").string(), (m_settings.asset_dir / "skybox_2/nz.png").string()
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("cgx://asset/cubemap/skybox02", "skybox02", skybox_2_face_paths));
}

int main()
{
    Sandbox app;
    app.run();
    return 0;
}
