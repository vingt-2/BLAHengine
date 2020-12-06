// You may think I'm a monster for doing this.
// And perhaps you are right

#undef IMGUI_IMPL_OPENGL_LOADER_GL3W
#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "StdInclude.h"

#include "imgui.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"

#include "backends/imgui_impl_vulkan.cpp"

#include "backends/imgui_impl_glfw.cpp"

namespace BLAImGui
{
    // For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
    bool BeginMainToolBar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = g.Viewports[0];
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y));
        ImGui::SetNextWindowViewport(viewport->ID); // Enforce viewport so we don't create our onw viewport when ImGuiConfigFlags_ViewportsNoMerge is set.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        bool is_open = ImGui::Begin("##MainToolBar", NULL, window_flags) && ImGui::BeginMenuBar();
        ImGui::PopStyleVar(2);
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
        if (!is_open)
        {
            ImGui::End();
            return false;
        }
        return true; //-V1020
    }

    void EndMainToolBar()
    {
        ImGui::EndMenuBar();

        // When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
        // FIXME: With this strategy we won't be able to restore a NULL focus.
        ImGuiContext& g = *GImGui;
        if (g.CurrentWindow == g.NavWindow && g.NavLayer == 0 && !g.NavAnyRequest)
            ImGui::FocusTopMostWindowUnderOne(g.NavWindow, NULL);

        ImGui::End();
    }
}

#include <algorithm>
#include <chrono>
#include <string>
#include <regex>
#include <cmath>

#include "DevGuiTextEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS

using namespace BLA;

#include "DevGuiTextEditor.inc" 