#pragma once

#include "UIDialogWnd.h"
#include "UIWndCallback.h"

class CUIXml;
class CUI3tButton;
class CUIStatic;
class CUIEditBoxEx;

class CUIPdaSpot : public CUIDialogWnd, public CUIWndCallback
{
    using base_class = CUIDialogWnd;

    CUIStatic* m_background;
    CUIEditBoxEx* m_editBox;
    CUI3tButton* m_btn_ok;
    CUI3tButton* m_btn_cancel;

    bool m_mainWnd;
    LPCSTR m_levelName;
    Fvector m_position;
    u16 m_spotID;
    shared_str m_spotType;

public:
    CUIPdaSpot();
    ~CUIPdaSpot() = default;

    void Init(u16 spot_id, LPCSTR level_name, Fvector pos, bool main_wnd);
    void InitControls();

    void OnAdd(CUIWindow* w, void* d);
    void OnApply(CUIWindow* w, void* d);
    void OnExit(CUIWindow* w, void* d);
    void Exit();
    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
};
