// Copyright © 2024 Jacob Curlin

#include "gui/panels/profiler_panel.h"

namespace cgx::gui
{
ProfilerPanel::ProfilerPanel(const std::shared_ptr<GUIContext> &context)
    : ImGuiPanel("Performance", context)
{}

ProfilerPanel::~ProfilerPanel() = default;

void ProfilerPanel::render()
{
    update();

    ImGui::Text("Frame Time: %.3f ms", m_last_frame_time * 1000.0); // Convert to milliseconds
    ImGui::Text("Current FPS: %u", m_current_fps);
    ImGui::Text("Average FPS: %u", m_average_fps);
    ImGui::Text("Average Frame Time: %u ms", m_average_frame_time);
    ImGui::Text("Total Uptime: %.2f seconds", m_total_uptime);
    ImGui::Text("Total Frames Rendered: %llu", m_total_frame_count);
}

void ProfilerPanel::update()
{
    // calculate/update display m_current_fps, m_last_frame_time, m_average_fps, m_average_frame_time,
    // m_total_uptime, m_total_frame_count

    const auto time_system = m_context->get_time_system();

    const auto frame_time = time_system->get_frame_time();
    m_last_frame_time = frame_time;

    m_current_fps = (frame_time > 0) ? static_cast<uint32_t>(1.0 / frame_time) : 0;

    if (m_frame_times.size() < m_max_frame_time_samples) {
        m_frame_times.push_back(frame_time);
    }
    else {
        m_frame_times[m_frame_time_count % m_max_frame_time_samples] = frame_time;
    }
    m_frame_time_count++;

    double total_frame_time = 0;
    for (const auto ft : m_frame_times) {
        total_frame_time += ft;
    }
    double average_frame_time = total_frame_time / m_frame_times.size();
    m_average_frame_time = static_cast<uint32_t>(average_frame_time * 1000);

    if (average_frame_time > 0) {
        m_average_fps = static_cast<uint32_t>(1.0 / average_frame_time);
    }
    else {
        m_average_fps = 0;
    }

    m_total_uptime = time_system->get_uptime();
    m_total_frame_count = time_system->get_frame_number();
}
}
