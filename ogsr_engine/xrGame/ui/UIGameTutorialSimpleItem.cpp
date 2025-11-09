#include "stdafx.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "..\..\xr_3da\xr_input.h"
#include "../xr_level_controller.h"
#include "../UIGameSp.h"
#include "../HUDManager.h"
#include "../level.h"
#include "UIPdaWnd.h"
#include "UIInventoryWnd.h"
#include "UITalkWnd.h"
#include "UICarBodyWnd.h"

extern ENGINE_API BOOL bShowPauseString;

//-----------------------------------------------------------------------------
// Tutorial Item
//-----------------------------------------------------------------------------
CUISequenceSimpleItem::~CUISequenceSimpleItem()
{
    SubItemVecIt _I = m_subitems.begin();
    SubItemVecIt _E = m_subitems.end();
    for (; _I != _E; ++_I)
        _I->Stop();
    m_subitems.clear();
    m_sound.stop();
    delete_data(m_UIWindow);
}

bool CUISequenceSimpleItem::IsPlaying() { return (m_time_start + m_time_length) > (Device.dwTimeContinual / 1000.0f); }

CUIWindow* find_child_window(CUIWindow* parent, const shared_str& _name)
{
    auto& wl = parent->GetChildWndList();
    auto _I = wl.begin();
    auto _E = wl.end();
    for (; _I != _E; ++_I)
        if ((*_I)->WindowName() == _name)
            return (*_I);
    return NULL;
}

void CUISequenceSimpleItem::Load(CUIXml* xml, int idx)
{
    CUISequenceItem::Load(xml, idx);

    XML_NODE* _stored_root = xml->GetLocalRoot();
    xml->SetLocalRoot(xml->NavigateToNode("item", idx));

    LPCSTR m_snd_name = xml->Read("sound", 0, "");
    if (m_snd_name && m_snd_name[0])
    {
        m_sound.create(m_snd_name, st_Effect, sg_Undefined);
        VERIFY(m_sound._handle());
    }

    m_time_length = xml->ReadFlt("length_sec", 0, 0);

    auto cur_pos = UI()->GetUICursor()->GetCursorPosition();

    m_desired_cursor_pos.x = xml->ReadAttribFlt("cursor_pos", 0, "x", cur_pos.x);
    m_desired_cursor_pos.y = xml->ReadAttribFlt("cursor_pos", 0, "y", cur_pos.y);

    strcpy_s(m_pda_section, xml->Read("pda_section", 0, ""));

    LPCSTR str = xml->Read("pause_state", 0, "ignore");
    m_flags.set(etiNeedPauseOn, 0 == _stricmp(str, "on"));
    m_flags.set(etiNeedPauseOff, 0 == _stricmp(str, "off"));

    LPCSTR str2 = xml->Read("pause_sound", 0, "ignore");
    m_flags.set(etiNeedPauseSound, 0 == _stricmp(str2, "on"));

    str = xml->Read("guard_key", 0, NULL);
    m_continue_dik_guard = -1;
    if (str && !_stricmp(str, "any"))
    {
        m_continue_dik_guard = 9999;
        str = NULL;
    }
    if (str)
    {
        EGameActions cmd = action_name_to_id(str);
        m_continue_dik_guard = get_action_dik(cmd);
    }

    m_flags.set(etiCanBeStopped, (m_continue_dik_guard == -1));
    m_flags.set(etiGrabInput, 1 == xml->ReadInt("grab_input", 0, 1));

    int actions_count = xml->GetNodesNum(0, 0, "action");
    m_actions.resize(actions_count);
    for (int idx = 0; idx < actions_count; ++idx)
    {
        auto& itm = m_actions[idx];
        LPCSTR str = xml->ReadAttrib("action", idx, "id");
        itm.m_action = action_name_to_id(str);
        itm.m_bfinalize = !!xml->ReadAttribInt("action", idx, "finalize", FALSE);
        itm.m_functor = xml->Read(xml->GetLocalRoot(), "action", idx, "");
    }

    // ui-components
    m_UIWindow = xr_new<CUIWindow>();
    m_UIWindow->SetAutoDelete(false);
    XML_NODE* _lsr = xml->GetLocalRoot();
    CUIXmlInit xml_init;
    xml_init.InitWindow(*xml, "main_wnd", 0, m_UIWindow);
    //.	xml_init.InitAutoStaticGroup	(*xml, "main_wnd",		m_UIWindow);
    xml->SetLocalRoot(_lsr);

    // initialize auto_static
    int cnt = xml->GetNodesNum("main_wnd", 0, "auto_static");
    m_subitems.resize(cnt);
    string64 sname;
    for (int i = 0; i < cnt; ++i)
    {
        XML_NODE* _sr = xml->GetLocalRoot();
        xml->SetLocalRoot(xml->NavigateToNode("main_wnd", 0));

        sprintf_s(sname, "auto_static_%d", i);

        SSubItem* _si = &m_subitems[i];
        _si->m_start = xml->ReadAttribFlt("auto_static", i, "start_time", 0);
        _si->m_length = xml->ReadAttribFlt("auto_static", i, "length_sec", 0);
        _si->m_visible = false;
        _si->m_wnd = smart_cast<CUIStatic*>(find_child_window(m_UIWindow, sname));
        VERIFY(_si->m_wnd);
        _si->m_wnd->SetTextComplexMode(true);
        _si->m_wnd->Show(false);
        //	_si->m_wnd->SetWidth(_si->m_wnd->GetWidth()*UI()->get_current_kx()); // KRodin: этот код вызывает кучу проблем с положением текстур, не раскомментировать!

        xml->SetLocalRoot(_sr);
    }
    xml->SetLocalRoot(_stored_root);
}

