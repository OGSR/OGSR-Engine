#include "stdafx.h"
#include "HUDManager.h"
#include "hudtarget.h"

#include "actor.h"
#include "..\xr_3da\igame_level.h"
#include "clsid_game.h"
#include "Car.h"
#include "ui/UIMessagesWindow.h"

CFontManager::CFontManager()
{
    Device.seqDeviceReset.Add(this, REG_PRIORITY_HIGH);

    InitializeFonts();
}

void CFontManager::InitializeFonts()
{
    //pFontMedium = nullptr;
    //pFontSmall = nullptr;

    //pFontDI = nullptr;

    //pFontArial14 = nullptr;
    //pFontArial21 = nullptr;
    //pFontGraffiti19Russian = nullptr;
    //pFontGraffiti22Russian = nullptr;
    //pFontLetterica16Russian = nullptr;
    //pFontLetterica18Russian = nullptr;
    //pFontGraffiti32Russian = nullptr;
    //pFontGraffiti40Russian = nullptr;
    //pFontGraffiti50Russian = nullptr;
    //pFontLetterica25 = nullptr;

    //delete_data(m_all_fonts);

    //m_all_fonts.clear();

    InitializeFont(pFontMedium, "hud_font_medium");
    InitializeFont(pFontSmall, "hud_font_small");

    InitializeFont(pFontDI, "stat_font", CGameFont::fsGradient | CGameFont::fsDeviceIndependent);

    InitializeFont(pFontArial14, "ui_font_arial_14");
    InitializeFont(pFontArial21, "ui_font_arial_21");

    InitializeFont(pFontGraffiti19Russian, "ui_font_graffiti19_russian");
    InitializeFont(pFontGraffiti22Russian, "ui_font_graffiti22_russian");

    InitializeFont(pFontLetterica16Russian, "ui_font_letterica16_russian");
    InitializeFont(pFontLetterica18Russian, "ui_font_letterica18_russian");

    InitializeFont(pFontGraffiti32Russian, "ui_font_graff_32");
    InitializeFont(pFontGraffiti40Russian, "ui_font_graff_40");
    InitializeFont(pFontGraffiti50Russian, "ui_font_graff_50");

    InitializeFont(pFontLetterica25, "ui_font_letter_25");

    for (auto& font : m_all_fonts)
    {
        if (font->m_bCustom)
            InitializeFont(font, font->m_font_name.c_str());
    }
}

LPCSTR CFontManager::GetFontTexName(LPCSTR section)
{
    constexpr const char* tex_names[] = {"texture800", "texture", "texture1600"};
    int def_idx = 1; // default 1024x768
    int idx = def_idx;

    u32 h = Device.dwHeight;

    if (h <= 600)
        idx = 0;
    else if (h < 1024)
        idx = 1;
    else
        idx = 2;

    while (idx >= 0)
    {
        if (pSettings->line_exist(section, tex_names[idx]))
            return pSettings->r_string(section, tex_names[idx]);
        --idx;
    }
    return pSettings->r_string(section, tex_names[def_idx]);
}

void CFontManager::InitializeFont(CGameFont*& F, LPCSTR section, u32 flags)
{
    LPCSTR font_tex_name = GetFontTexName(section);
    R_ASSERT(font_tex_name);

    const char* sh_name = READ_IF_EXISTS(pSettings, r_string, section, "shader", "font");
    if (!F)
    {
        F = xr_new<CGameFont>(sh_name, font_tex_name, section, flags);
        m_all_fonts.push_back(F);
    }
    else
        F->Initialize(sh_name, font_tex_name, section);

    F->m_font_name = section;

    if (!(flags & CGameFont::fsDeviceIndependent))
    {
        if (pSettings->line_exist(section, "scale_x"))
        {
            F->SetWidthScale(pSettings->r_float(section, "scale_x"));
        }
        if (pSettings->line_exist(section, "scale_y"))
        {
            F->SetHeightScale(pSettings->r_float(section, "scale_y"));
        }
    }

    if (pSettings->line_exist(section, "size"))
    {
        float sz = pSettings->r_float(section, "size");
        if (flags & CGameFont::fsDeviceIndependent)
            F->SetHeightI(sz);
        else
            F->SetHeight(sz);
    }
    if (pSettings->line_exist(section, "interval"))
        F->SetInterval(pSettings->r_fvector2(section, "interval"));
}

