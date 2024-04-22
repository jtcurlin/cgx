// Copyright © 2024 Jacob Curlin

#include "gui/imgui_manager.h"

#include "gui/panels/asset_panel.h"
#include "gui/panels/profiler_panel.h"
#include "gui/panels/render_settings_panel.h"
#include "gui/panels/scene_panel.h"
#include "gui/panels/viewport_panel.h"
#include "gui/panels/properties_panel.h"

#include "core/window_manager.h"
#include "utility/paths.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>

namespace cgx::gui
{
ImGuiManager::ImGuiManager(GUIContext* context)
    : m_context(context)
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
}

ImGuiManager::~ImGuiManager()
{
    shutdown();
}

void ImGuiManager::initialize()
{
    register_panel(std::make_unique<AssetPanel>(m_context, this));
    register_panel(std::make_unique<ProfilerPanel>(m_context, this));
    register_panel(std::make_unique<RenderSettingsPanel>(m_context, this));
    register_panel(std::make_unique<ViewportPanel>(m_context, this));
    register_panel(std::make_unique<ScenePanel>(m_context, this));
    register_panel(std::make_unique<PropertiesPanel>(m_context, this));

    const std::string ini_path = std::string(DATA_DIRECTORY) + "/gui_layout.ini";
    ImGui::LoadIniSettingsFromDisk(ini_path.c_str());
}

