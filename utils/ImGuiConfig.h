#pragma once

#include <imgui.h>

namespace ImGuiConfig
{
    inline void ConfigureStyle()
    {
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowRounding = 4.0f;
        style.FrameRounding = 2.0f;
        style.FrameBorderSize = 1.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 3.0f;
        style.WindowBorderSize = 0.0f;
    }
} // namespace ImGuiConfig