CGameFont* CFontManager::InitializeCustomFont(LPCSTR section, u32 flags)
{
    for (auto& font : m_all_fonts)
    {
        if (strstr(font->m_font_name.c_str(), section))
            return font;
    }

    CGameFont* pFontAdd = NULL;
    InitializeFont(pFontAdd, section, flags);
    pFontAdd->m_bCustom = true;
    return pFontAdd;
}

CFontManager::~CFontManager()
{
    Device.seqDeviceReset.Remove(this);

    pFontMedium = nullptr;
    pFontSmall = nullptr;

    pFontDI = nullptr;

    pFontArial14 = nullptr;
    pFontArial21 = nullptr;
    pFontGraffiti19Russian = nullptr;
    pFontGraffiti22Russian = nullptr;
    pFontLetterica16Russian = nullptr;
    pFontLetterica18Russian = nullptr;
    pFontGraffiti32Russian = nullptr;
    pFontGraffiti40Russian = nullptr;
    pFontGraffiti50Russian = nullptr;
    pFontLetterica25 = nullptr;

    delete_data(m_all_fonts);

    m_all_fonts.clear();
}

void CFontManager::Render()
{
    for (auto& font : m_all_fonts)
    {
        font->OnRender();
    }
}

void CFontManager::OnDeviceReset() { InitializeFonts(); }

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{
    pUI = 0;
    m_pHUDTarget = xr_new<CHUDTarget>();
    OnDisconnected();
}
//--------------------------------------------------------------------
CHUDManager::~CHUDManager()
{
    xr_delete(pUI);
    xr_delete(m_pHUDTarget);
    b_online = false;
}

//--------------------------------------------------------------------

void CHUDManager::Load()
{
    if (pUI)
    {
        pUI->Load(pUI->UIGame());
        return;
    }
    pUI = xr_new<CUI>(this);
    pUI->Load(NULL);
    OnDisconnected();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
    if (!b_online)
        return;
    if (pUI)
        pUI->UIOnFrame();
    m_pHUDTarget->CursorOnFrame();
}
//--------------------------------------------------------------------

bool need_render_hud()
{
    CObject* O = (g_pGameLevel) ? g_pGameLevel->CurrentViewEntity() : nullptr;
    if (!O)
        return false;

    CActor* A = smart_cast<CActor*>(O);
    if (A && (!A->HUDview() || !A->g_Alive()))
        return false;

    if (smart_cast<CCar*>(O))
        return false;

    return true;
}

void CHUDManager::Render_First()
{
    if (!psHUD_Flags.is(HUD_WEAPON_RT))
        return;

    if (pUI == nullptr)
        return;

    if (!need_render_hud())
        return;

    // only shadow
    CObject* O = g_pGameLevel->CurrentViewEntity();
    ::Render->set_Invisible(TRUE);
    ::Render->set_Object(O->H_Root());
    O->renderable_Render();
    ::Render->set_Invisible(FALSE);
}

void CHUDManager::Render_Last()
{
    if (!psHUD_Flags.is(HUD_WEAPON_RT))
        return;
    if (0 == pUI)
        return;
    CObject* O = g_pGameLevel->CurrentViewEntity();
    if (0 == O)
        return;
    CActor* A = smart_cast<CActor*>(O);
    if (A && !A->HUDview())
        return;
    if (O->CLS_ID == CLSID_CAR)
        return;

    if (O->CLS_ID == CLSID_SPECTATOR)
        return;

    // hud itself
    ::Render->set_HUD(TRUE);
    ::Render->set_Object(O->H_Root());
    O->OnHUDDraw(this);
    ::Render->set_HUD(FALSE);
}

