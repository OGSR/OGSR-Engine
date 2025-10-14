#pragma once
#include "embedded_editor_window.h"

class CImGuiDebugRTWnd : public CImGuiEditorWnd
{
public:
    CImGuiDebugRTWnd() : CImGuiEditorWnd("Debug RT") {}
    CImGuiDebugRTWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};
