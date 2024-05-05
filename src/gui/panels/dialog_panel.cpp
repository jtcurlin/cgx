// Copyright © 2024 Jacob Curlin

#include "gui/panels/dialog_panel.h"
#include "core/input_manager.h"

namespace cgx::gui
{
DialogPanel::~DialogPanel() = default;

bool DialogPanel::draw_file_import_dialog(const std::string& title, const std::string& filters, std::string& selection)
{
    static IGFD::FileDialogConfig file_dialog_config{.path = std::string(DATA_DIRECTORY)};
    static bool                   initialized = false;
    if (!initialized) {
        ImGuiFileDialog::Instance()->OpenDialog("dialog_panel", title, filters.c_str(), file_dialog_config);
    }

    if (ImGuiFileDialog::Instance()->Display("dialog_panel")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            selection = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        else {
            selection = "";
        }
        ImGuiFileDialog::Instance()->Close();
        return true;
    }
    return false;
}

bool DialogPanel::draw_text_input_dialog(const std::string& title, std::string& selection)
{
    static char input_buffer[256];
    bool success   = false;
    bool exited   = false;

    const std::string popup_title = title + "##TextInputDialogPopup";
    ImGui::OpenPopup(popup_title.c_str());

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(popup_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Enter Label");
        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##TextInputFieldDialogPopup", input_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
            selection = std::string(input_buffer);
            success = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemTooltip("Press enter to submit, escape to cancel.");

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            exited = true;
        }

        ImGui::EndPopup();
        if (success || exited) {
            return true;
        }
    }
    return false;
}

