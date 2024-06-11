#pragma once

class CUIWindow;
class CUIDialogWnd;
class CUICursor;
class CUIMessageBoxEx;
#include "..\xr_3da\iinputreceiver.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "ui/UIWndCallback.h"
#include "ui_base.h"

class CMainMenu : public IMainMenu, public IInputReceiver, public pureRender, public CDialogHolder, public CUIWndCallback, public CDeviceResetNotifier

{
    CUIDialogWnd* m_startDialog;

    enum
    {
        flRestoreConsole = (1 << 0),
        flRestorePause = (1 << 1),
        flRestorePauseStr = (1 << 2),
        flActive = (1 << 3),
        flNeedChangeCapture = (1 << 4),
        flRestoreCursor = (1 << 5),
        flGameSaveScreenshot = (1 << 6),
        flNeedVidRestart = (1 << 7),
    };
    Flags16 m_Flags;
    string_path m_screenshot_name;
    u32 m_screenshotFrame;
    void ReadTextureInfo();

    xr_vector<CUIWindow*> m_pp_draw_wnds;

private:
    ref_sound CurrentSound;
    bool languageChanged;

public:
    enum EErrorDlg
    {
        SessionTerminate,
        ErrMax,
        ErrNoError = ErrMax,
    };

protected:
    EErrorDlg m_NeedErrDialog;
    u32 m_start_time;

    xr_vector<CUIMessageBoxEx*> m_pMB_ErrDlgs;

public:
    u32 m_deactivated_frame;
    virtual void DestroyInternal(bool bForce);
    CMainMenu();
    virtual ~CMainMenu();

    virtual void Activate(bool bActive);
    virtual bool IsActive();
    virtual bool CanSkipSceneRendering();

    virtual bool IsLanguageChanged() const;
    virtual void SetLanguageChanged(bool status);

    virtual void IR_OnMousePress(int btn);
    virtual void IR_OnMouseRelease(int btn);
    virtual void IR_OnMouseHold(int btn);
    virtual void IR_OnMouseMove(int x, int y);
    virtual void IR_OnMouseStop(int x, int y);

    virtual void IR_OnKeyboardPress(int dik);
    virtual void IR_OnKeyboardRelease(int dik);
    virtual void IR_OnKeyboardHold(int dik);

    virtual void IR_OnMouseWheel(int direction);

    bool OnRenderPPUI_query();
    void OnRenderPPUI_main();
    void OnRenderPPUI_PP();

    virtual void OnRender();
    virtual void OnFrame(void);
    virtual void StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
    virtual bool UseIndicators() { return false; }

    void OnDeviceCreate();

    void Screenshot(IRender_interface::ScreenshotMode mode = IRender_interface::SM_NORMAL, LPCSTR name = 0);
    void RegisterPPDraw(CUIWindow* w);
    void UnregisterPPDraw(CUIWindow* w);

    void SetErrorDialog(EErrorDlg ErrDlg);
    EErrorDlg GetErrorDialogType() const { return m_NeedErrDialog; };
    void CheckForErrorDlg();
    void OnSessionTerminate(LPCSTR reason);
    void SetNeedVidRestart();
    virtual void OnDeviceReset();
    LPCSTR GetGSVer();

    void PlaySound(LPCSTR Path);
};

extern CMainMenu* MainMenu();
