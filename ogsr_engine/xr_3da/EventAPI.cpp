#include "stdafx.h"
#include "eventapi.h"
#include "xr_ioconsole.h"

//---------------------------------------------------------------------
class ENGINE_API CEvent
{
    friend class CEventAPI;

private:
    char* Name;
    xr_vector<IEventReceiver*> Handlers;
    u32 dwRefCount;

public:
    CEvent(const char* S);
    ~CEvent();

    LPCSTR GetFull() { return Name; }
    u32 RefCount() { return dwRefCount; }

    BOOL Equal(CEvent& E) { return _stricmp(Name, E.Name) == 0; }

    void Attach(IEventReceiver* H)
    {
        if (std::find(Handlers.begin(), Handlers.end(), H) == Handlers.end())
            Handlers.push_back(H);
    }
    void Detach(IEventReceiver* H)
    {
        xr_vector<IEventReceiver*>::iterator I = std::find(Handlers.begin(), Handlers.end(), H);
        if (I != Handlers.end())
            Handlers.erase(I);
    }
    void Signal(u64 P1, u64 P2)
    {
        for (auto& Handler : Handlers)
            Handler->OnEvent(this, P1, P2);
    }
};
//-----------------------------------------
CEvent::CEvent(const char* S)
{
    Name = xr_strdup(S);
    _strupr(Name);
    dwRefCount = 1;
}
CEvent::~CEvent() { xr_free(Name); }

//-----------------------------------------
IC bool ev_sort(CEvent* E1, CEvent* E2) { return E1->GetFull() < E2->GetFull(); }

void CEventAPI::Dump()
{
    std::sort(Events.begin(), Events.end(), ev_sort);
    for (auto& Event : Events)
        Msg("* [%d] %s", Event->RefCount(), Event->GetFull());
}

EVENT CEventAPI::Create(const char* N)
{
    CS.Enter();
    CEvent E(N);
    for (auto& Event : Events)
    {
        if (Event->Equal(E))
        {
            EVENT F = Event;
            F->dwRefCount++;
            CS.Leave();
            return F;
        }
    }

    EVENT X = xr_new<CEvent>(N);
    Events.push_back(X);
    CS.Leave();
    return X;
}
void CEventAPI::Destroy(EVENT& E)
{
    CS.Enter();
    E->dwRefCount--;
    if (E->dwRefCount == 0)
    {
        xr_vector<CEvent*>::iterator I = std::find(Events.begin(), Events.end(), E);
        R_ASSERT(I != Events.end());
        Events.erase(I);
        xr_delete(E);
    }
    CS.Leave();
}

EVENT CEventAPI::Handler_Attach(const char* N, IEventReceiver* H)
{
    CS.Enter();
    EVENT E = Create(N);
    E->Attach(H);
    CS.Leave();
    return E;
}

void CEventAPI::Handler_Detach(EVENT& E, IEventReceiver* H)
{
    if (nullptr == E)
        return;
    CS.Enter();
    E->Detach(H);
    Destroy(E);
    CS.Leave();
}
void CEventAPI::Signal(EVENT E, u64 P1, u64 P2)
{
    CS.Enter();
    E->Signal(P1, P2);
    CS.Leave();
}
void CEventAPI::Signal(LPCSTR N, u64 P1, u64 P2)
{
    CS.Enter();
    EVENT E = Create(N);
    Signal(E, P1, P2);
    Destroy(E);
    CS.Leave();
}
void CEventAPI::Defer(EVENT E, u64 P1, u64 P2)
{
    CS.Enter();
    E->dwRefCount++;
    Events_Deferred.emplace_back();
    Events_Deferred.back().E = E;
    Events_Deferred.back().P1 = P1;
    Events_Deferred.back().P2 = P2;
    CS.Leave();
}
void CEventAPI::Defer(LPCSTR N, u64 P1, u64 P2)
{
    CS.Enter();
    EVENT E = Create(N);
    Defer(E, P1, P2);
    Destroy(E);
    CS.Leave();
}

void CEventAPI::OnFrame()
{
    ZoneScoped;

    CS.Enter();
    if (Events_Deferred.empty())
    {
        CS.Leave();
        return;
    }
    for (auto& I : Events_Deferred)
    {
        Deferred& DEF = I;
        Signal(DEF.E, DEF.P1, DEF.P2);
        Destroy(I.E);
    }
    Events_Deferred.clear();
    CS.Leave();
}

BOOL CEventAPI::Peek(LPCSTR EName)
{
    CS.Enter();
    if (Events_Deferred.empty())
    {
        CS.Leave();
        return FALSE;
    }
    for (auto& DEF : Events_Deferred)
    {
        if (_stricmp(DEF.E->GetFull(), EName) == 0)
        {
            CS.Leave();
            return TRUE;
        }
    }
    CS.Leave();
    return FALSE;
}

void CEventAPI::Destroy()
{
    Dump();
    if (Events.empty())
        Events.clear();
    if (Events_Deferred.empty())
        Events_Deferred.clear();
}
