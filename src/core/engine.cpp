// Copyright © 2024 Jacob Curlin

#include "core/engine.h"
#include "event/events/engine_events.h"

#include <iostream>

namespace cgx::core {

    Engine::Engine()
            : m_time_data{0.0, 0.0}, m_is_running(false) {}

    // main game loop
    void Engine::Run() {
        Initialize();
        m_is_running = true;

        static double curr_time = 0.0;
        while (m_is_running) {
            while (glfwGetTime() < m_time_data.last_time + 0.016) {}        // limit fps to 60

            // frame timing
            curr_time = static_cast<float>(glfwGetTime());
            m_time_data.delta_time = curr_time - m_time_data.last_time;
            m_time_data.last_time = curr_time;

            Update();
            Render();

            m_window_manager->Update();
        }

        Shutdown();
    }

    void Engine::Initialize() {
        cgx::utility::LoggingHandler::Initialize();
        CGX_INFO("Engine: Initializing.")

        m_time_system = Time();
        m_time_system.Start();

        m_event_handler = std::make_shared<cgx::event::EventHandler>();
        m_window_manager= std::make_shared<cgx::core::WindowManager>();
        m_window_manager->Initialize(m_settings.window_width,
                                     m_settings.window_height,
                                     "CGX");
        cgx::input::InputManager::GetSingleton().Initialize(m_event_handler, m_window_manager);

        
        SetupECS(); // setup ECS registries and register standard components
        
        // setup main scene
        m_scene_manager = std::make_shared<cgx::scene::SceneManager>();
        auto scene = m_scene_manager->AddScene("main_scene",
                                               m_entity_registry,
                                               m_component_registry,
                                               m_system_registry,
                                               m_event_handler);
        
        // setup / register physics system
        auto physics_system = m_system_registry->RegisterSystem<cgx::core::PhysicsSystem>();
        {
            cgx::ecs::Signature signature;
            signature.set(m_component_registry->GetComponentType<cgx::component::RigidBody>());
            signature.set(m_component_registry->GetComponentType<cgx::component::Transform>());
            m_system_registry->SetSignature<cgx::core::PhysicsSystem>(signature);
        }

        // retup / register rendering system
        m_render_system = m_system_registry->RegisterSystem<cgx::render::RenderSystem>();
        {
            cgx::ecs::Signature signature;
            signature.set(m_component_registry->GetComponentType<cgx::component::Render>());
            signature.set(m_component_registry->GetComponentType<cgx::component::Transform>());
            m_system_registry->SetSignature<cgx::render::RenderSystem>(signature);
        }
        m_render_system->Initialize();

        SetupEngineEvents();        // setup default engine event callbacks etc.
        SetupGUI();                 // setup imgui menus

        auto model_importer = std::make_shared<cgx::resource::ResourceImporterOBJ>();
        auto image_importer = std::make_shared<cgx::resource::ResourceImporterImage>();

        auto& resource_manager = cgx::resource::ResourceManager::getSingleton();
        resource_manager.setEventHandler(m_event_handler);

        resource_manager.RegisterImporter<cgx::resource::Model>(model_importer);
        resource_manager.RegisterImporter<cgx::resource::Texture>(image_importer);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDepthMask(GL_TRUE);
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LESS);
    }

    void Engine::Update() {
        m_time_system.Update();
        TimeContext& update_time_data = m_time_system.getLastUpdate();
        double& dt = update_time_data.frame_time;

        m_system_registry->Update(dt);
    }

    void Engine::Render() {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_render_system->Render();
        m_imgui_manager->Render();

    }

    void Engine::SetupEngineEvents()
    {
        auto& input_manager = cgx::input::InputManager::GetSingleton();

        // 'esc' : quit engine & close window
        cgx::event::Event quit_event(cgx::events::engine::QUIT);
        input_manager.BindKeyInputEvent(cgx::input::Key::key_escape, cgx::input::KeyAction::press, quit_event);
        m_event_handler->AddListener(cgx::events::engine::QUIT, [this](cgx::event::Event& event) {
            this->m_is_running = false;
        });

        // 'm' : activate manual camera control (look/move around)
        cgx::event::Event enable_camera_control_event(cgx::events::engine::ENABLE_CAMERA_CONTROL);
        input_manager.BindKeyInputEvent(cgx::input::Key::key_m, cgx::input::KeyAction::press, enable_camera_control_event);
        m_event_handler->AddListener(cgx::events::engine::ENABLE_CAMERA_CONTROL, [this](cgx::event::Event& event) {
            this->m_window_manager->DisableCursor();
            this->m_imgui_manager->DisableInput();
        });

        // 'g' : activate GUI control (normal cursor operation, fixed camera)
        cgx::event::Event disable_camera_control_event(cgx::events::engine::DISABLE_CAMERA_CONTROL);
        input_manager.BindKeyInputEvent(cgx::input::Key::key_g, cgx::input::KeyAction::press, disable_camera_control_event);
        m_event_handler->AddListener(cgx::events::engine::DISABLE_CAMERA_CONTROL, [this](cgx::event::Event& event) {
            this->m_window_manager->EnableCursor();
            this->m_imgui_manager->EnableInput();
        });
    }

    void Engine::SetupGUI()
    {
        // imgui manager
        m_imgui_manager = std::make_unique<cgx::gui::ImGuiManager>();
        m_imgui_manager->Initialize(m_window_manager->getGLFWWindow());
        m_imgui_manager->SetStyle((m_settings.font_dir / "SFPRODISPLAYMEDIUM.OTF").c_str());

        // imgui resource manager menu
        m_resource_manager_adapter = std::make_shared<cgx::gui::ResourceManagerAdapter>(m_event_handler);
        m_imgui_resource_manager_window = std::make_unique<cgx::gui::ImGuiResourceManagerWindow>(m_settings.asset_dir, m_resource_manager_adapter);
        m_imgui_manager->RegisterImGuiWindow(m_imgui_resource_manager_window.get());

        // imgui render viewport window
        m_imgui_render_window = std::make_unique<cgx::gui::ImGuiRenderWindow>(m_render_system->getFramebuffer());
        m_imgui_manager->RegisterImGuiWindow(m_imgui_render_window.get());

        // imgui ecs menu
        // m_imgui_ecs_window = std::make_unique<cgx::gui::ImGuiECSWindow>(m_ecs_manager, m_resource_manager_adapter);
        // m_imgui_manager->RegisterImGuiWindow(m_imgui_ecs_window.get());

        // imgui performance statistics window
        m_imgui_performance_window = std::make_unique<cgx::gui::ImGuiPerformanceWindow>(m_time_system);
        m_imgui_manager->RegisterImGuiWindow(m_imgui_performance_window.get());

        // (todo: remove/move to renderer class) render settings setup
        // m_render_settings = std::make_shared<cgx::gui::RenderSettings>();
        // m_render_settings->msaa = false;
        // m_render_settings->skybox = false;

        // imgui render settings window
        // m_imgui_render_settings_window = std::make_unique<cgx::gui::ImGuiRenderSettingsWindow>(m_render_settings);
        // m_imgui_manager->RegisterImGuiWindow(m_imgui_render_settings_window.get());
    }

    void Engine::SetupECS()
    {
        m_entity_registry = std::make_shared<cgx::ecs::EntityManager>();
        m_component_registry = std::make_shared<cgx::ecs::ComponentManager>();
        m_system_registry = std::make_shared<cgx::ecs::SystemManager>(m_component_registry);

        // register components
        m_component_registry->RegisterComponent<cgx::component::Transform>();
        m_component_registry->RegisterComponent<cgx::component::RigidBody>();
        m_component_registry->RegisterComponent<cgx::component::Render>();
        m_component_registry->RegisterComponent<cgx::component::PointLight>();
    }

    void Engine::Shutdown() {}
}