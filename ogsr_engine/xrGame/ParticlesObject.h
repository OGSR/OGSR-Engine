#pragma once

#include "..\xr_3da\PS_instance.h"

class CParticlesObject : public CPS_Instance
{
    typedef CPS_Instance inherited;

    std::atomic<u32> dwLastTime{};
    std::atomic<u32> market{};
    std::atomic_bool m_bLooped{}; // флаг, что система зациклена

    void Init(LPCSTR p_name, IRender_Sector::sector_id_t sector_id, BOOL bAutoRemove);
    void UpdateSpatial();

public:
    CParticlesObject(LPCSTR p_name, BOOL bAutoRemove, bool destroy_on_game_load);
    virtual ~CParticlesObject();

    virtual bool shedule_Needed() { return true; };
    virtual float shedule_Scale();
    virtual void shedule_Update(u32 dt);
    virtual void renderable_Render(u32 context_id, IRenderable* root) override;
    void DoWork();
    virtual void PerformFrame() override;

    Fvector& Position();
    void SetXFORM(const Fmatrix& m);
    IC Fmatrix& XFORM() { return renderable.xform; }
    void UpdateParent(const Fmatrix& m, const Fvector& vel);

    void play_at_pos(const Fvector& pos, BOOL xform = FALSE);
    virtual void Play(BOOL hudMode = FALSE);
    void Stop(BOOL bDefferedStop = TRUE);

    bool IsLooped() { return m_bLooped; }
    bool IsAutoRemove();
    bool IsPlaying();
    void SetAutoRemove(bool auto_remove);
    int LifeTime() const { return m_iLifeTime; }

    const shared_str Name();

public:
    static CParticlesObject* Create(LPCSTR p_name, BOOL bAutoRemove = TRUE, bool remove_on_game_load = true)
    {
        return xr_new<CParticlesObject>(p_name, bAutoRemove, remove_on_game_load);
    }
    static void Destroy(CParticlesObject*& p)
    {
        if (p)
        {
            p->PSI_destroy();
            p = 0;
        }
    }
};
