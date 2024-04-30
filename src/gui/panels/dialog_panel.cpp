// Copyright © 2024 Jacob Curlin, Connor Cotturone, Chip Bevil, William Osborne

#include "gui/panels/dialog_panel.h"

namespace cgx::gui
{
DialogPanel::~DialogPanel() = default;

bool DialogPanel::draw_file_import_dialog(const std::string& title, const std::string& filters, std::string& selection)
{
    static IGFD::FileDialogConfig file_dialog_config{.path = std::string(DATA_DIRECTORY)};
    static bool initialized = false;
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
{}

}
