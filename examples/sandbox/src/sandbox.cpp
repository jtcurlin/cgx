// Copyright © 2024 Jacob Curlin

#include "sandbox.h"

#include "core/components/controllable.h"

Sandbox::Sandbox() = default;

Sandbox::~Sandbox() = default;

void Sandbox::initialize()
{
    Engine::initialize();

    m_scene_manager->add_scene("Default Scene");
    m_scene_manager->set_active_scene("Default Scene");

    m_scene_manager->import_node(std::string(DATA_DIRECTORY) + "/assets/scenes/default_scene.glb");


    const auto        root   = m_scene_manager->get_active_scene()->get_root();
    cgx::scene::Node* camera = nullptr;
    root->for_each(
        [&camera](cgx::core::Hierarchy& hierarchy) -> bool {
            if (auto* casted_node = dynamic_cast<cgx::scene::Node*>(&hierarchy) ; casted_node) {
                if (casted_node->is_camera()) {
                    camera = casted_node;
                    return false;
                }
            }
            return true;
        });

    if (!camera) {
        camera = m_scene_manager->add_node(
            "Default Camera",
            cgx::scene::NodeFlag::Camera);
    }

    auto& controllable_c = m_ecs_manager->get_component<cgx::component::Controllable>(camera->get_entity());
    controllable_c.enable_rotation = true;
    controllable_c.enable_translation = true;

    auto* viewport_panel        = m_imgui_manager->get_panel("Viewport");
    auto* casted_viewport_panel = dynamic_cast<cgx::gui::ViewportPanel*>(viewport_panel);
    if (casted_viewport_panel) {
        auto node_sptr = camera->get_shared();
        casted_viewport_panel->set_camera(dynamic_pointer_cast<cgx::scene::Node>(node_sptr));
    }

    m_render_system->get_render_settings().default_shader_enabled = true;
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
    std::filesystem::path          kenney = asset_dir / "kenney";
    const std::vector<std::string> model_filenames{
        "misc/soccerball/ball.obj", (kenney / "city/obj/large_buildingA.obj").string(),
    };
    for (const auto& filename : model_filenames) {
        std::filesystem::path model_path = asset_dir / filename;
        m_asset_manager->import_asset(model_path.string());
    }

    // load model, lighting shaders
    const std::vector<std::string> shader_names = {"model", "lighting", "pbr", "default"};
    for (const auto& name : shader_names) {
        std::filesystem::path shader_path = shader_dir / name;
        auto shader = std::make_shared<cgx::asset::Shader>(name, shader_path.string(), cgx::asset::ShaderType::Unknown);
        m_asset_manager->add_asset(shader);
    }

    std::filesystem::path grid_tex_path(
        std::string(DATA_DIRECTORY) + "/assets/kenney/prototype_textures/png/Dark/texture_13.png");
    const std::vector<std::string> grid_skybox_face_paths = {
        grid_tex_path.string(), grid_tex_path.string(), grid_tex_path.string(), grid_tex_path.string(),
        grid_tex_path.string(), grid_tex_path.string(),
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("grid_cubemap", "/assets/misc/skybox_2", grid_skybox_face_paths));

    // load skybox 2
    const std::vector<std::string> skybox_2_face_paths = {
        (asset_dir / "misc/skybox_2/px.png").string(), (asset_dir / "misc/skybox_2/nx.png").string(),
        (asset_dir / "misc/skybox_2/py.png").string(), (asset_dir / "misc/skybox_2/ny.png").string(),
        (asset_dir / "misc/skybox_2/pz.png").string(), (asset_dir / "misc/skybox_2/nz.png").string()
    };
    auto default_skybox_id = m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>(
            "Default Skybox",
            "cgx://asset/cubemap/default_skybox",
            skybox_2_face_paths));

    auto default_skybox = std::dynamic_pointer_cast<cgx::asset::Cubemap>(m_asset_manager->get_asset(default_skybox_id));
    m_render_system->set_skybox_cubemap(default_skybox);


    /*
    // load skybox 1
    const std::vector<std::string> skybox_1_face_paths = {
        (asset_dir / "skybox_mountains/right.jpg").string(), (asset_dir / "skybox_mountains/left.jpg").string(),
        (asset_dir / "skybox_mountains/top.jpg").string(), (asset_dir / "skybox_mountains/bottom.jpg").string(),
        (asset_dir / "skybox_mountains/back.jpg").string(), (asset_dir / "skybox_mountains/front.jpg").string()
    };
    m_asset_manager->add_asset(
        std::make_shared<cgx::asset::Cubemap>("skybox01", "cgx://asset/cubemap/skybox01", skybox_1_face_paths));
    */

    geometry_test();
}