bool DialogPanel::draw_key_input_dialog(const std::string& title, core::Key& selection)
{
    auto& input_manager = core::InputManager::get_instance();
    bool success = false;
    bool exited = false;

    const std::string popup_title = title + "##TextInputDialogPopup";
    ImGui::OpenPopup(popup_title.c_str());

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::SetNextWindowSize(ImVec2(90.0f, 8.0f));
    if (ImGui::BeginPopupModal(popup_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (input_manager.is_key_pressed(core::Key::key_0)) {
            success = true;
            selection = core::Key::key_0;
        }

        if (input_manager.is_key_pressed(core::Key::key_1)) {
            success = true;
            selection = core::Key::key_1;
        }

        if (input_manager.is_key_pressed(core::Key::key_2)) {
            success = true;
            selection = core::Key::key_2;
        }

        if (input_manager.is_key_pressed(core::Key::key_3)) {
            success = true;
            selection = core::Key::key_3;
        }

        if (input_manager.is_key_pressed(core::Key::key_4)) {
            success = true;
            selection = core::Key::key_4;
        }

        if (input_manager.is_key_pressed(core::Key::key_5)) {
            success = true;
            selection = core::Key::key_5;
        }

        if (input_manager.is_key_pressed(core::Key::key_6)) {
            success = true;
            selection = core::Key::key_6;
        }

        if (input_manager.is_key_pressed(core::Key::key_7)) {
            success = true;
            selection = core::Key::key_7;
        }

        if (input_manager.is_key_pressed(core::Key::key_8)) {
            success = true;
            selection = core::Key::key_8;
        }

        if (input_manager.is_key_pressed(core::Key::key_9)) {
            success = true;
            selection = core::Key::key_9;
        }

        if (input_manager.is_key_pressed(core::Key::key_a)) {
            success = true;
            selection = core::Key::key_a;
        }

        if (input_manager.is_key_pressed(core::Key::key_b)) {
            success = true;
            selection = core::Key::key_b;
        }

        if (input_manager.is_key_pressed(core::Key::key_c)) {
            success = true;
            selection = core::Key::key_c;
        }

        if (input_manager.is_key_pressed(core::Key::key_d)) {
            success = true;
            selection = core::Key::key_d;
        }

        if (input_manager.is_key_pressed(core::Key::key_e)) {
            success = true;
            selection = core::Key::key_e;
        }

        if (input_manager.is_key_pressed(core::Key::key_f)) {
            success = true;
            selection = core::Key::key_f;
        }

        if (input_manager.is_key_pressed(core::Key::key_g)) {
            success = true;
            selection = core::Key::key_g;
        }

        if (input_manager.is_key_pressed(core::Key::key_h)) {
            success = true;
            selection = core::Key::key_h;
        }

        if (input_manager.is_key_pressed(core::Key::key_i)) {
            success = true;
            selection = core::Key::key_i;
        }

        if (input_manager.is_key_pressed(core::Key::key_j)) {
            success = true;
            selection = core::Key::key_j;
        }

        if (input_manager.is_key_pressed(core::Key::key_k)) {
            success = true;
            selection = core::Key::key_k;
        }

        if (input_manager.is_key_pressed(core::Key::key_l)) {
            success = true;
            selection = core::Key::key_l;
        }

        if (input_manager.is_key_pressed(core::Key::key_m)) {
            success = true;
            selection = core::Key::key_m;
        }

        if (input_manager.is_key_pressed(core::Key::key_n)) {
            success = true;
            selection = core::Key::key_n;
        }

        if (input_manager.is_key_pressed(core::Key::key_o)) {
            success = true;
            selection = core::Key::key_o;
        }

        if (input_manager.is_key_pressed(core::Key::key_p)) {
            success = true;
            selection = core::Key::key_p;
        }

        if (input_manager.is_key_pressed(core::Key::key_q)) {
            success = true;
            selection = core::Key::key_q;
        }

        if (input_manager.is_key_pressed(core::Key::key_r)) {
            success = true;
            selection = core::Key::key_r;
        }

        if (input_manager.is_key_pressed(core::Key::key_s)) {
            success = true;
            selection = core::Key::key_s;
        }

        if (input_manager.is_key_pressed(core::Key::key_t)) {
            success = true;
            selection = core::Key::key_t;
        }

        if (input_manager.is_key_pressed(core::Key::key_u)) {
            success = true;
            selection = core::Key::key_u;
        }

        if (input_manager.is_key_pressed(core::Key::key_v)) {
            success = true;
            selection = core::Key::key_v;
        }

        if (input_manager.is_key_pressed(core::Key::key_w)) {
            success = true;
            selection = core::Key::key_w;
        }

        if (input_manager.is_key_pressed(core::Key::key_x)) {
            success = true;
            selection = core::Key::key_x;
        }

        if (input_manager.is_key_pressed(core::Key::key_y)) {
            success = true;
            selection = core::Key::key_y;
        }

        if (input_manager.is_key_pressed(core::Key::key_z)) {
            success = true;
            selection = core::Key::key_z;
        }

        if (input_manager.is_key_pressed(core::Key::key_comma)) {
            CGX_INFO("Master event key, please select another");
        }

        if (input_manager.is_key_pressed(core::Key::key_period)) {
            success = true;
            selection = core::Key::key_period;
        }

        if (input_manager.is_key_pressed(core::Key::key_fslash)) {
            success = true;
            selection = core::Key::key_fslash;
        }

        if (input_manager.is_key_pressed(core::Key::key_apostrophe)) {
            success = true;
            selection = core::Key::key_apostrophe;
        }

        if (input_manager.is_key_pressed(core::Key::key_space)) {
            success = true;
            selection = core::Key::key_space;
        }

        if (input_manager.is_key_pressed(core::Key::key_enter)) {
            success = true;
            selection = core::Key::key_enter;
        }

        if (input_manager.is_key_pressed(core::Key::key_tab)) {
            CGX_INFO("Master event key, please select another");
        }

        if (input_manager.is_key_pressed(core::Key::key_backspace)) {
            success = true;
            selection = core::Key::key_backspace;
        }

        if (input_manager.is_key_pressed(core::Key::key_insert)) {
            success = true;
            selection = core::Key::key_insert;
        }

        if (input_manager.is_key_pressed(core::Key::key_delete)) {
            success = true;
            selection = core::Key::key_delete;
        }

        if (input_manager.is_key_pressed(core::Key::key_right)) {
            success = true;
            selection = core::Key::key_right;
        }

        if (input_manager.is_key_pressed(core::Key::key_left)) {
            success = true;
            selection = core::Key::key_left;
        }

        if (input_manager.is_key_pressed(core::Key::key_down)) {
            success = true;
            selection = core::Key::key_down;
        }

        if (input_manager.is_key_pressed(core::Key::key_up)) {
            success = true;
            selection = core::Key::key_up;
        }

        if (input_manager.is_key_pressed(core::Key::key_page_up)) {
            success = true;
            selection = core::Key::key_page_up;
        }

        if (input_manager.is_key_pressed(core::Key::key_page_down)) {
            success = true;
            selection = core::Key::key_page_down;
        }

        if (input_manager.is_key_pressed(core::Key::key_home)) {
            success = true;
            selection = core::Key::key_home;
        }

        if (input_manager.is_key_pressed(core::Key::key_end)) {
            CGX_INFO("Master event key, please select another");
        }

        if (input_manager.is_key_pressed(core::Key::key_caps_lock)) {
            success = true;
            selection = core::Key::key_caps_lock;
        }

        if (input_manager.is_key_pressed(core::Key::key_num_lock)) {
            success = true;
            selection = core::Key::key_num_lock;
        }

        if (input_manager.is_key_pressed(core::Key::key_print_screen)) {
            success = true;
            selection = core::Key::key_print_screen;
        }

        if (input_manager.is_key_pressed(core::Key::key_pause)) {
            success = true;
            selection = core::Key::key_pause;
        }

        if (input_manager.is_key_pressed(core::Key::key_f1)) {
            success = true;
            selection = core::Key::key_f1;
        }

        if (input_manager.is_key_pressed(core::Key::key_f2)) {
            success = true;
            selection = core::Key::key_f2;
        }

        if (input_manager.is_key_pressed(core::Key::key_f3)) {
            success = true;
            selection = core::Key::key_f3;
        }

        if (input_manager.is_key_pressed(core::Key::key_f4)) {
            success = true;
            selection = core::Key::key_f4;
        }

        if (input_manager.is_key_pressed(core::Key::key_f5)) {
            success = true;
            selection = core::Key::key_f5;
        }

        if (input_manager.is_key_pressed(core::Key::key_f6)) {
            success = true;
            selection = core::Key::key_f6;
        }

        if (input_manager.is_key_pressed(core::Key::key_f7)) {
            success = true;
            selection = core::Key::key_f7;
        }

        if (input_manager.is_key_pressed(core::Key::key_f8)) {
            success = true;
            selection = core::Key::key_f8;
        }

        if (input_manager.is_key_pressed(core::Key::key_f9)) {
            success = true;
            selection = core::Key::key_f9;
        }

        if (input_manager.is_key_pressed(core::Key::key_f10)) {
            success = true;
            selection = core::Key::key_f10;
        }

        if (input_manager.is_key_pressed(core::Key::key_f11)) {
            success = true;
            selection = core::Key::key_f11;
        }

        if (input_manager.is_key_pressed(core::Key::key_f12)) {
            success = true;
            selection = core::Key::key_f12;
        }

        if (input_manager.is_key_pressed(core::Key::key_left_ctrl)) {
            success = true;
            selection = core::Key::key_left_ctrl;
        }

        if (input_manager.is_key_pressed(core::Key::key_escape)) {
            exited = true;
        }

        ImGui::EndPopup();
        if (success || exited) {
            return true;
        }
    }
    return false;
}

/*
bool DialogPanel::draw_text_input_dialog(const std::string& title, std::string& selection)
{
    bool success   = false;
    bool exited    = false;
    bool attempted = false;

    ImGui::OpenPopup("Add Scene ##AddSceneMenu");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // ImGui::PushFont(m_manager->m_title_font);
    if (ImGui::BeginPopupModal("Add Scene ##AddSceneMenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // ImGui::PushFont(m_manager->m_body_font);

        ImGui::TextUnformatted("Enter Label");
        ImGui::SetNextItemWidth(300.0f);
        if (ImGui::InputText("##AddScene-LabelInput", m_input_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
            attempted = true;
        }
        ImGui::SetItemTooltip("Press enter to add scene, escape to cancel.");

        if (attempted) {
            const auto& scenes = m_context->get_scene_manager()->get_scenes();
            if (std::strlen(m_input_buffer) == 0) {
                m_error_message = "Please specify a valid scene label.";
                m_error_active  = true;
            }
            else if (scenes.find(std::string(m_input_buffer)) != scenes.end()) {
                m_error_message = "Specified scene label is already in use.";
                m_error_active  = true;
            }
            else {
                auto*      scene_manager = m_context->get_scene_manager();
                const auto scene_label   = std::string(m_input_buffer);
                scene_manager->add_scene(scene_label);
                scene_manager->set_active_scene(scene_label);
                success = true;
            }
        }

        if (m_error_active) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextUnformatted(m_error_message.c_str());
            ImGui::PopStyleColor();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            exited = true;
        }

        if (success || exited) {
            m_adding_scene    = false;
            m_error_active    = false;
            m_input_buffer[0] = '\0';
            m_error_message   = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopFont();
        ImGui::EndPopup();
    }
    ImGui::PopFont();
}
*/

}
