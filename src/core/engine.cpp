// Copyright © 2024 Jacob Curlin

#include "core/engine.h"

#include "core/window_manager.h"
#include "core/time_system.h"
#include "core/physics_system.h"
#include "ecs/entity_registry.h"
#include "ecs/component_registry.h"
#include "ecs/system_registry.h"
#include "scene/scene_manager.h"
#include "render/render_system.h"
#include "input/input_manager.h"
#include "event/event_handler.h"
#include "event/events/engine_events.h"
#include "gui/gui_context.h"
#include "gui/imgui_manager.h"
#include "ecs/components/render.h"
#include "ecs/components/transform.h"
#include "ecs/components/rigid_body.h"
#include "ecs/components/point_light.h"
#include "asset/asset_manager.h"
#include "asset/import/asset_importer_image.h"
#include "asset/import/asset_importer_obj.h"

namespace cgx::core
{
Engine::Engine()  = default;
Engine::~Engine() = default;

// main game loop
void Engine::run()
{
    initialize();
    m_is_running = true;

    while (m_is_running) {
        update();
        render();

        m_window_manager->update();
    }
}

void Engine::initialize()
{
    util::LoggingHandler::initialize();
    m_time_system = std::make_shared<TimeSystem>();

    m_window_manager = std::make_shared<WindowManager>(
        m_settings.window_width,
        m_settings.window_height,
        m_settings.window_title);

    input::InputManager::GetSingleton().initialize(m_window_manager);

    setup_ecs(); // setup ECS registries and register standard components

    // setup main scene
    m_scene_manager = std::make_shared<scene::SceneManager>();
    auto scene = m_scene_manager->add_scene("main_scene", m_entity_registry, m_component_registry, m_system_registry);

    // setup / register physics system
    auto physics_system = m_system_registry->register_system<PhysicsSystem>(); {
        ecs::Signature signature;
        signature.set(m_component_registry->get_component_type<component::RigidBody>());
        signature.set(m_component_registry->get_component_type<component::Transform>());
        m_system_registry->set_signature<PhysicsSystem>(signature);
    }

    // retup / register rendering system
    m_render_system = m_system_registry->register_system<render::RenderSystem>(); {
        ecs::Signature signature;
        signature.set(m_component_registry->get_component_type<component::Render>());
        signature.set(m_component_registry->get_component_type<component::Transform>());
        m_system_registry->set_signature<render::RenderSystem>(signature);
    }
    m_render_system->initialize();

    setup_engine_events();

    m_asset_manager = std::make_shared<asset::AssetManager>();
    m_asset_manager->register_importer(std::make_shared<asset::AssetImporterImage>());
    m_asset_manager->register_importer(std::make_shared<asset::AssetImporterOBJ>());

    setup_gui();
}

void Engine::update()
{
    constexpr double fps_limit_60 = 1.0 / 60;
    m_time_system->frame_update(fps_limit_60);

    const auto dt = m_time_system->get_frame_time();
    m_system_registry->update(static_cast<float>(dt));
}

void Engine::render()
{
    m_render_system->render();
    m_imgui_manager->render();
}

void Engine::setup_engine_events()
{
    auto& input_manager = input::InputManager::GetSingleton();
    auto& event_handler = event::EventHandler::get_instance();

    // 'esc' : quit engine & close window
    const event::Event quit_event(events::engine::QUIT);
    input_manager.bind_key_input_event(input::Key::key_escape, input::KeyAction::press, quit_event);
    event_handler.AddListener(
        events::engine::QUIT,
        [this](event::Event& event) {
            this->m_is_running = false;
        });

    // 'm' : activate manual camera control (look/move around)
    const event::Event enable_camera_control_event(events::engine::ENABLE_CAMERA_CONTROL);
    input_manager.bind_key_input_event(input::Key::key_m, input::KeyAction::press, enable_camera_control_event);
    event_handler.AddListener(
        events::engine::ENABLE_CAMERA_CONTROL,
        [this](event::Event& event) {
            this->m_window_manager->disable_cursor();
            this->m_imgui_manager->disable_input();
        });

    // 'g' : activate GUI control (normal cursor operation, fixed camera)
    const event::Event disable_camera_control_event(events::engine::DISABLE_CAMERA_CONTROL);
    input_manager.bind_key_input_event(input::Key::key_g, input::KeyAction::press, disable_camera_control_event);
    event_handler.AddListener(
        events::engine::DISABLE_CAMERA_CONTROL,
        [this](event::Event& event) {
            this->m_window_manager->enable_cursor();
            this->m_imgui_manager->enable_input();
        });
}

void Engine::setup_gui()
{
    m_gui_context = std::make_shared<gui::GUIContext>(
        m_asset_manager,
        m_render_system,
        m_scene_manager,
        m_window_manager,
        m_time_system);

    m_imgui_manager = std::make_shared<gui::ImGuiManager>(m_gui_context);
    m_imgui_manager->initialize();
}

void Engine::setup_ecs()
{
    m_entity_registry    = std::make_shared<ecs::EntityRegistry>();
    m_component_registry = std::make_shared<ecs::ComponentRegistry>();
    m_system_registry    = std::make_shared<ecs::SystemRegistry>(m_component_registry);

    // register components
    m_component_registry->register_component<component::Transform>();
    m_component_registry->register_component<component::RigidBody>();
    m_component_registry->register_component<component::Render>();
    m_component_registry->register_component<component::PointLight>();
}
}
