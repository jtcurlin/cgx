// Copyright © 2024 Jacob Curlin, Connor Cotturone, Chip Bevil, William Osborne

#include "gui/panels/dialog_panel.h"

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
