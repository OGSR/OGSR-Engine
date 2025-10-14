#pragma once
#include "embedded_editor_window.h"

class CImGuiGameWnd : public CImGuiEditorWnd
{
public:
    CImGuiGameWnd() : CImGuiEditorWnd("Game") {}
    CImGuiGameWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};
