// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "gui/imgui_panel.h"
#include <vector>

namespace cgx::gui
{
class ImGuiManager;

class ProfilerPanel final : public ImGuiPanel
{
public:
    ProfilerPanel(GUIContext* context, ImGuiManager* manager);
    ~ProfilerPanel() override;

    void render() override;

    void update();

private:
    uint32_t m_current_fps{0};
    double   m_last_frame_time{0.0f};

    uint32_t m_average_fps{0};
    uint32_t m_average_frame_time{0};

    double   m_total_uptime{0.0f};
    uint64_t m_total_frame_count{0};

    std::vector<double>           m_frame_times;
    unsigned int                  m_frame_time_count{0};
    static constexpr unsigned int m_max_frame_time_samples{100};
};
}
