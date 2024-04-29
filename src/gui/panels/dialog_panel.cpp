// Copyright © 2024 Jacob Curlin

#include "gui/panels/dialog_panel.h"

#include "utility/logging.h"

namespace cgx::gui
{
DialogPanel::~DialogPanel() = default;

bool DialogPanel::draw_file_import_dialog(const std::string& title, const std::string& filters, std::string& selection)
{
    // static ImGuiFileDialog file_dialog;
    static IGFD::FileDialogConfig file_dialog_config{.path = std::string(DATA_DIRECTORY)};
    static bool initialized = false;
    if (!initialized) {
        ImGuiFileDialog::Instance()->OpenDialog("dialog_panel", title, filters.c_str(), file_dialog_config);
    }

    CGX_INFO("drawing import dialog");

    constexpr ImGuiWindowFlags file_dialog_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

    bool file_selected = false;
    // if (ImGuiFileDialog::Instance()->Display("FileImportDialog##dialogpanel", file_dialog_flags, ImVec2(0,0), ImVec2(0,350))) {
    if (ImGuiFileDialog::Instance()->Display("dialog_panel")) {
        file_selected = ImGuiFileDialog::Instance()->IsOk();
        if (file_selected) {
            selection = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        ImGuiFileDialog::Instance()->Close();
    }
    return file_selected;
}

bool DialogPanel::draw_text_input_dialog(const std::string& title, std::string& selection)
{
    // todo
}

}