void CUISequenceSimpleItem::SSubItem::Start()
{
    m_wnd->Show(true);
    m_wnd->ResetClrAnimation();
    m_visible = true;
}

void CUISequenceSimpleItem::SSubItem::Stop()
{
    m_wnd->Show(false);
    m_visible = false;
}

void CUISequenceSimpleItem::Update()
{
    float gt = float(Device.dwTimeContinual) / 1000.0f;
    SubItemVecIt _I = m_subitems.begin();
    SubItemVecIt _E = m_subitems.end();
    for (; _I != _E; ++_I)
    {
        SSubItem& s = *_I;
        bool bPlaying = (gt > (m_time_start + s.m_start)) && (gt < (m_time_start + s.m_start + s.m_length));
        if (true == bPlaying && (false == s.m_visible))
            s.Start();
        else if ((false == bPlaying) && (true == s.m_visible))
            s.Stop();
    }

    if (g_pGameLevel)
    {
        CUIGameSP* ui_game_sp = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());

        if (ui_game_sp)
        {
            if (!m_pda_section || 0 == xr_strlen(m_pda_section))
                if (ui_game_sp->PdaMenu->IsShown() || ui_game_sp->InventoryMenu->IsShown() || ui_game_sp->TalkMenu->IsShown() || ui_game_sp->UICarBodyMenu->IsShown() ||
                    ui_game_sp->UIChangeLevelWnd->IsShown())
                    m_UIWindow->Show(false);
                else
                    m_UIWindow->Show(true);
        }
    }
}

void CUISequenceSimpleItem::Start()
{
    inherited::Start();
    m_flags.set(etiStoredPauseState, Device.Paused());

    if (m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
    {
        Device.Pause(TRUE, TRUE, FALSE, "simpleitem_start");
        bShowPauseString = FALSE;
    }

    if (m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
        Device.Pause(FALSE, TRUE, FALSE, "simpleitem_start");

    if (m_flags.test(etiNeedPauseSound))
        Device.Pause(TRUE, FALSE, TRUE, "simpleitem_start");

    GetUICursor()->SetUICursorPosition(m_desired_cursor_pos);
    m_time_start = float(Device.dwTimeContinual) / 1000.0f;
    m_owner->MainWnd()->AttachChild(m_UIWindow);

    if (m_sound._handle())
        m_sound.play(NULL, sm_2D);

    if (g_pGameLevel)
    {
        bool bShowPda = false;
        CUIGameSP* ui_game_sp = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
        if (!_stricmp(m_pda_section, "pda_contacts"))
        {
            ui_game_sp->PdaMenu->SetActiveSubdialog(eptContacts);
            bShowPda = true;
        }
        else
        {
            if (!_stricmp(m_pda_section, "pda_map"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptMap);
                bShowPda = true;
            }
            else if (!_stricmp(m_pda_section, "pda_quests"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptQuests);
                bShowPda = true;
            }
            else if (!_stricmp(m_pda_section, "pda_diary"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptDiary);
                bShowPda = true;
            }
            else if (!_stricmp(m_pda_section, "pda_ranking"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptRanking);
                bShowPda = true;
            }
            else if (!_stricmp(m_pda_section, "pda_statistics"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptActorStatistic);
                bShowPda = true;
            }
            else if (!_stricmp(m_pda_section, "pda_encyclopedia"))
            {
                ui_game_sp->PdaMenu->SetActiveSubdialog(eptEncyclopedia); //-V595
                bShowPda = true;
            }
        }
        if (ui_game_sp)
        {
            if ((!ui_game_sp->PdaMenu->IsShown() && bShowPda) || (ui_game_sp->PdaMenu->IsShown() && !bShowPda))
                HUD().GetUI()->StartStopMenu(ui_game_sp->PdaMenu, true);
        }
    }
}

bool CUISequenceSimpleItem::Stop(bool bForce)
{
    if (!m_flags.test(etiCanBeStopped) && !bForce)
        return false;

    m_owner->MainWnd()->DetachChild(m_UIWindow);
    m_sound.stop();

    if (m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
        Device.Pause(FALSE, TRUE, FALSE, "simpleitem_stop");

    if (m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
        Device.Pause(TRUE, TRUE, FALSE, "simpleitem_stop");

    if (m_flags.test(etiNeedPauseSound))
        Device.Pause(FALSE, FALSE, TRUE, "simpleitem_stop");

    if (g_pGameLevel)
    {
        CUIGameSP* ui_game_sp = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
        if (ui_game_sp && ui_game_sp->PdaMenu->IsShown())
            HUD().GetUI()->StartStopMenu(ui_game_sp->PdaMenu, true);
    }
    inherited::Stop();
    return true;
}

void CUISequenceSimpleItem::OnKeyboardPress(int dik)
{
    if (!m_flags.test(etiCanBeStopped))
    {
        VERIFY(m_continue_dik_guard != -1);
        if (m_continue_dik_guard == -1)
            m_flags.set(etiCanBeStopped, TRUE); // not binded action :(

        if (m_continue_dik_guard == 9999 || dik == m_continue_dik_guard)
            m_flags.set(etiCanBeStopped, TRUE); // match key
    }

    for (const auto& itm : m_actions)
    {
        if (is_binded(itm.m_action, dik))
        {
            luabind::functor<void> functor_to_call;
            const bool functor_exists = ai().script_engine().functor(itm.m_functor.c_str(), functor_to_call);
            ASSERT_FMT_DBG(functor_exists, "!![%s] Cannot find script function described in tutorial item [%s]", __FUNCTION__, itm.m_functor.c_str());
            if (functor_exists)
                functor_to_call();

            if (itm.m_bfinalize)
            {
                m_flags.set(etiCanBeStopped, TRUE);
                m_stop_lua_functions.clear();
                Stop();
            }
        }
    }
}
