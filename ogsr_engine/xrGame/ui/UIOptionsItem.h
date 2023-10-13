#pragma once
#include "UIOptionsManager.h"

class CUIOptionsItem
{
    friend class CUIOptionsManager;

public:
    enum ESystemDepends
    {
        sdNothing,
        sdVidRestart,
        sdSndRestart,
        sdSystemRestart,
    };


    virtual ~CUIOptionsItem();
    virtual void Register(const char* entry, const char* group);
    void SetSystemDepends(ESystemDepends val) { m_dep = val; }

    const char* GetEntry() const { return m_entry.c_str(); }

    static CUIOptionsManager* GetOptionsManager() { return &m_optionsManager; }

protected:
    virtual void SetCurrentValue() = 0;
    virtual void SaveValue();

    virtual bool IsChanged() = 0;
    virtual void SeveBackUpValue(){};
    virtual void Undo() { SetCurrentValue(); };

    void SendMessage2Group(const char* group, const char* message);
    virtual void OnMessage(const char* message);

    // string
    LPCSTR GetOptStringValue();
    void SaveOptStringValue(const char* val);
    // integer
    void GetOptIntegerValue(int& val, int& min, int& max);
    void SaveOptIntegerValue(int val);
    // float
    void GetOptFloatValue(float& val, float& min, float& max);
    void SaveOptFloatValue(float val);
    // bool
    bool GetOptBoolValue();
    void SaveOptBoolValue(bool val);
    // token
    LPCSTR GetOptTokenValue();
    const xr_token* GetOptToken();
    void SaveOptTokenValue(const char* val);

    xr_string m_entry;
    ESystemDepends m_dep;

    static CUIOptionsManager m_optionsManager;
};
