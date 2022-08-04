// XR_IOConsole.h: interface for the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "iinputreceiver.h"

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/ConsoleRender.h"

// refs
class ENGINE_API CGameFont;
class ENGINE_API IConsole_Command;

class ENGINE_API CConsole : public IInputReceiver, public pureRender, public pureFrame
{
private:
    FactoryPtr<IConsoleRender>* m_pRender = nullptr;

public:
    // t-defs
    struct str_pred
    {
        IC bool operator()(const char* x, const char* y) const { return xr_strcmp(x, y) < 0; }
    };
    typedef xr_map<LPCSTR, IConsole_Command*, str_pred> vecCMD;
    typedef vecCMD::iterator vecCMD_IT;
    enum
    {
        MAX_LEN = 1024
    };

private:
    u32 last_mm_timer;
    float cur_time;
    float rep_time;
    float fAccel;

    int cmd_delta;
    int old_cmd_delta;

    char* editor_last;
    BOOL bShift;

    BOOL bRepeat;
    BOOL RecordCommands;

    void out_font(const char* text, float& pos_y);

protected:
    int scroll_delta;
    char editor[MAX_LEN];
    BOOL bCursor;

    CGameFont* pFont;

public:
    virtual ~CConsole()
    {
        if (m_pRender)
            xr_delete(m_pRender);
    }

    string64 ConfigFile;
    BOOL bVisible;
    vecCMD Commands;

    void AddCommand(IConsole_Command*);
    void RemoveCommand(IConsole_Command*);
    void Reset();

    void Show();
    void Hide();

    void Save();
    void Execute(LPCSTR cmd);
    void ExecuteScript(LPCSTR name);
    void ExecuteCommand(bool = false);

    // get
    BOOL GetBool(LPCSTR cmd, BOOL& val);
    float GetFloat(LPCSTR cmd, float& val, float& min, float& max);
    char* GetString(LPCSTR cmd);
    int GetInteger(LPCSTR cmd, int& val, int& min, int& max);
    char* GetToken(LPCSTR cmd);
    const xr_token* GetXRToken(LPCSTR cmd);
    Fvector GetFVector(LPCSTR cmd);
    Fvector* GetFVectorPtr(LPCSTR cmd);
    IConsole_Command* GetCommand(LPCSTR cmd);
    //	char *			GetNextValue		(LPCSTR cmd);
    //	char *			GetPrevValue		(LPCSTR cmd);

    void SelectCommand();

    // keyboard
    void OnPressKey(int dik, BOOL bHold = false);
    virtual void IR_OnKeyboardPress(int dik);
    virtual void IR_OnKeyboardHold(int dik);
    virtual void IR_OnKeyboardRelease(int dik);

    // render & onmove
    virtual void OnRender(void);
    virtual void OnFrame(void);

    virtual void Initialize();
    virtual void Destroy();
};

ENGINE_API extern CConsole* Console;
