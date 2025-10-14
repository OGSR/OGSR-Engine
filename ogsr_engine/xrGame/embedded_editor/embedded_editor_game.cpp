#include "StdAfx.h"
#include "embedded_editor_game.h"
#include "imgui.h"

//-- main rt game rt
void CImGuiGameWnd::Render()
{
    ImGui::Begin(m_Name.c_str(), nullptr, ImGuiWindowFlags_NoMove);

    //-- center the main wp
    ImVec2 avail = ImGui::GetContentRegionAvail();
    const float texAspect = (float)Device.dwWidth / (float)Device.dwHeight;
    const float availAspect = avail.x / avail.y;
    ImVec2 imgSize = avail;
    if (availAspect > texAspect)
    {
        imgSize.x = avail.y * texAspect;
        imgSize.y = avail.y;
    }
    else
    {
        imgSize.x = avail.x;
        imgSize.y = avail.x / texAspect;
    }

    ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + (avail.x - imgSize.x) * 0.5f, cursor.y + (avail.y - imgSize.y) * 0.5f));
    ::Render->DrawMainRTTexture(imgSize.x, imgSize.y);

    ImGui::End();
}
