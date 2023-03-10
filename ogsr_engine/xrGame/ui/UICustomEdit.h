#pragma once

#include "UILines.h"
#include "UIWindow.h"

class CUICustomEdit : public CUIWindow, public CUILinesOwner
{
    u32 m_max_symb_count;

public:
    CUICustomEdit();
    virtual ~CUICustomEdit() = default;
    // CUILinesOwner
    virtual void SetFont(CGameFont* pFont) { CUILinesOwner::SetFont(pFont); }
    virtual CGameFont* GetFont() { return CUILinesOwner::GetFont(); }
    virtual void SetTextColor(u32 color);
    virtual void SetTextColorD(u32 color);

    virtual void Init(float x, float y, float width, float height);
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
    virtual bool OnKeyboardHold(int dik);
    virtual void OnFocusLost();

    virtual void Update();
    virtual void Draw();

    void CaptureFocus(bool bCapture) { m_bInputFocus = bCapture; }
    virtual void SetText(const char* str);
    virtual const char* GetText();
    void SetMaxCharCount(u32 cnt) { m_max_symb_count = cnt; }
    virtual void Enable(bool status);
    void SetNumbersOnly(bool status);
    void SetFloatNumbers(bool status);
    void SetPasswordMode(bool mode = true);
    void SetDbClickMode(bool mode = true) { m_bFocusByDbClick = mode; }
    void SetCursorColor(u32 color) { m_lines.SetCursorColor(color); }

    //Какой-то недодел
    void SetLightAnim(LPCSTR lanim);

    virtual void SetTextPosX(float x);
    virtual void SetTextPosY(float y);

protected:
    bool KeyPressed(int dik);
    bool KeyReleased(int dik);

    virtual void AddChar(const u16 c);

    bool m_bInputFocus;

    bool m_bNumbersOnly;
    bool m_bFloatNumbers;
    bool m_bFocusByDbClick;

    u32 m_textColor[2];

    // DIK клавиши, кот. нажата и удерживается, 0 если такой нет
    int m_iKeyPressAndHold;
    bool m_bHoldWaitMode;

    //	u32	m_cursorColor;

    CLAItem* m_lanim;
};
