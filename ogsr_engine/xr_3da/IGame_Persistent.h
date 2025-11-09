#pragma once

#include "Environment.h"
#include "IGame_ObjectPool.h"
#include "Render.h"
#include "../xrCDB/ISpatial.h"

class IRenderVisual;
class IMainMenu;
class ScriptWallmarksManager;
class CPS_Instance;

class ENGINE_API IGame_Persistent : public DLL_Pure, public pureAppStart, public pureAppEnd, public pureAppActivate, public pureAppDeactivate, public pureFrame
{
public:
    union params
    {
        struct
        {
            string256 m_game_or_spawn;
            string256 m_game_type;
            string256 m_alife;
            string256 m_new_or_load;
            u32 m_e_game_type;
        };

        string256 m_params[4];

        params() { reset(); }

        void reset()
        {
            for (auto& m_param : m_params)
                strcpy_s(m_param, "");
        }

        void parse_cmd_line(LPCSTR cmd_line)
        {
            reset();

            int n = _min(4, _GetItemCount(cmd_line, '/'));
            for (int i = 0; i < n; ++i)
            {
                _GetItem(cmd_line, i, m_params[i], '/');

                _strlwr(m_params[i]);
            }
        }
    };
    params m_game_params;

    xr_set<CPS_Instance*> ps_active, ps_destroy;
    xr_vector<CPS_Instance*> ps_needtoplay;
    xr_vector<CPS_Instance*> ps_needtocreate;

    enum GrassBenders_Anim : s8
    {
        BENDER_ANIM_EXPLOSION = 0,
        BENDER_ANIM_DEFAULT = 1,
        BENDER_ANIM_WAVY = 2,
        BENDER_ANIM_SUCK = 3,
        BENDER_ANIM_BLOW = 4,
        BENDER_ANIM_PULSE = 5,
    }; 

  	void GrassBendersUpdateAnimations();
    void GrassBendersAddExplosion(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersAddShot(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersRemoveById(const u16 id);
    void GrassBendersRemoveByIndex(size_t& idx);
    static float GrassBenderToValue(float& current, const float go_to, const float intensity, const bool use_easing);
    void GrassBendersUpdate(const u16 id, size_t& data_idx, u32& data_frame, const Fvector& position, const float init_radius, const float init_str);
    void GrassBendersReset(const size_t idx);
    static void GrassBendersSet(const size_t idx, const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius, const GrassBenders_Anim anim, const bool resetTime);

    CPerlinNoise1D* PerlinNoise1D{};

    static bool IsActorInHideout();
    void UpdateHudRaindrops() const;
    void UpdateRainGloss() const;

    void destroy_particles(const bool& all_particles);

    virtual void PreStart(LPCSTR op);
    virtual void Start(LPCSTR op);
    virtual void Disconnect();

    IGame_ObjectPool ObjectPool;
    IMainMenu* m_pMainMenu{};

    //virtual ScriptWallmarksManager& GetWallmarksManager() const = 0;

    CEnvironment& Environment() const { return *pEnvironment; };

    virtual void OnAppStart();
    virtual void OnAppEnd();
    virtual void OnAppActivate();
    virtual void OnAppDeactivate();
    virtual void OnFrame();

    // вызывается только когда изменяется тип игры
    virtual void OnGameStart();
    virtual void OnGameEnd();

    virtual void UpdateGameType(){};

    virtual void OnSectorChanged(IRender_Sector::sector_id_t sector){};

    virtual void RegisterModel(IRenderVisual* V) = 0;
    virtual float MtlTransparent(u32 mtl_idx) = 0;

    IGame_Persistent();
    virtual ~IGame_Persistent();

    u32 GameType() { return m_game_params.m_e_game_type; };
    virtual void Statistics(CGameFont* F) = 0;
    virtual void LoadTitle(const char* title_name) = 0;
    virtual void SetTip() = 0;

    virtual	void models_savePrefetch();

 //   virtual bool CreateAmbientParticle(const CEnvAmbient::SEffect* effect) = 0;

private:
    CEnvironment* pEnvironment{};

    void ProcessParticlesCreate();
};

class IMainMenu
{
public:
    virtual ~IMainMenu(){};
    virtual void Activate(bool bActive) = 0;
    virtual bool IsActive() = 0;
    virtual bool CanSkipSceneRendering() = 0;
    virtual void DestroyInternal(bool bForce) = 0;

    virtual void Screenshot(IRender_interface::ScreenshotMode mode = IRender_interface::SM_NORMAL, LPCSTR name = nullptr) = 0;
    //virtual void ScreenshotEnd() = 0;
};

ENGINE_API extern IGame_Persistent* g_pGamePersistent;
ENGINE_API extern bool IsMainMenuActive();

ENGINE_API extern BOOL g_prefetch;
