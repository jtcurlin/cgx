// Copyright © 2024 Jacob Curlin

#include "gui/imgui_manager.h"

#include "core/event_handler.h"
#include "core/events/master_events.h"
#include "core/window_manager.h"

#include "gui/panels/asset_panel.h"
#include "gui/panels/profiler_panel.h"
#include "gui/panels/render_settings_panel.h"
#include "gui/panels/scene_panel.h"
#include "gui/panels/viewport_panel.h"
#include "gui/panels/properties_panel.h"

#include "render/render_system.h"
#include "utility/math.h"
#include "utility/paths.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
#include <sstream>
#include <iostream>

#include "core/input_manager.h"
#include "gui/panels/dialog_panel.h"
#include "scene/scene_manager.h"


namespace cgx::gui
{
ImGuiManager::ImGuiManager(GUIContext* context)
    : m_context(context)
{
    init();
    register_event_handlers();
}

ImGuiManager::~ImGuiManager()
{
    shutdown();
}

void ImGuiManager::shutdown()
{
    const std::string ini_path = std::string(DATA_DIRECTORY) + "/gui_layout.ini";
    ImGui::SaveIniSettingsToDisk(ini_path.c_str());
    CGX_INFO("Saved imgui configuration to disk @ {}", ini_path);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::init()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    load_fonts();
    set_style();

    const auto window_manager = m_context->get_window_manager();
    ImGui_ImplGlfw_InitForOpenGL(window_manager->get_glfw_window(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    register_panel(std::make_unique<AssetPanel>(m_context, this));
    register_panel(std::make_unique<ProfilerPanel>(m_context, this));
    register_panel(std::make_unique<RenderSettingsPanel>(m_context, this));
    register_panel(std::make_unique<ViewportPanel>(m_context, this));
    register_panel(std::make_unique<ScenePanel>(m_context, this));
    register_panel(std::make_unique<PropertiesPanel>(m_context, this));

    const std::string ini_path = std::string(DATA_DIRECTORY) + "/gui_layout.ini";
    ImGui::LoadIniSettingsFromDisk(ini_path.c_str());

}

void ImGuiManager::register_event_handlers()
{
    auto& event_handler = core::EventHandler::get_instance();
    // auto& input_manager = core::InputManager::get_instance();

    event_handler.add_listener(
        core::event::master::TOGGLE_INTERFACE_MODE,
        [this](core::event::Event& event) {
            m_interface_enabled = !m_interface_enabled;
        });

    event_handler.add_listener(
        core::event::master::ACTIVATE_GUI_CONTROL_MODE,
        [this](core::event::Event& event) {
            this->enable_imgui_input();
        });

    event_handler.add_listener(
        core::event::master::ACTIVATE_GAME_CONTROL_MODE,
        [this](core::event::Event& event) {
            this->disable_imgui_input();
        });
}

void ImGuiManager::register_panel(std::unique_ptr<ImGuiPanel> panel)
{
    auto it = std::find_if(
        m_imgui_panels.begin(),
        m_imgui_panels.end(),
        [&panel](const std::unique_ptr<ImGuiPanel>& w) {
            return w.get() == panel.get();
        });
    if (it == m_imgui_panels.end()) {
        m_imgui_panels.push_back(std::move(panel));
    }
    else {
        CGX_ERROR("ImGuiManager : Panel {} already registered.", panel->get_title());
    }
}

ImGuiPanel* ImGuiManager::get_panel(const std::string& panel_title)
{
    for (auto& panel : m_imgui_panels) {
        if (panel_title == panel->get_title()) {
            return panel.get();
        }
    }
    return nullptr;
}

void ImGuiManager::render()
{
    begin_render();

    if (m_interface_enabled) {
        draw_editor();
    }
    if (!m_interface_enabled) {
        draw_fullscreen_render();
    }

    end_render();
}

void ImGuiManager::begin_render() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // create/configure a primary dockspace window
    static bool        open            = true;
    ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // center dockspace and size to match window viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // disable 'window' functionality of the dockspace window
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (!m_interface_enabled) {
        dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("primary_dockspace_window", &open, window_flags);
    ImGui::PopStyleVar(3);

    // submit the dockspace
    if (const ImGuiIO& io = ImGui::GetIO() ; io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        static ImGuiID dockspace_id = ImGui::GetID("primary_dockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
}

void ImGuiManager::draw_editor()
{
    draw_main_menu_bar();

    for (const auto& window : m_imgui_panels) {
        if (window->is_visible()) {
            window->Begin();
            window->render();
            window->End();
        }
    }

    if (m_context->m_importing_node) {
        std::string selected_file = "none";
        if (DialogPanel::draw_file_import_dialog("Select Scene File", ".glb,.gltf", selected_file)) {
            if (selected_file != "none") {
                m_context->get_scene_manager()->import_node(selected_file, dynamic_cast<scene::Node*>(m_context->get_node_to_birth()));
                m_context->set_node_to_birth(nullptr);
            }
            m_context->m_importing_node = false;
        }
    }

    if (m_context->m_adding_scene) {
        std::string scene_name = "";
        if (DialogPanel::draw_text_input_dialog("Enter Scene Label", scene_name)) {
            if (scene_name != "") {
                m_context->get_scene_manager()->add_scene(scene_name);
            }
            m_context->m_adding_scene = false;
        }
    }

    if (m_context->m_renaming_item) {
        std::string new_name = "";
        if (DialogPanel::draw_text_input_dialog("Enter New Tag", new_name)) {
            if (new_name != "") {
                m_context->get_item_to_rename()->set_tag(new_name);
            }
            m_context->m_renaming_item = false;
        }
    }



    // const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    /*
    // rename item popup
    if (m_context->get_item_to_rename() != nullptr) {
        const std::string unique_id = "Rename Item ## CoreRenamePopup" + std::to_string(
                                          m_context->get_item_to_rename()->get_id());
        CGX_INFO("Item to rename is not nullptr");
        ImGui::OpenPopup(unique_id.c_str());
        if (ImGui::BeginPopupModal(unique_id.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::InputText(
                "Enter Tag ##CoreRenamePopup",
                m_input_buffer,
                256,
                ImGuiInputTextFlags_EnterReturnsTrue)) {
                const auto item = m_context->get_item_to_rename();
                item->set_tag(m_input_buffer);
                m_context->set_item_to_rename(nullptr);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
        */
}

void ImGuiManager::draw_fullscreen_render()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    static ImGuiID dockspace_id = ImGui::GetID("primary_dockspace");
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                              ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav;

    ImGui::Begin("fullscreen_render_window", nullptr, window_flags);

    const ImVec2   image_size         = ImGui::GetContentRegionAvail();
    auto texture_id = m_context->get_render_system()->get_output_fb()->get_attachment_info(GL_COLOR_ATTACHMENT0).id;
    ImGui::Image((void*) (intptr_t) texture_id, image_size, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

void ImGuiManager::end_render()
{
    ImGui::End();
    ImGui::Render();

    if (ImGuiIO& io = ImGui::GetIO() ; io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::draw_main_menu_bar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Engine")) {
            if (ImGui::MenuItem("Interface", "tab", m_interface_enabled)) {
                m_interface_enabled = !m_interface_enabled;
            }

            if (ImGui::MenuItem("Quit", "end")) {
                core::EventHandler::get_instance().send_event(core::event::master::QUIT);
            }
            ImGui::EndMenu();
        }
        if (m_interface_enabled) {
            if (ImGui::BeginMenu("Panels")) {
                for (const auto& window : m_imgui_panels) {
                    if (ImGui::MenuItem(window->get_title().c_str(), "", window->is_visible())) {
                        if (window->is_visible()) {
                            window->hide();
                        }
                        else {
                            window->show();
                        }
                    }
                }
                ImGui::EndMenu();
            }
        }

        if (ImGui::BeginMenu("Scene")) {
            const auto scene_manager = m_context->get_scene_manager();
            if (ImGui::MenuItem("Add")) {
                m_context->m_adding_scene = true;
            }
            if (ImGui::MenuItem("Import")) {
                m_context->m_importing_node = true;
                m_context->set_node_to_birth(scene_manager->get_active_scene()->get_root());
            }
            if (ImGui::BeginMenu("Remove")) {
                std::string current_scene_label = scene_manager->get_active_scene()->get_label();
                for (const auto& scene_pair : scene_manager->get_scenes()) {
                    std::string scene_label = scene_pair.first + "##RemoveSceneList";
                    if (ImGui::MenuItem(scene_label.c_str(), "", scene_pair.first == current_scene_label)) {
                        // todo: remove scene
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Select")) {
                std::string current_scene_label = scene_manager->get_active_scene()->get_label();
                for (const auto& scene_pair : scene_manager->get_scenes()) {

                    std::string scene_label = scene_pair.first + "##SelectActiveSceneList";
                    if (ImGui::MenuItem(scene_label.c_str(), "", scene_pair.first == current_scene_label)) {
                        m_context->get_scene_manager()->set_active_scene(scene_pair.first);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ImGuiManager::load_fonts()
{
    const ImGuiIO&              io = ImGui::GetIO();
    const std::filesystem::path fonts_directory(std::string(DATA_DIRECTORY) + "/fonts");

    const std::string regular_text_path  = (fonts_directory / "sf_pro_display_regular.otf").string();
    const std::string bold_text_path     = (fonts_directory / "sf_pro_display_bold.otf").string();
    const std::string regular_icons_path = (fonts_directory / "font_awesome_regular.otf").string();
    const std::string bold_icons_path    = (fonts_directory / "font_awesome_bold.otf").string();

    static constexpr ImWchar icon_ranges[] = {0xf000, 0xffff, 0};

    // body font
    ImFontConfig body_config;
    body_config.MergeMode = false;
    m_body_font           = io.Fonts->AddFontFromFileTTF(regular_text_path.c_str(), 16.0f, &body_config);
    body_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 16.0f, &body_config, icon_ranges);

    // header font
    ImFontConfig header_config;
    header_config.MergeMode = false;
    m_header_font           = io.Fonts->AddFontFromFileTTF(bold_text_path.c_str(), 17.0f, &header_config);
    header_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 17.0f, &header_config, icon_ranges);

    // title font
    ImFontConfig title_config;
    title_config.MergeMode = false;
    m_title_font           = io.Fonts->AddFontFromFileTTF(bold_text_path.c_str(), 20.0f, &title_config);
    title_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 20.0f, &title_config, icon_ranges);

    // small font
    ImFontConfig small_config;
    small_config.MergeMode = false;
    m_small_font           = io.Fonts->AddFontFromFileTTF(regular_text_path.c_str(), 14.0f, &small_config);
    small_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 14.0f, &small_config, icon_ranges);

    io.Fonts->Build();
}

void ImGuiManager::enable_imgui_input()
{
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    // io.MouseDrawCursor = true;
}

void ImGuiManager::disable_imgui_input()
{
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    // io.MouseDrawCursor = false;

    clear_inputs(io);
}

void ImGuiManager::clear_inputs(ImGuiIO& io)
{
    for (int i = 0 ; i < IM_ARRAYSIZE(io.MouseDown) ; i++) {
        io.MouseDown[i] = false;
    }

    for (int i = 0 ; i < IM_ARRAYSIZE(io.KeysDown) ; i++) {
        io.KeysDown[i] = false;
    }

    io.MouseWheel  = 0;
    io.MouseWheelH = 0;

    io.InputQueueCharacters.clear();
}

ImVec4 hex_to_imvec4(const std::string& hex)
{
    const auto color = math::HexToColor4f(hex);
    return {color.r, color.g, color.b, color.a};
}


// @formatter:off
void ImGuiManager::set_style()
{

    // constexpr auto gray = "#2C2A2E";
    constexpr auto dark_gray = "#211F22";
    constexpr auto gray = "#2C2A2E";
    constexpr auto light_gray = "#616161";
    constexpr auto lightest_gray = "#625E65";


    ImGuiStyle& style                   = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]         = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 0.501960813999176f);
    style.Colors[ImGuiCol_WindowBg]     = hex_to_imvec4(gray);
    style.Colors[ImGuiCol_ChildBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.1587982773780823f);
    style.Colors[ImGuiCol_PopupBg]      = hex_to_imvec4("#39373A");
    style.Colors[ImGuiCol_Border]       = hex_to_imvec4("#403E41");
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg]      = hex_to_imvec4("#424043");
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = hex_to_imvec4(dark_gray);
    style.Colors[ImGuiCol_TitleBgActive] = hex_to_imvec4(gray);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = hex_to_imvec4("#39373B");
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5333333611488342f, 0.5333333611488342f, 0.5333333611488342f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3333333432674408f, 0.3333333432674408f, 0.3333333432674408f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.5215686559677124f, 0.8784313797950745f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9803921580314636f, 1.0f);
    style.Colors[ImGuiCol_Button] = hex_to_imvec4("#424043");
    style.Colors[ImGuiCol_ButtonHovered] = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_ButtonActive]  = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_Header]        = hex_to_imvec4(gray);
    style.Colors[ImGuiCol_HeaderHovered] = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_HeaderActive] = hex_to_imvec4(lightest_gray);

    style.Colors[ImGuiCol_Separator] = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(
        0.09803921729326248f,
        0.4000000059604645f,
        0.7490196228027344f,
        0.7799999713897705f);
    style.Colors[ImGuiCol_SeparatorActive] = hex_to_imvec4(lightest_gray);

    style.Colors[ImGuiCol_ResizeGrip] = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        0.250980406999588f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        1.0f);
    style.Colors[ImGuiCol_Tab]        = hex_to_imvec4(gray);
    style.Colors[ImGuiCol_TabHovered] = hex_to_imvec4(lightest_gray);
    style.Colors[ImGuiCol_TabActive] = hex_to_imvec4(light_gray);
    style.Colors[ImGuiCol_TabUnfocused] = hex_to_imvec4(dark_gray);
    style.Colors[ImGuiCol_TabUnfocusedActive] = hex_to_imvec4(gray);

    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 0.5021458864212036f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03862661123275757f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.2039215713739395f,0.2313725501298904f, 0.2823529541492462f, 0.7529411911964417f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);

    style.Alpha                     = 1.0f;
    style.DisabledAlpha             = 0.5f;
    style.WindowPadding             = ImVec2(8.0f, 8.0f);
    style.WindowRounding            = 5.0f;
    style.WindowBorderSize          = 1.0f;
    style.WindowMinSize             = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition  = ImGuiDir_Left;
    style.ChildRounding             = 5.0f;
    style.ChildBorderSize           = 1.0f;
    style.PopupRounding             = 5.0f;
    style.PopupBorderSize           = 1.0f;
    style.FramePadding              = ImVec2(2.0f, 2.0f);
    style.FrameRounding             = 5.0f;
    style.FrameBorderSize           = 0.0f;
    style.ItemSpacing               = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
    style.CellPadding               = ImVec2(4.0f, 2.0f);
    style.IndentSpacing             = 21.0f;
    style.ColumnsMinSpacing         = 6.0f;
    style.ScrollbarSize             = 14.0f;
    style.ScrollbarRounding         = 0.0f;
    style.GrabMinSize               = 10.0f;
    style.GrabRounding              = 0.0f;
    style.TabRounding               = 3.0f;
    style.TabBorderSize             = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition       = ImGuiDir_Left;
    style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);
}
// @formatter:off

void ImGuiManager::toggle_interface()
{
    m_interface_enabled = !m_interface_enabled;
}

void ImGuiManager::set_buffer(std::string tag) {
    strcpy(m_input_buffer, m_context->get_item_to_rename()->get_tag().c_str());
}
}
