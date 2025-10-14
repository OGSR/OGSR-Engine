#pragma once
#include "embedded_editor_window.h"

class CImGuiWeatherWnd : public CImGuiEditorWnd
{
public:
    CImGuiWeatherWnd() : CImGuiEditorWnd("Weather###Weather") {}
    CImGuiWeatherWnd(LPSTR name) : CImGuiEditorWnd(name) {}

    void Render() override;
};

