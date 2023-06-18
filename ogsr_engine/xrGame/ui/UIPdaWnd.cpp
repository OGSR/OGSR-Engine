#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIInventoryUtilities.h"

#include "../HUDManager.h"
#include "../level.h"
#include "../game_cl_base.h"

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
//#include "UIPdaCommunication.h"
#include "UIPdaContactsWnd.h"
#include "UIMapWnd.h"
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"
#include "UIStalkersRankingWnd.h"
#include "UIActorInfo.h"
#include "UIEventsWnd.h"
#include "../object_broker.h"
#include "UIMessagesWindow.h"
#include "UIMainIngameWnd.h"
#include "UITabButton.h"
#include "../actor.h"
#include "UIPdaSpot.h"
#include "player_hud.h"
#include "../../xr_3da/XR_IOConsole.h"
#include "inventory.h"

#define PDA_XML "pda.xml"
u32 g_pda_info_state = 0;

void RearrangeTabButtons(CUITabControl* pTab, xr_vector<Fvector2>& vec_sign_places);

CUIPdaWnd::CUIPdaWnd()
{
    UIMapWnd = NULL;
    UIPdaContactsWnd = NULL;
    UIEncyclopediaWnd = NULL;
    UIDiaryWnd = NULL;
    UIActorInfo = NULL;
    UIStalkersRanking = NULL;
    UIEventsWnd = NULL;
    m_updatedSectionImage = NULL;
    m_oldSectionImage = NULL;

    last_cursor_pos.set(UI_BASE_WIDTH / 2.f, UI_BASE_HEIGHT / 2.f);

    Init();
}

CUIPdaWnd::~CUIPdaWnd()
{
    delete_data(UIMapWnd);
    delete_data(UIPdaContactsWnd);
    delete_data(UIEncyclopediaWnd);
    delete_data(UIDiaryWnd);
    delete_data(UIActorInfo);
    delete_data(UIStalkersRanking);
    delete_data(UIEventsWnd);
    delete_data(m_updatedSectionImage);
    delete_data(m_oldSectionImage);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Init()
{
    CUIXml uiXml;
    bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, PDA_XML);
    R_ASSERT3(xml_result, "xml file not found", PDA_XML);

    CUIXmlInit xml_init;

    m_pActiveDialog = NULL;

    xml_init.InitWindow(uiXml, "main", 0, this);

    UIMainPdaFrame = xr_new<CUIStatic>();
    UIMainPdaFrame->SetAutoDelete(true);
    AttachChild(UIMainPdaFrame);
    xml_init.InitStatic(uiXml, "background_static", 0, UIMainPdaFrame);

    //Элементы автоматического добавления
    xml_init.InitAutoStatic(uiXml, "auto_static", this);

    // Main buttons background
    UIMainButtonsBackground = xr_new<CUIFrameLineWnd>();
    UIMainButtonsBackground->SetAutoDelete(true);
    UIMainPdaFrame->AttachChild(UIMainButtonsBackground);
    xml_init.InitFrameLine(uiXml, "mbbackground_frame_line", 0, UIMainButtonsBackground);

    // Timer background
    UITimerBackground = xr_new<CUIFrameLineWnd>();
    UITimerBackground->SetAutoDelete(true);
    UIMainPdaFrame->AttachChild(UITimerBackground);
    xml_init.InitFrameLine(uiXml, "timer_frame_line", 0, UITimerBackground);

    // Oкно карты
    UIMapWnd = xr_new<CUIMapWnd>();
    UIMapWnd->Init("pda_map.xml", "map_wnd");

    // Oкно коммуникaции
    UIPdaContactsWnd = xr_new<CUIPdaContactsWnd>();
    UIPdaContactsWnd->Init();

    // Oкно новостей
    UIDiaryWnd = xr_new<CUIDiaryWnd>();
    UIDiaryWnd->Init();

    // Окно энциклопедии
    UIEncyclopediaWnd = xr_new<CUIEncyclopediaWnd>();
    UIEncyclopediaWnd->Init();

    // Окно статистики о актере
    UIActorInfo = xr_new<CUIActorInfoWnd>();
    UIActorInfo->Init();

    // Окно рейтинга сталкеров
    UIStalkersRanking = xr_new<CUIStalkersRankingWnd>();
    UIStalkersRanking->Init();

    UIEventsWnd = xr_new<CUIEventsWnd>();
    UIEventsWnd->Init();
    // Tab control
    UITabControl = xr_new<CUITabControl>();
    UITabControl->SetAutoDelete(true);
    UIMainPdaFrame->AttachChild(UITabControl);
    xml_init.InitTabControl(uiXml, "tab", 0, UITabControl);
    UITabControl->SetMessageTarget(this);

    m_updatedSectionImage = xr_new<CUIStatic>();
    xml_init.InitStatic(uiXml, "updated_section_static", 0, m_updatedSectionImage);

    m_oldSectionImage = xr_new<CUIStatic>();
    xml_init.InitStatic(uiXml, "old_section_static", 0, m_oldSectionImage);

    m_pActiveSection = eptNoActiveTab;

    RearrangeTabButtons(UITabControl, m_sign_places_main);
}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (pWnd == UITabControl)
    {
        if (TAB_CHANGED == msg)
        {
            SetActiveSubdialog((EPdaTabs)UITabControl->GetActiveIndex());
        }
    }
    else
    {
        R_ASSERT(m_pActiveDialog);
        m_pActiveDialog->SendMessage(pWnd, msg, pData);
    }
}

