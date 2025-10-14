#pragma once
#include "embedded_editor_window.h"

class CImGuiHudEditorWnd : public CImGuiEditorWnd
{
public:
    CImGuiHudEditorWnd() : CImGuiEditorWnd("HUD Editor") {}
    CImGuiHudEditorWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};

