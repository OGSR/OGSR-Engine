#include "StdAfx.h"
#include "embedded_editor_debug_rt.h"
#include "imgui.h"

void CImGuiDebugRTWnd::Render()
{
    if (!RenderBegin())
    {
        RenderEnd();
        return;
    }

    ImGui::Text("Debug render targets");
    ImGui::Separator();
    ::Render->DrawRTTextures();

    RenderEnd();
}
