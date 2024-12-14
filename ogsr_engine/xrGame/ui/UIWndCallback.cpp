#include "stdafx.h"
#include "UIWndCallback.h"
#include "UIWindow.h"
#include "../object_broker.h"
#include "../callback_info.h"

struct event_comparer
{
    shared_str name;
    s16 event;

    event_comparer(shared_str n, s16 e) : name(n), event(e) {}
    bool operator()(SCallbackInfo& i) { return ((i.m_controlName == name) && (i.m_event == event)); }
};

void CUIWndCallback::Register(CUIWindow* pChild) { pChild->SetMessageTarget(smart_cast<CUIWindow*>(this)); }

void CUIWndCallback::OnEvent(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (!pWnd)
        return;
    event_comparer ec(pWnd->WindowName(), msg);

    CALLBACK_IT it = std::find_if(m_callbacks.begin(), m_callbacks.end(), ec);
    if (it == m_callbacks.end())
        return;

    (*it).m_callback();

    if ((*it).m_cpp_callback)
        (*it).m_cpp_callback(pWnd, pData);
}

SCallbackInfo* CUIWndCallback::NewCallback() { return &m_callbacks.emplace_back(); }

CUIWndCallback::~CUIWndCallback() { ClearCallbacks(); }

void CUIWndCallback::AddCallback(LPCSTR control_id, s16 event, const void_function& f)
{
    SCallbackInfo* c = NewCallback();
    c->m_cpp_callback = f;
    c->m_controlName = control_id;
    c->m_event = event;
}

void CUIWndCallback::ClearCallbacks()
{
    for (auto& cb : m_callbacks)
        cb.m_callback.clear();

    m_callbacks.clear();
}