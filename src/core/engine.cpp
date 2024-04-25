// Copyright © 2024 Jacob Curlin

#include "core/engine.h"
#include "core/window_manager.h"
#include "core/input_manager.h"

#include "core/systems/time_system.h"
#include "core/systems/physics_system.h"
#include "core/systems/transform_system.h"
#include "core/systems/hierarchy_system.h"

#include "core/events/master_events.h"

#include "core/components/render.h"
#include "core/components/transform.h"
#include "core/components/rigid_body.h"
#include "core/components/point_light.h"

#include "asset/asset_manager.h"
#include "asset/import/asset_importer_image.h"
#include "asset/import/asset_importer_obj.h"

#include "ecs/ecs_manager.h"
#include "core/event_handler.h"

#include "gui/gui_context.h"
#include "gui/imgui_manager.h"

#include "render/render_system.h"
#include "scene/scene_manager.h"

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
    m_time_system = std::make_unique<TimeSystem>();

    m_window_manager = std::make_shared<WindowManager>(
        m_settings.window_width,
        m_settings.window_height,
        m_settings.window_title);

    InputManager::GetSingleton().initialize(m_window_manager);

    m_ecs_manager = std::make_unique<ecs::ECSManager>();

    m_ecs_manager->register_component<component::Hierarchy>();
    m_ecs_manager->register_component<component::Transform>();
    m_ecs_manager->register_component<component::RigidBody>();
    m_ecs_manager->register_component<component::Render>();
    m_ecs_manager->register_component<component::PointLight>();

    auto m_hierarchy_system = m_ecs_manager->register_system<HierarchySystem>(); {
        ecs::Signature signature;
        signature.set(m_ecs_manager->get_component_type<component::Hierarchy>());
        m_ecs_manager->set_system_signature<HierarchySystem>(signature);
    }

    auto m_transform_system = m_ecs_manager->register_system<TransformSystem>(); {
        ecs::Signature signature;
        signature.set(m_ecs_manager->get_component_type<component::Transform>());
        m_ecs_manager->set_system_signature<TransformSystem>(signature);
    }
    m_transform_system->initialize(m_hierarchy_system.get());

    m_ecs_manager->register_system<PhysicsSystem>(); {
        ecs::Signature signature;
        signature.set(m_ecs_manager->get_component_type<component::RigidBody>());
        signature.set(m_ecs_manager->get_component_type<component::Transform>());
        m_ecs_manager->set_system_signature<PhysicsSystem>(signature);
    }

    m_render_system = m_ecs_manager->register_system<render::RenderSystem>(); {
        ecs::Signature signature;
        signature.set(m_ecs_manager->get_component_type<component::Render>());
        signature.set(m_ecs_manager->get_component_type<component::Transform>());
        m_ecs_manager->set_system_signature<render::RenderSystem>(signature);
    }
    m_render_system->initialize();

    setup_engine_events();

    m_asset_manager = std::make_shared<asset::AssetManager>();
    m_asset_manager->register_importer(std::make_shared<asset::AssetImporterImage>());
    m_asset_manager->register_importer(std::make_shared<asset::AssetImporterOBJ>());

    m_scene_manager = std::make_shared<scene::SceneManager>(m_ecs_manager.get(), m_asset_manager.get());
    m_scene_manager->add_scene("main_scene");
    m_scene_manager->set_active_scene("main_scene");

    setup_gui();
}

void Engine::update()
{
    constexpr double fps_limit_60 = 1.0 / 60;
    m_time_system->frame_update(fps_limit_60);

    const auto dt = m_time_system->get_frame_time();
    m_ecs_manager->update(static_cast<float>(dt));
}

void Engine::render()
{
    m_render_system->render();
    m_imgui_manager->render();
}

void Engine::setup_engine_events()
{
    auto& input_manager = InputManager::GetSingleton();
    auto& event_handler = EventHandler::get_instance();

    // on , : toggle gui interface
    const event::Event toggle_interface_event(event::master::TOGGLE_INTERFACE_MODE);
    input_manager.bind_key_input_event(Key::key_comma, KeyAction::press, toggle_interface_event);
    event_handler.add_listener(
        event::master::TOGGLE_INTERFACE_MODE,
        [this](event::Event& event) {
            switch (m_interface_mode) {
                case Mode::Game: {
                    EventHandler::get_instance().send_event(event::master::ACTIVATE_GUI_INTERFACE);
                    m_interface_mode = Mode::GUI;
                    break;
                }
                case Mode::GUI: {
                    EventHandler::get_instance().send_event(event::master::ACTIVATE_GAME_INTERFACE);
                    m_interface_mode = Mode::Game;
                    break;
                }
                default: return;
            }
        });

    // on . : toggle control mode
    const event::Event toggle_control_event(event::master::TOGGLE_CONTROL_MODE);
    input_manager.bind_key_input_event(Key::key_period, KeyAction::press, toggle_control_event);
    event_handler.add_listener(
        event::master::TOGGLE_CONTROL_MODE,
        [this](event::Event& event) {
            switch (m_control_mode) {
                case Mode::Game: {
                    EventHandler::get_instance().send_event(event::master::ACTIVATE_GUI_CONTROL_MODE);
                    m_control_mode = Mode::GUI;
                    break;
                }
                case Mode::GUI: {
                    EventHandler::get_instance().send_event(event::master::ACTIVATE_GAME_CONTROL_MODE);
                    m_control_mode = Mode::Game;
                    break;
                }
                default: return;
            }
        });

    // on ` : quit engine & close window
    const event::Event quit_event(event::master::QUIT);
    input_manager.bind_key_input_event(Key::key_fslash, KeyAction::press, quit_event);
    event_handler.add_listener(
        event::master::QUIT,
        [this](event::Event& event) {
            this->m_is_running = false;
        });
}

void Engine::setup_gui()
{
    m_gui_context = std::make_unique<gui::GUIContext>(
        m_asset_manager.get(),
        m_ecs_manager.get(),
        m_render_system.get(),
        m_scene_manager.get(),
        m_window_manager.get(),
        m_time_system.get());

    m_imgui_manager = std::make_unique<gui::ImGuiManager>(m_gui_context.get());
}

}