void CHUDManager::Render_Actor_Shadow() // added by KD
{
    if (0 == pUI)
        return;
    CObject* O = g_pGameLevel->CurrentViewEntity();
    if (0 == O)
        return;
    CActor* A = smart_cast<CActor*>(O);
    if (!A)
        return;
    if (A->active_cam() != eacFirstEye)
        return; // KD: we need to render actor shadow only in first eye cam mode because
                // in other modes actor model already in scene graph and renders well
    ::Render->set_Object(O->H_Root());
    O->renderable_Render();
}

extern void draw_wnds_rects();

extern ENGINE_API BOOL bShowPauseString;

//отрисовка элементов интерфейса
#include "string_table.h"

void CHUDManager::RenderUI()
{
    if (!b_online)
        return;

    if (HUD().GetUI()->GameIndicatorsShown() /*&& psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT)*/)
    {
        HitMarker.Render();

        if (psHUD_Flags.is(HUD_CROSSHAIR | HUD_CROSSHAIR_RT | HUD_CROSSHAIR_RT2))
            m_pHUDTarget->Render();
    }

    if (pUI)
        pUI->Render();

    Font().Render();


    if (Device.Paused() && bShowPauseString)
    {
        CGameFont* pFont = Font().pFontGraffiti50Russian;
        pFont->SetColor(0x80FF0000);
        LPCSTR _str = CStringTable().translate("st_game_paused").c_str();

        Fvector2 _pos;
        _pos.set(UI_BASE_WIDTH / 2.0f, UI_BASE_HEIGHT / 2.0f);
        UI()->ClientToScreenScaled(_pos);
        pFont->SetAligment(CGameFont::alCenter);
        pFont->Out(_pos.x, _pos.y, _str);
        pFont->OnRender();
    }
}

void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2) {}

collide::rq_result& CHUDManager::GetCurrentRayQuery() { return m_pHUDTarget->RQ; }

void CHUDManager::SetCrosshairDisp(float dispf, float disps) { m_pHUDTarget->HUDCrosshair.SetDispersion(psHUD_Flags.test(HUD_CROSSHAIR_DYNAMIC) ? dispf : disps); }

void CHUDManager::ShowCrosshair(bool show) { m_pHUDTarget->m_bShowCrosshair = show; }

void CHUDManager::Hit(int idx, float power, const Fvector& dir) { HitMarker.Hit(idx, dir); }

void CHUDManager::SetHitmarkType(LPCSTR tex_name) { HitMarker.InitShader(tex_name); }
#include "ui\UIMainInGameWnd.h"
void CHUDManager::OnScreenRatioChanged()
{
    xr_delete(pUI->UIMainIngameWnd);

    pUI->UIMainIngameWnd = xr_new<CUIMainIngameWnd>();
    pUI->UIMainIngameWnd->Init();
    pUI->UIMainIngameWnd->OnConnected(); //Перезагружаем здесь миникарту, чтобы она не пропадала при изменении разрешения экрана.
    pUI->UnLoad();
    pUI->Load(pUI->UIGame());
    if (auto act = Actor())
        act->UpdateArtefactPanel(); //Обновляем панель с артами на худе
}

void CHUDManager::OnDisconnected()
{
    //.	if(!b_online)			return;
    b_online = false;
    if (pUI)
        Device.seqFrame.Remove(pUI);
}

void CHUDManager::OnConnected()
{
    if (b_online)
        return;
    b_online = true;
    if (pUI)
    {
        Device.seqFrame.Add(pUI, REG_PRIORITY_LOW - 1000);
    }
}

void CHUDManager::net_Relcase(CObject* object)
{
    VERIFY(m_pHUDTarget);
    m_pHUDTarget->net_Relcase(object);
}

#include "player_hud.h"
bool CHUDManager::RenderActiveItemUIQuery()
{
    if (!psHUD_Flags.is(HUD_WEAPON_RT))
        return false;

    if (!need_render_hud())
        return false;

    return (g_player_hud && g_player_hud->render_item_ui_query());
}

void CHUDManager::RenderActiveItemUI() { g_player_hud->render_item_ui(); }
