#pragma once

#include "Environment.h"
#include "IGame_ObjectPool.h"

#include "ShadersExternalData.h" //--#SM+#--

class IRenderVisual;
class IMainMenu;
class ENGINE_API CPS_Instance;
//-----------------------------------------------------------------------------------------------------------
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
            for (int i = 0; i < 4; ++i)
                strcpy_s(m_params[i], "");
        }
        void parse_cmd_line(LPCSTR cmd_line)
        {
            reset();
            int n = _min(4, _GetItemCount(cmd_line, '/'));
            for (int i = 0; i < n; ++i)
            {
                _GetItem(cmd_line, i, m_params[i], '/');
                strlwr(m_params[i]);
            }
        }
    };
    params m_game_params;

    xr_set<CPS_Instance*> ps_active, ps_destroy;
    xr_vector<CPS_Instance*> ps_needtoplay;

    void GrassBendersUpdateExplosions();
    void GrassBendersAddExplosion(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersAddShot(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersRemoveById(const u16 id);
    void GrassBendersRemoveByIndex(size_t& idx);
    void GrassBendersUpdate(const u16 id, size_t& data_idx, u32& data_frame, const Fvector& position);
    void GrassBendersReset(const size_t idx);
    void GrassBendersSet(const size_t idx, const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius, const bool resetTime);

    bool IsActorInHideout() const;
    void UpdateHudRaindrops() const;
    void UpdateRainGloss() const;

    void destroy_particles(const bool& all_particles);

    virtual void PreStart(LPCSTR op);
    virtual void Start(LPCSTR op);
    virtual void Disconnect();

    IGame_ObjectPool ObjectPool;
    IMainMenu* m_pMainMenu;

    CEnvironment* pEnvironment;
    CEnvironment& Environment() { return *pEnvironment; };

    ShadersExternalData m_pGShaderConstants; //--#SM+#--

    virtual bool OnRenderPPUI_query() { return FALSE; }; // should return true if we want to have second function called
    virtual void OnRenderPPUI_main(){};
    virtual void OnRenderPPUI_PP(){};

    virtual void OnAppStart();
    virtual void OnAppEnd();
    virtual void OnAppActivate();
    virtual void OnAppDeactivate();
    virtual void OnFrame();

    // вызывается только когда изменяется тип игры
    virtual void OnGameStart();
    virtual void OnGameEnd();

    virtual void UpdateGameType(){};

    virtual void OnSectorChanged(int sector){};

    virtual void RegisterModel(IRenderVisual* V) = 0;
    virtual float MtlTransparent(u32 mtl_idx) = 0;

    IGame_Persistent();
    virtual ~IGame_Persistent();

    u32 GameType() { return m_game_params.m_e_game_type; };
    virtual void Statistics(CGameFont* F) = 0;
    virtual void LoadTitle(const char* title_name) = 0;
    virtual void SetTip() = 0;

    virtual bool CanBePaused() { return true; }
    virtual	void models_savePrefetch();
};

class IMainMenu
{
public:
    virtual ~IMainMenu(){};
    virtual void Activate(bool bActive) = 0;
    virtual bool IsActive() = 0;
    virtual bool CanSkipSceneRendering() = 0;
    virtual void DestroyInternal(bool bForce) = 0;
};

extern ENGINE_API IGame_Persistent* g_pGamePersistent;
ENGINE_API extern bool IsMainMenuActive();
ENGINE_API extern BOOL g_prefetch;