bool CUIPdaWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
    switch (mouse_action)
    {
    case WINDOW_LBUTTON_DOWN:
    // case WINDOW_RBUTTON_DOWN:
    case WINDOW_LBUTTON_UP:
        // case WINDOW_RBUTTON_UP:
        {
            if (auto pda = Actor()->GetPDA())
            {
                if (pda->IsPending())
                    return true;

                if (mouse_action == WINDOW_LBUTTON_DOWN)
                    bButtonL = true;
                // else if (mouse_action == WINDOW_RBUTTON_DOWN)
                //	bButtonR = true;
                else if (mouse_action == WINDOW_LBUTTON_UP)
                    bButtonL = false;
                // else if (mouse_action == WINDOW_RBUTTON_UP)
                //	bButtonR = false;
            }
            break;
        }
    case WINDOW_RBUTTON_DOWN:
        if (auto pda = Actor()->GetPDA(); pda && pda->Is3DPDA() && psActorFlags.test(AF_3D_PDA))
        {
            pda->m_bZoomed = false;
            HUD().GetUI()->SetMainInputReceiver(nullptr, false);
            return true;
        }
        break;
    }

    CUIDialogWnd::OnMouse(x, y, mouse_action);
    return true; // always true because StopAnyMove() == false
}

void CUIPdaWnd::MouseMovement(float x, float y)
{
    CPda* pda = Actor()->GetPDA();
    if (!pda)
        return;

    if (pda->ThumbAnimsAllowed())
    {
        if (!pda->IsZoomed() || pda->IsPending())
            return;

        static u32 DeltaUpdTG{}, SavedClickTime{};

        if (Device.dwTimeGlobal < DeltaUpdTG)
            return;

        const xr_string last_thumb_anim_name = pda->thumb_anim_name;

        constexpr float PDA_CURSOR_MOVE_TREASURE = 2.f;
        if (std::abs(x) < PDA_CURSOR_MOVE_TREASURE && std::abs(y) < PDA_CURSOR_MOVE_TREASURE)
        {
            pda->thumb_anim_name = (pda->thumb_anim_name.empty() && SavedClickTime != m_dwLastClickTime) ? "_click" : "";
        }
        else
        {
            const float gyp = std::sqrt(x * x + y * y);
            float angle = std::asin(std::clamp(y / gyp, -1.0f, 1.0f));
            if (negative(x))
                angle = M_PI - angle;
            if (negative(angle))
                angle = angle + 2.0f * M_PI;

            if (angle >= 0.393f && angle < 1.18f)
                pda->thumb_anim_name = "_down_right";
            else if (angle >= 1.18f && angle < 1.96f)
                pda->thumb_anim_name = "_down";
            else if (angle >= 1.96f && angle < 2.74f)
                pda->thumb_anim_name = "_down_left";
            else if (angle >= 2.74f && angle < 3.53f)
                pda->thumb_anim_name = "_left";
            else if (angle >= 3.53f && angle < 4.32f)
                pda->thumb_anim_name = "_up_left";
            else if (angle >= 4.32f && angle < 5.10f)
                pda->thumb_anim_name = "_up";
            else if (angle >= 5.10f && angle < 5.89f)
                pda->thumb_anim_name = "_up_right";
            else
                pda->thumb_anim_name = "_right";
        }

        if (last_thumb_anim_name == pda->thumb_anim_name)
            return;

        DeltaUpdTG = Device.dwTimeGlobal + 117;
        SavedClickTime = m_dwLastClickTime;

        pda->PlayAnimIdle();
        return;
    }

    x *= .1f;
    y *= .1f;
    clamp(x, -.15f, .15f);
    clamp(y, -.15f, .15f);

    if (_abs(x) < .05f)
        x = 0.f;

    if (_abs(y) < .05f)
        y = 0.f;

    bool buttonpressed = (bButtonL || bButtonR);

    target_buttonpress = (buttonpressed ? -.0015f : 0.f);
    target_joystickrot.set(x * -.75f, 0.f, y * .75f);

    x += y * pda->m_thumb_rot[0];
    y += x * pda->m_thumb_rot[1];

    g_player_hud->target_thumb0rot.set(y * .15f, y * -.05f, (x * -.15f) + (buttonpressed ? .002f : 0.f));
    g_player_hud->target_thumb01rot.set(0.f, 0.f, (x * -.25f) + (buttonpressed ? .01f : 0.f));
    g_player_hud->target_thumb02rot.set(0.f, 0.f, (x * .75f) + (buttonpressed ? .025f : 0.f));
}

