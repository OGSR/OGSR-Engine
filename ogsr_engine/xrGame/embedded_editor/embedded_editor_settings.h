#pragma once

#include "embedded_editor_window.h"

class CImGuiSettingsWnd : public CImGuiEditorWnd
{
public:
    CImGuiSettingsWnd() : CImGuiEditorWnd("ImSettings###ImSettings") {}
    CImGuiSettingsWnd(LPSTR name) : CImGuiEditorWnd(name) {}

    void Render() override;
};