/*
void Sandbox::audio_setup()
{
    CGX_INFO("Setting up OpenAL Audio Context");

    m_al_device = alcOpenDevice(NULL);
    CGX_VERIFY(m_al_device != nullptr);

    m_al_context = alcCreateContext(m_al_device, NULL);
    CGX_VERIFY(m_al_context);

    alcMakeContextCurrent(m_al_context);

    alGenSources(1, &m_al_source);

    alSourcef(m_al_source, AL_PITCH, 1);
    alSourcef(m_al_source, AL_GAIN, 1);
    alSource3f(m_al_source, AL_POSITION, 0, 0, 0);
    alSource3f(m_al_source, AL_VELOCITY, 0, 0, 0);
    alSourcei(m_al_source, AL_LOOPING, AL_FALSE);

    // Generate a buffer to store audio data
    alGenBuffers(1, &m_al_buffer);

    CGX_INFO("Completed OpenAL Setup.");
}

void Sandbox::audio_test()
{
    std::string data_dir = std::string(DATA_DIRECTORY);
    std::filesystem::path test_wav_filepath = std::filesystem::path(data_dir) / "audio" / "CantinaBand3.wav";
    CGX_INFO("Loading audio file ({})", test_wav_filepath.string());

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE *sndfile = sf_open(test_wav_filepath.c_str(), SFM_READ, &sfinfo);
    CGX_VERIFY(sndfile != nullptr);

    if (!(sfinfo.format & SF_FORMAT_WAV)) {
        sf_close(sndfile);
        CGX_FATAL("Unsupported Audio Format.");
    }

    // Allocate memory to hold the audio samples
    const size_t num_samples = static_cast<size_t>(sfinfo.frames) * sfinfo.channels;
    std::vector<short> samples(num_samples);

    // Read audio samples
    const sf_count_t num_read = sf_read_short(sndfile, samples.data(), num_samples);
    CGX_VERIFY(num_read == num_samples);

    // Determine the OpenAL format
    ALenum format;
    if (sfinfo.channels == 1)
        format = AL_FORMAT_MONO16;
    else if (sfinfo.channels == 2)
        format = AL_FORMAT_STEREO16;
    else {
        sf_close(sndfile);
        throw std::runtime_error("Unsupported channel count.");
    }

    // Bind the buffer with the audio data
    alBufferData(m_al_buffer, format, samples.data(), num_samples * sizeof(short), sfinfo.samplerate);

    // Attach the buffer to the source
    alSourcei(m_al_source, AL_BUFFER, m_al_buffer);

    // Play the source
    CGX_INFO("Playing Audio.");
    alSourcePlay(m_al_source);

    // Check source state and wait for the playback to stop
    ALint source_state;
    alGetSourcei(m_al_source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(m_al_source, AL_SOURCE_STATE, &source_state);
    }

    CGX_INFO("Finished Playing Audio.");

    // Close the sound file
    sf_close(sndfile);
}
*/

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
    std::vector<std::shared_ptr<cgx::asset::Mesh>> meshes = {my_primitive_sphere_mesh};

    // pass our meshes vector along with path and tag strings into the model constructor
    auto my_primitives_model = std::make_shared<cgx::asset::Model>(
        "cgx://geometry/primitive-sphere-model",
        "sphere",
        meshes);

    // add the asset to the asset manager
    m_asset_manager->add_asset(my_primitives_model);
}

int main()
{
    Sandbox app;
    app.run();
    return 0;
}