void CUIPdaWnd::Show()
{
    if (Core.Features.test(xrCore::Feature::more_hide_weapon))
        Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL, true);

    InventoryUtilities::SendInfoToActor("ui_pda");

    inherited::Show();
}

void CUIPdaWnd::Hide()
{
    inherited::Hide();

    InventoryUtilities::SendInfoToActor("ui_pda_hide");
    HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, false);

    if (Core.Features.test(xrCore::Feature::more_hide_weapon))
        Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL, false);
}

void CUIPdaWnd::UpdateDateTime()
{
    static shared_str prevStrTime = " ";
    xr_string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
    strTime += " ";
    strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

    if (xr_strcmp(strTime.c_str(), prevStrTime))
    {
        UITimerBackground->UITitleText.SetText(strTime.c_str());
        prevStrTime = strTime.c_str();
    }
}

void CUIPdaWnd::Update()
{
    inherited::Update();
    UpdateDateTime();
}

void CUIPdaWnd::SetActiveSubdialog(EPdaTabs section)
{
    if (m_pActiveSection == section)
        return;

    if (m_pActiveDialog)
    {
        UIMainPdaFrame->DetachChild(m_pActiveDialog);
        m_pActiveDialog->Show(false);
    }

    switch (section)
    {
    case eptDiary:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIDiaryWnd);
        InventoryUtilities::SendInfoToActor("ui_pda_events");
        g_pda_info_state &= ~pda_section::diary;
        break;
    case eptContacts:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIPdaContactsWnd);
        InventoryUtilities::SendInfoToActor("ui_pda_contacts");
        g_pda_info_state &= ~pda_section::contacts;
        break;
    case eptMap:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIMapWnd);
        g_pda_info_state &= ~pda_section::map;
        break;
    case eptEncyclopedia:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIEncyclopediaWnd);
        InventoryUtilities::SendInfoToActor("ui_pda_encyclopedia");
        g_pda_info_state &= ~pda_section::encyclopedia;
        break;
    case eptActorStatistic:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIActorInfo);
        InventoryUtilities::SendInfoToActor("ui_pda_actor_info");
        g_pda_info_state &= ~pda_section::statistics;
        break;
    case eptRanking:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIStalkersRanking);
        g_pda_info_state &= ~pda_section::ranking;
        InventoryUtilities::SendInfoToActor("ui_pda_ranking");
        break;
    case eptQuests:
        m_pActiveDialog = smart_cast<CUIWindow*>(UIEventsWnd);
        g_pda_info_state &= ~pda_section::quests;
        break;
    default: Msg("not registered button identifier [%d]", UITabControl->GetActiveIndex());
    }
    UIMainPdaFrame->AttachChild(m_pActiveDialog);
    m_pActiveDialog->Show(true);

    if (UITabControl->GetActiveIndex() != section)
        UITabControl->SetNewActiveTab(section);

    m_pActiveSection = section;
}

void CUIPdaWnd::Draw()
{
    static u32 last_frame{};
    if (last_frame == Device.dwFrame)
        return;
    last_frame = Device.dwFrame;

    inherited::Draw();
    DrawUpdatedSections();
}

void CUIPdaWnd::PdaContentsChanged(pda_section::part type, bool flash)
{
    if (type == pda_section::encyclopedia)
    {
        UIEncyclopediaWnd->ReloadArticles();
    }
    else if (type == pda_section::news)
    {
        UIDiaryWnd->AddNews();
        UIDiaryWnd->MarkNewsAsRead(UIDiaryWnd->IsShown());
    }
    else if (type == pda_section::quests)
    {
        UIEventsWnd->Reload();
    }
    else if (type == pda_section::contacts)
    {
        UIPdaContactsWnd->Reload();
        flash = false;
    }

    else if (type == pda_section::journal || type == pda_section::info)
        UIDiaryWnd->ReloadJournal();

    if (flash)
    {
        g_pda_info_state |= type;
        HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, true);
    }
}
void draw_sign(CUIStatic* s, Fvector2& pos)
{
    s->SetWndPos(pos);
    s->Draw();
}

