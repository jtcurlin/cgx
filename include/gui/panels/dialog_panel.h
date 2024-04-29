// Copyright © 2024 Jacob Curlin

// something like this albiet fully open to modification if necessary/beneficial

#pragma once

#include "utility/paths.h"
#include <imguifiledialog/ImGuiFileDialog.h>

namespace cgx::gui
{
class DialogPanel
{
public:
    DialogPanel();
    ~DialogPanel();

    static bool draw_file_import_dialog(
        const std::string& title,
        const std::string& filters,
        std::string&       selection);
    static bool draw_text_input_dialog(const std::string& title, std::string& selection);

private:


};
}
