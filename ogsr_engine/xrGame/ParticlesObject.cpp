//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"

#include "ParticlesObject.h"

#include "gamepersistent.h"
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/ParticleCustom.h"
#include "..\xr_3da\render.h"
#include "..\xr_3da\IGame_Persistent.h"

CParticlesObject::CParticlesObject(LPCSTR p_name, BOOL bAutoRemove, bool destroy_on_game_load) : inherited(destroy_on_game_load)
{
    Init(p_name, IRender_Sector::INVALID_SECTOR_ID, bAutoRemove);
}

void CParticlesObject::Init(LPCSTR p_name, IRender_Sector::sector_id_t sector_id, BOOL bAutoRemove)
{
    m_bLooped = false;
    m_bAutoRemove = bAutoRemove;

    m_name = p_name;

    GamePersistent().ps_needtocreate.push_back(this);

    float time_limit = Render->GetParticlesTimeLimit(p_name);

    if (time_limit > 0.f)
    {
        m_iLifeTime = iFloor(time_limit * 1000.f);
    }
    else
    {
        if (bAutoRemove)
        {
            R_ASSERT(!m_bAutoRemove, "Can't set auto-remove flag for looped particle system.", p_name);
        }
        else
        {
            m_iLifeTime = 0;
            m_bLooped = true;
        }
    }

    // spatial
    spatial.type = 0;
    spatial.sector_id = sector_id;
    spatial.dbg_name = "CParticlesObject";

    // sheduled
    shedule.t_min = 20;
    shedule.t_max = 50;
    shedule_register();

    dwLastTime = Device.dwTimeGlobal;
}

//----------------------------------------------------
CParticlesObject::~CParticlesObject()
{
    //	we do not need this since CPS_Instance does it
    //	shedule_unregister(true);
}

void CParticlesObject::UpdateSpatial()
{
    R_ASSERT(renderable.visual);

    auto& vis_data = renderable.visual->getVisData();

    // spatial	(+ workaround occasional bug inside particle-system)
    if (_valid(vis_data.sphere))
    {
        Fvector P;
        renderable.xform.transform_tiny(P, vis_data.sphere.P);
        const float R = vis_data.sphere.R;
        if (0 == spatial.type)
        {
            // First 'valid' update - register
            spatial.type = STYPE_RENDERABLE | STYPE_PARTICLE;
            spatial.sphere.set(P, R);
            spatial_register();
        }
        else
        {
            BOOL bMove = FALSE;
            if (!P.similar(spatial.sphere.P, EPS_L * 10.f))
                bMove = TRUE;
            if (!fsimilar(R, spatial.sphere.R, 0.15f))
                bMove = TRUE;
            if (bMove)
            {
                spatial.sphere.set(P, R);
                spatial_move();
            }
        }
    }
}

//----------------------------------------------------
void CParticlesObject::Play(BOOL hudMode)
{
    m_bStopping = false;
    m_bDeferredStopped = false;
    m_bPlaying = true;

    dwLastTime = Device.dwTimeGlobal - 33ul;
    market = Device.dwFrame - 1;

    if (!m_bCreated)
    {
        hud_mode = hudMode;

        return;
    }

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);

    V->SetHudMode(hudMode);
    V->Play();

    DoWork();
}

void CParticlesObject::PlayAtPos(const Fvector& pos)
{
    m_bStopping = false;
    m_bDeferredStopped = false;
    m_bPlaying = true;

    dwLastTime = Device.dwTimeGlobal - 33ul;
    market = Device.dwFrame - 1;

    Fmatrix m;
    m.translate(pos);

    if (!m_bCreated)
    {
        matrix = m;

        return;
    }

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);

    V->UpdateParent(m, {}, false);
    V->Play();

    DoWork();
}

void CParticlesObject::Stop(BOOL bDefferedStop)
{
    m_bStopping = true;
    m_bPlaying = false;
    m_bDeferredStopped = bDefferedStop;

    if (!m_bCreated)
        return;

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->Stop(bDefferedStop);
}

void CParticlesObject::shedule_Update(u32 _dt)
{
    inherited::shedule_Update(_dt);

    if (m_bCreated && market < Device.dwFrame - (30 + Random.randI(60))) // если не рендерили на прошлом кадре - ГГ не видит
    {
        DoWork();
    }
}

void CParticlesObject::DoWork()
{
    // Update
    if (m_bDead)
        return;

    if (market != Device.dwFrame)
    {
        market = Device.dwFrame;

        ZoneScoped;

        const u32 dt = Device.dwTimeGlobal - dwLastTime;
        if (dt)
        {
            IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
            VERIFY(V);
            V->OnFrame(dt);

            dwLastTime = Device.dwTimeGlobal;
        }

        UpdateSpatial();
    }
}

void CParticlesObject::PerformFrame()
{
    if (m_bCreated)
        DoWork();
}

void CParticlesObject::PerformCreate()
{
    if (!m_bCreated)
    {
        renderable.visual = Render->model_CreateParticles(m_name.c_str());
        R_ASSERT(renderable.visual);

        IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
        VERIFY(V);

        V->UpdateParent(matrix, velocity, xform_set);
        if (xform_set)
            renderable.xform.set(matrix);
        UpdateSpatial();

        if (m_bPlaying)
        {
            V->SetHudMode(hud_mode);
            V->Play();

            DoWork();
        }
        m_bCreated = true;
    }
}

void CParticlesObject::SetXFORM(const Fmatrix& m)
{
    if (!m_bCreated)
    {
        matrix = m;
        velocity = {};
        xform_set = true;

        return;
    }

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->UpdateParent(m, {}, TRUE);
    renderable.xform.set(m);
    UpdateSpatial();
}

void CParticlesObject::UpdateParent(const Fmatrix& m, const Fvector& vel)
{
    if (!m_bCreated)
    {
        matrix = m;
        velocity = vel;
        xform_set = false;

        return;
    }

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->UpdateParent(m, vel, FALSE);
    UpdateSpatial();
}

float CParticlesObject::shedule_Scale()
{
    if (!m_bCreated)
        return 50.f / 200.f;

    R_ASSERT(renderable.visual);
    return Device.vCameraPosition.distance_to(renderable.visual->getVisData().sphere.P) / 200.f;
}

void CParticlesObject::renderable_Render(u32 context_id, IRenderable* root)
{
    R_ASSERT(renderable.visual);

    if (m_bCreated)
        ::Render->add_Visual(context_id, root, renderable.visual, renderable.xform);
}

bool CParticlesObject::IsAutoRemove() const
{
    if (m_bAutoRemove)
        return true;
    else
        return false;
}
void CParticlesObject::SetAutoRemove(bool auto_remove)
{
    VERIFY(!IsLooped());
    m_bAutoRemove = auto_remove;
}

//играются ли партиклы, отличается от PSI_Alive, тем что после
//остановки Stop партиклы могут еще доигрывать анимацию IsPlaying = true
bool CParticlesObject::IsPlaying() const
{
    if (!m_bCreated)
        return m_bPlaying;

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    return !!V->IsPlaying();
}

bool CParticlesObject::IsDeferredStopped() const
{
    if (!m_bCreated)
        return m_bDeferredStopped;

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    return !!V->IsDeferredStopped();
}

shared_str CParticlesObject::Name() const
{
    if (!m_bCreated)
        return m_name;

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    return (V) ? V->Name() : "";
}