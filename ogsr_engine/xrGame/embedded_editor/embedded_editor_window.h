#pragma once
#include "stdafx.h"
#include "imgui.h"

class CImGuiEditorWnd
{
public:
    CImGuiEditorWnd(LPCSTR name) { m_Name = name; }
    virtual ~CImGuiEditorWnd() {}

    virtual bool RenderBegin() { 
        m_Collapsed = !ImGui::Begin(m_Name.c_str(), &m_Opened, m_flags);
        if (!m_Opened)
            return false;
        return !m_Collapsed;
    }
    virtual void Render() {}
    virtual void RenderEnd() { ImGui::End(); }


    ImGuiWindowFlags_ m_flags{ImGuiWindowFlags_None};
    shared_str m_Name{};
    bool m_Opened{};
    bool m_Collapsed{};

    bool m_bCanHide{true};
};
