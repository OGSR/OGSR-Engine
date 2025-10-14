#pragma once
#include "embedded_editor_window.h"

class CImGuiPositionInformerWnd : public CImGuiEditorWnd
{
public:
    CImGuiPositionInformerWnd() : CImGuiEditorWnd("Position Informer") {}
    CImGuiPositionInformerWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;

    void SavePosition();
    string256 m_SectionName{"section"};
};
