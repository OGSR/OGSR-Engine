#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../HudManager.h"
#include "../object_broker.h"
#include "../callback_info.h"

struct event_comparer
{
    shared_str name;
    s16 event;

    event_comparer(shared_str n, s16 e) : name(n), event(e) {}
    bool operator()(SCallbackInfo& i) { return ((i.m_controlName == name) && (i.m_event == event)); }
};

CUIDialogWndEx::CUIDialogWndEx() : inherited() { Hide(); }

CUIDialogWndEx::~CUIDialogWndEx() { ClearCallbacks(); }

void CUIDialogWndEx::Register(CUIWindow* pChild) { pChild->SetMessageTarget(this); }

void CUIDialogWndEx::Register(CUIWindow* pChild, LPCSTR name)
{
    pChild->SetWindowName(name);
    pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    event_comparer ec(pWnd->WindowName(), msg);

    CALLBACK_IT it = std::find_if(m_callbacks.begin(), m_callbacks.end(), ec);
    if (it == m_callbacks.end())
        return inherited::SendMessage(pWnd, msg, pData);

    it->m_callback();
}

bool CUIDialogWndEx::Load(LPCSTR xml_name) { return true; }

SCallbackInfo* CUIDialogWndEx::NewCallback() { return &m_callbacks.emplace_back(); }

void CUIDialogWndEx::AddCallback(LPCSTR control_id, s16 event, const luabind::functor<void>& lua_function)
{
    SCallbackInfo* c = NewCallback();
    c->m_callback.set(lua_function);
    c->m_controlName = control_id;
    c->m_event = event;
}

void CUIDialogWndEx::AddCallback(LPCSTR control_id, s16 event, const luabind::functor<void>& functor, const luabind::object& object)
{
    SCallbackInfo* c = NewCallback();
    c->m_callback.set(functor, object);
    c->m_controlName = control_id;
    c->m_event = event;
}

bool CUIDialogWndEx::OnKeyboard(int dik, EUIMessages keyboard_action) { return inherited::OnKeyboard(dik, keyboard_action); }
void CUIDialogWndEx::Update() { inherited::Update(); }

void CUIDialogWndEx::ClearCallbacks()
{
    for (auto& cb : m_callbacks)
        cb.m_callback.clear();

    m_callbacks.clear();
}