void CUIPdaWnd::DrawUpdatedSections()
{
    m_updatedSectionImage->Update();
    m_oldSectionImage->Update();

    Fvector2 tab_pos;
    UITabControl->GetAbsolutePos(tab_pos);

    Fvector2 pos;

    pos = m_sign_places_main[eptQuests];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::quests)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptMap];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::map)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptDiary];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::diary)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptContacts];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::contacts)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptRanking];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::ranking)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptActorStatistic];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::statistics)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);

    pos = m_sign_places_main[eptEncyclopedia];
    pos.add(tab_pos);
    if (g_pda_info_state & pda_section::encyclopedia)
        draw_sign(m_updatedSectionImage, pos);
    else
        draw_sign(m_oldSectionImage, pos);
}

void CUIPdaWnd::Reset()
{
    inherited::Reset();
    if (UIMapWnd)
        UIMapWnd->Reset();
    if (UIPdaContactsWnd)
        UIPdaContactsWnd->Reset();
    if (UIEncyclopediaWnd)
        UIEncyclopediaWnd->Reset();
    if (UIDiaryWnd)
        UIDiaryWnd->Reset();
    if (UIActorInfo)
        UIActorInfo->Reset();
    if (UIStalkersRanking)
        UIStalkersRanking->Reset();
    if (UIEventsWnd)
        UIEventsWnd->Reset();
}

void RearrangeTabButtons(CUITabControl* pTab, xr_vector<Fvector2>& vec_sign_places)
{
    TABS_VECTOR* btn_vec = pTab->GetButtonsVector();
    TABS_VECTOR::iterator it = btn_vec->begin();
    TABS_VECTOR::iterator it_e = btn_vec->end();
    vec_sign_places.clear();
    vec_sign_places.resize(btn_vec->size());

    Fvector2 pos;
    pos.set((*it)->GetWndPos());
    Fvector2 sign_sz;
    sign_sz.set(9.0f + 3.0f, 11.0f);
    u32 idx = 0;
    float btn_text_len = 0.0f;
    CUIStatic* st = NULL;

    for (; it != it_e; ++it, ++idx)
    {
        if (idx != 0)
        {
            st = xr_new<CUIStatic>();
            st->SetAutoDelete(true);
            pTab->AttachChild(st);
            st->SetFont((*it)->GetFont());
            st->SetTextColor(color_rgba(90, 90, 90, 255));
            st->SetText("//");
            st->SetWndSize((*it)->GetWndSize());
            st->AdjustWidthToText();
            st->SetWndPos(pos);
            pos.x += st->GetWndSize().x;
        }

        vec_sign_places[idx].set(pos);
        vec_sign_places[idx].y += iFloor(((*it)->GetWndSize().y - sign_sz.y) / 2.0f);
        vec_sign_places[idx].y = (float)iFloor(vec_sign_places[idx].y);
        pos.x += sign_sz.x;

        (*it)->SetWndPos(pos);
        (*it)->AdjustWidthToText();
        btn_text_len = (*it)->GetWndSize().x;
        pos.x += btn_text_len + 3.0f;
    }
}
bool CUIPdaWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
    if (WINDOW_KEY_PRESSED == keyboard_action && IsShown())
    {
        if (psActorFlags.test(AF_3D_PDA))
        {
            CPda* pda = Actor()->GetPDA();
            if (pda && pda->Is3DPDA())
            {
                EGameActions action = get_binded_action(dik);

                if (action == kQUIT) // "Hack" to make Esc key open main menu instead of simply hiding the PDA UI
                {
                    if (pda->GetState() == CPda::eHiding || pda->GetState() == CPda::eHidden)
                    {
                        Console->Execute("main_menu");
                        return false;
                    }
                    else if (pda->m_bZoomed)
                    {
                        pda->m_bZoomed = false;
                        HUD().GetUI()->SetMainInputReceiver(nullptr, false);
                        return true;
                    }
                    else
                    {
                        Actor()->inventory().Activate(NO_ACTIVE_SLOT);
                        return true;
                    }
                }
            }
        }
    }
    return inherited::OnKeyboard(dik, keyboard_action);
}
