#pragma once

#include "..\xr_3da\PS_instance.h"

class CParticlesObject : public CPS_Instance
{
    typedef CPS_Instance inherited;

    std::atomic<u32> dwLastTime{};
    std::atomic<u32> market{};

    std::atomic_bool m_bLooped{}; // флаг, что система зациклена
    std::atomic_bool m_bPlaying{}; // вызвана функция Play()
    std::atomic_bool m_bStopping{}; // вызвана функция Stop()
    std::atomic_bool m_bDeferredStopped{};

    std::atomic_bool m_bCreated{}; // визуал партикла уже создан

    void Init(LPCSTR p_name, IRender_Sector::sector_id_t sector_id, BOOL bAutoRemove);
    void UpdateSpatial();
    void DoWork();

protected:
    shared_str m_name{}; // имя группы или эффекта

    Fmatrix matrix{};
    Fvector velocity{};
    bool hud_mode{};
    bool xform_set{};

public:
    CParticlesObject(LPCSTR p_name, BOOL bAutoRemove, bool destroy_on_game_load);
    virtual ~CParticlesObject();

    virtual bool shedule_Needed() { return true; }
    virtual float shedule_Scale();
    virtual void shedule_Update(u32 dt);
    virtual void renderable_Render(u32 context_id, IRenderable* root);

    virtual void PerformFrame();
    virtual void PerformCreate();    

    void SetXFORM(const Fmatrix& m);
    IC Fmatrix& XFORM() { return renderable.xform; }
    void UpdateParent(const Fmatrix& m, const Fvector& vel);

    void PlayAtPos(const Fvector& pos);
    virtual void Play(BOOL hudMode = FALSE);
    void Stop(BOOL bDefferedStop = TRUE);

    bool IsLooped() const { return m_bLooped; }
    bool IsAutoRemove() const;
    void SetAutoRemove(bool auto_remove);

    bool IsPlaying() const;
    bool IsDeferredStopped() const;

    int LifeTime() const { return m_iLifeTime; }

    shared_str Name() const;

public:
    static CParticlesObject* Create(LPCSTR p_name, BOOL bAutoRemove = TRUE, bool remove_on_game_load = true)
    {
        ZoneScoped;

        return xr_new<CParticlesObject>(p_name, bAutoRemove, remove_on_game_load);
    }
    static void Destroy(CParticlesObject*& p)
    {
        if (p)
        {
            p->PSI_destroy();
            p = nullptr;
        }
    }
};
