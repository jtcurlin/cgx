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
    std::filesystem::path asset_dir(std::string(DATA_DIRECTORY) + "/assets");
    std::filesystem::path shader_dir(std::string(DATA_DIRECTORY) + "/shaders");

    // load models
    const std::vector<std::string> model_filenames{
        "soccerball/ball.obj", "light_cube/light_cube.obj", "sponza/sponza.obj",
        // "backpack/backpack.obj",
        // "holodeck/holodeck.obj"
    };
    for (const auto& filename : model_filenames) {
        std::filesystem::path model_path = asset_dir / filename;
        m_asset_manager->import_asset(model_path.string());
    }

    // load model, lighting shaders
    const std::vector<std::string> shader_names = {"model", "lighting"};
    for (const auto& name : shader_names) {
        std::filesystem::path shader_path = shader_dir / name;
        auto                  shader      = std::make_shared<cgx::asset::Shader>(shader_path.string(), name);
        m_asset_manager->add_asset(shader);
    }

    // load skybox 1
    const std::vector<std::string> skybox_1_face_paths = {
        (asset_dir / "skybox_mountains/right.jpg").string(),
        (asset_dir / "skybox_mountains/left.jpg").string(),
        (asset_dir / "skybox_mountains/top.jpg").string(),
        (asset_dir / "skybox_mountains/bottom.jpg").string(),
        (asset_dir / "skybox_mountains/back.jpg").string(),
        (asset_dir / "skybox_mountains/front.jpg").string()
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("cgx://asset/cubemap/skybox01", "skybox01", skybox_1_face_paths));

    // load skybox 2
    const std::vector<std::string> skybox_2_face_paths = {
        (asset_dir / "skybox_2/px.png").string(), (asset_dir / "skybox_2/nx.png").string(),
        (asset_dir / "skybox_2/py.png").string(), (asset_dir / "skybox_2/ny.png").string(),
        (asset_dir / "skybox_2/pz.png").string(), (asset_dir / "skybox_2/nz.png").string()
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("cgx://asset/cubemap/skybox02", "skybox02", skybox_2_face_paths));

    geometry_test();
}

void Sandbox::geometry_test() const
{
    // the asset manager, and engine in general, is currently built largely around imported 3d data, since
    // implementing systems for interactive manual design/creation of objects is not exactly trivial.

    // thus, the asset manager and primitive geometry generation functions aren't directly coupled right now
    // rather, the mesh creation functions, e.g. create_plane/create_sphere, just return a Mesh object (shared pointer).
    // there isn't a way to purely call/generate one of these meshes via the gui, although its planned. thus,
    // if you want to actual render the geometry these functions create, you must manually create the mesh via one of the
    // functions, and then pass it into asset manager's add_asset function, to register it with the asset manager.

    // since the renderer currently considers 'models' the high-order renderable object, you've got to create the mesh,
    // use it to create a model, and then add that model to the asset manager. it'll then be available for assignment to
    // an entity node



    auto my_primitive_sphere_mesh = cgx::geometry::create_sphere(10, 10, 5);

    // this is optional, but if we want to have the mesh itself registered / visible in the asset manager (rather than just the parent model), we must register it
    m_asset_manager->add_asset(my_primitive_sphere_mesh);

    // create meshes vector (just with our one primitive mesh) in order to construct a model
    std::vector<std::shared_ptr<cgx::asset::Mesh>> meshes = { my_primitive_sphere_mesh };

    // pass our meshes vector along with path and tag strings into the model constructor
    auto my_primitives_model = std::make_shared<cgx::asset::Model>("cgx://geometry/primitive-sphere-model", "sphere",  meshes);

    // add the asset to the asset manager
    m_asset_manager->add_asset(my_primitives_model);
}

int main()
{
    Sandbox app;
    app.run();
    return 0;
}
