#pragma once
#include "embedded_editor_window.h"

class CImGuiSoundEnvWnd : public CImGuiEditorWnd
{
public:
    CImGuiSoundEnvWnd() : CImGuiEditorWnd("SoundEnv###SoundEnv") {}
    CImGuiSoundEnvWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};
