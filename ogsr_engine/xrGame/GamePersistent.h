#ifndef GamePersistentH
#define GamePersistentH
#pragma once

#include "..\xr_3da\IGame_Persistent.h"
class CMainMenu;
class CUICursor;
class CParticlesObject;
class CUISequencer;
class ui_core;

class CGamePersistent : public IGame_Persistent, public IEventReceiver
{
    bool GameAutopaused = false;

    // ambient particles
    CParticlesObject* ambient_particles;
    u32 ambient_sound_next_time[40]{}; // max snd channels
    u32 ambient_sound_next_time_shoc{};
    u32 ambient_effect_next_time;
    u32 ambient_effect_stop_time;

    float ambient_effect_wind_start;
    float ambient_effect_wind_in_time;
    float ambient_effect_wind_end;
    float ambient_effect_wind_out_time;
    bool ambient_effect_wind_on;

    CUISequencer* m_intro;
    EVENT eQuickLoad;

    fastdelegate::FastDelegate<void()> m_intro_event;

    void start_logo_intro();
    void update_logo_intro();
    void start_game_intro();
    void update_game_intro();

#ifdef DEBUG
    u32 m_frame_counter;
    u32 m_last_stats_frame;
#endif

    void WeathersUpdate();

public:
    ui_core* m_pUI_core;
    IReader* pDemoFile;
    u32 uTime2Change;
    EVENT eDemoStart;

    CGamePersistent();
    virtual ~CGamePersistent();

    void PreStart(LPCSTR op) override;
    virtual void Start(LPCSTR op);
    virtual void Disconnect();

    virtual void OnAppActivate();
    virtual void OnAppDeactivate();

    virtual void OnAppStart();
    virtual void OnAppEnd();
    virtual void OnGameStart();
    virtual void OnGameEnd();
    virtual void OnFrame();
    virtual void OnEvent(EVENT E, u64 P1, u64 P2);

    virtual void UpdateGameType();

    virtual void RegisterModel(IRenderVisual* V);
    virtual float MtlTransparent(u32 mtl_idx);
    virtual void Statistics(CGameFont* F);

    virtual bool OnRenderPPUI_query();
    virtual void OnRenderPPUI_main();
    virtual void OnRenderPPUI_PP();
    virtual void LoadTitle(const char* title_name);
    virtual void SetTip();

    virtual bool CanBePaused();
    bool OnKeyboardPress(int dik);
};

IC CGamePersistent& GamePersistent() { return *((CGamePersistent*)g_pGamePersistent); }

#endif // GamePersistentH