void ImGuiManager::shutdown()
{
    const std::string ini_path = std::string(DATA_DIRECTORY) + "/gui_layout.ini";
    ImGui::SaveIniSettingsToDisk(ini_path.c_str());
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

void ImGuiManager::render()
{
    begin_render();

    render_core_menu();

    for (const auto& window : m_imgui_panels) {
        if (window->is_visible()) {
            window->Begin();
            window->render();
            window->End();
        }
    }

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Rename Node ##PopUp", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::InputText("Rename Node ##InputField", m_input_buffer, 256);
        if (ImGui::Button("Ok ##Rename Node")) {
            const auto item = m_context->get_item_to_rename();
            item->set_tag(m_input_buffer);
            m_context->set_item_to_rename(nullptr);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    end_render();
}

void ImGuiManager::begin_render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dock Space
    static bool               dockSpaceOpen   = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags          window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace
    if (const ImGuiIO& io = ImGui::GetIO() ; io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End(); // End the DockSpace window
}

void ImGuiManager::end_render()
{
    ImGui::Render();
    // ImGuiIO& io = ImGui::GetIO();
    // (void) io;
    if (ImGuiIO& io = ImGui::GetIO() ; io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::render_core_menu() const
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Engine")) {
            for (const auto& window : m_imgui_panels) {
                if (ImGui::MenuItem(window->get_title().c_str(), "", window->is_visible())) {
                    window->show();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ImGuiManager::load_fonts()
{
    const ImGuiIO&              io              = ImGui::GetIO();
    const std::filesystem::path fonts_directory(std::string(DATA_DIRECTORY) + "/fonts");

    const std::string regular_text_path  = (fonts_directory / "sf_pro_display_regular.otf").string();
    const std::string bold_text_path     = (fonts_directory / "sf_pro_display_bold.otf").string();
    const std::string regular_icons_path = (fonts_directory / "font_awesome_regular.otf").string();
    const std::string bold_icons_path    = (fonts_directory / "font_awesome_bold.otf").string();

    static constexpr ImWchar icon_ranges[] = {0xf000, 0xffff, 0};

    // body font
    ImFontConfig body_config;
    body_config.MergeMode = false;
    m_body_font      = io.Fonts->AddFontFromFileTTF(regular_text_path.c_str(), 16.0f, &body_config);
    body_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 16.0f, &body_config, icon_ranges);

    // header font
    ImFontConfig header_config;
    header_config.MergeMode = false;
    m_header_font    = io.Fonts->AddFontFromFileTTF(bold_text_path.c_str(), 17.0f, &header_config);
    header_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 17.0f, &header_config, icon_ranges);

    // title font
    ImFontConfig title_config;
    title_config.MergeMode = false;
    m_title_font     = io.Fonts->AddFontFromFileTTF(bold_text_path.c_str(), 20.0f, &title_config);
    title_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 20.0f, &title_config, icon_ranges);

    // small font
    ImFontConfig small_config;
    small_config.MergeMode = false;
    m_small_font     = io.Fonts->AddFontFromFileTTF(regular_text_path.c_str(), 14.0f, &small_config);
    small_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(bold_icons_path.c_str(), 14.0f, &small_config, icon_ranges);

    io.Fonts->Build();
}

void ImGuiManager::enable_input()
{
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    // io.MouseDrawCursor = true;
}

void ImGuiManager::disable_input()
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

void ImGuiManager::set_style()
{
    ImGuiStyle& style                   = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]         = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(
        0.8313725590705872f,
        0.8470588326454163f,
        0.8784313797950745f,
        0.501960813999176f);
    style.Colors[ImGuiCol_WindowBg]     = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
    style.Colors[ImGuiCol_ChildBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.1587982773780823f);
    style.Colors[ImGuiCol_PopupBg]      = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
    style.Colors[ImGuiCol_Border]       = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg]      = ImVec4(
        0.105882354080677f,
        0.1137254908680916f,
        0.1372549086809158f,
        0.501960813999176f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        0.250980406999588f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(
        0.105882354080677f,
        0.1137254908680916f,
        0.1372549086809158f,
        1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5333333611488342f, 0.5333333611488342f, 0.5333333611488342f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(
        0.3333333432674408f,
        0.3333333432674408f,
        0.3333333432674408f,
        1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(
        0.6000000238418579f,
        0.6000000238418579f,
        0.6000000238418579f,
        1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.5215686559677124f, 0.8784313797950745f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(
        0.2588235437870026f,
        0.5882353186607361f,
        0.9803921580314636f,
        1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(
        0.3529411822557449f,
        0.4125490242242813f,
        0.4417647081613541f,
        0.501960813999176f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_Header]        = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        0.250980406999588f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(
        0.09803921729326248f,
        0.4000000059604645f,
        0.7490196228027344f,
        0.7799999713897705f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(
        0.09803921729326248f,
        0.4000000059604645f,
        0.7490196228027344f,
        1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
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
    style.Colors[ImGuiCol_Tab]        = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        0.250980406999588f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(
        0.3098039329051971f,
        0.6235294342041016f,
        0.9333333373069763f,
        1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(
        0.2039215713739395f,
        0.2313725501298904f,
        0.2823529541492462f,
        1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(
        0.2039215713739395f,
        0.2313725501298904f,
        0.2823529541492462f,
        0.5021458864212036f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03862661123275757f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(
        0.2039215713739395f,
        0.2313725501298904f,
        0.2823529541492462f,
        0.7529411911964417f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(
        0.105882354080677f,
        0.1137254908680916f,
        0.1372549086809158f,
        0.7529411911964417f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(
        0.105882354080677f,
        0.1137254908680916f,
        0.1372549086809158f,
        0.7529411911964417f);

    style.Alpha                     = 1.0f;
    style.DisabledAlpha             = 0.5f;
    style.WindowPadding             = ImVec2(8.0f, 8.0f);
    style.WindowRounding            = 0.0f;
    style.WindowBorderSize          = 1.0f;
    style.WindowMinSize             = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition  = ImGuiDir_Left;
    style.ChildRounding             = 0.0f;
    style.ChildBorderSize           = 1.0f;
    style.PopupRounding             = 0.0f;
    style.PopupBorderSize           = 1.0f;
    style.FramePadding              = ImVec2(4.0f, 3.0f);
    style.FrameRounding             = 0.0f;
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
    style.TabRounding               = 0.0f;
    style.TabBorderSize             = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition       = ImGuiDir_Left;
    style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);
}
}
