#pragma once
#include "embedded_editor_window.h"


enum class EditorStage
{
    None = 0,
    Light,
    Full,
    Count,
};

class CImGuiEditor final
{
public:
    CImGuiEditor();
    ~CImGuiEditor();

    //-- singleton
    CImGuiEditor(const CImGuiEditor&) = delete;
    CImGuiEditor& operator=(const CImGuiEditor&) = delete;

    static CImGuiEditor& Get()
    {
        static CImGuiEditor editor;
        return editor;
    }

    void ShowEditor();

    bool Editor_KeyPress(int key);
    bool Editor_KeyRelease(int key);
    bool Editor_KeyHold(int key);
    bool Editor_MouseMove(int dx, int dy);
    bool Editor_MouseWheel(int direction);

    //-- states
    bool IsEditorActive() const { return curr_stage == EditorStage::Full; }
    bool IsEditorShouldOpen()
    {
        curr_stage = target_stage;
        return target_stage != EditorStage::None;
    }
    bool IsEditorShouldOpenOnNextFrame() const { return target_stage != EditorStage::None; }
    bool IsEditor() const { return curr_stage != EditorStage::None; }

    const xr_vector<CImGuiEditorWnd*>& Windows() const { return m_Windows; };

    EditorStage curr_stage{EditorStage::None};
    EditorStage target_stage{EditorStage::None};

    bool m_bShowDemoWindow{};

private:
    void RenderDockSpace();
    void RenderMenuBar();
    void RenderDemoWindow();

    //-- idk how good is this 
    xr_vector<CImGuiEditorWnd*> m_Windows;
};


class CImGuiMainWnd : public CImGuiEditorWnd
{
public:
    CImGuiMainWnd() : CImGuiEditorWnd("Main") {}
    CImGuiMainWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};
