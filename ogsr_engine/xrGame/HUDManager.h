#pragma once

#include "..\xr_3da\CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

class CHUDTarget;

struct CFontManager : public pureDeviceReset
{
    CFontManager();
    ~CFontManager();

    xr_vector<CGameFont*> m_all_fonts;

    void Render();

    // hud font
    CGameFont* pFontMedium{};
    CGameFont* pFontSmall{};

    CGameFont* pFontDI{};

    CGameFont* pFontArial14{};
    CGameFont* pFontArial21{};
    CGameFont* pFontGraffiti19Russian{};
    CGameFont* pFontGraffiti22Russian{};
    CGameFont* pFontLetterica16Russian{};
    CGameFont* pFontLetterica18Russian{};
    CGameFont* pFontGraffiti32Russian{};
    CGameFont* pFontGraffiti40Russian{};
    CGameFont* pFontGraffiti50Russian{};
    CGameFont* pFontLetterica25{};

    void InitializeFonts();
    void InitializeFont(CGameFont*& F, LPCSTR section, u32 flags = 0);
    LPCSTR GetFontTexName(LPCSTR section);
    CGameFont* InitializeCustomFont(LPCSTR section, u32 flags = 0);

    virtual void OnDeviceReset();
};

class CHUDManager : public CCustomHUD
{
    friend class CUI;

private:
    CUI* pUI;
    CHitMarker HitMarker;
    CHUDTarget* m_pHUDTarget;
    bool b_online;

public:
    CHUDManager();
    virtual ~CHUDManager();
    virtual void OnEvent(EVENT E, u64 P1, u64 P2);

    virtual void Load();

    virtual void Render_First();
    virtual void Render_Last();
    virtual void Render_Actor_Shadow(); // added by KD
    virtual void OnFrame();

    virtual void RenderUI();

    virtual IC CUI* GetUI() { return pUI; }

    void Hit(int idx, float power, const Fvector& dir);
    CFontManager& Font() { return *(UI()->Font()); }

    //текущий предмет на который смотрит HUD
    collide::rq_result& GetCurrentRayQuery();

    //устанвка внешнего вида прицела в зависимости от текущей дисперсии
    void SetCrosshairDisp(float dispf, float disps = 0.f);
    void ShowCrosshair(bool show);

    void SetHitmarkType(LPCSTR tex_name);
    virtual void OnScreenRatioChanged();
    virtual void OnDisconnected();
    virtual void OnConnected();

    virtual void RenderActiveItemUI();
    virtual bool RenderActiveItemUIQuery();

    virtual void net_Relcase(CObject* object);
    CHUDTarget* GetTarget() const { return m_pHUDTarget; };
};
