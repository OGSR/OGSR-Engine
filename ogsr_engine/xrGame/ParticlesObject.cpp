//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"

#include "ParticlesObject.h"
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

    // create visual
    renderable.visual = Render->model_CreateParticles(p_name);
    VERIFY(renderable.visual);
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    const float time_limit = V->GetTimeLimit();

    if (time_limit > 0.f)
    {
        m_iLifeTime = iFloor(time_limit * 1000.f);
    }
    else
    {
        if (bAutoRemove)
        {
            R_ASSERT3(!m_bAutoRemove, "Can't set auto-remove flag for looped particle system.", p_name);
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
    // spatial	(+ workaround occasional bug inside particle-system)
    if (_valid(renderable.visual->getVisData().sphere))
    {
        Fvector P;
        float R;
        renderable.xform.transform_tiny(P, renderable.visual->getVisData().sphere.P);
        R = renderable.visual->getVisData().sphere.R;
        if (0 == spatial.type)
        {
            // First 'valid' update - register
            spatial.type = STYPE_RENDERABLE;
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

const shared_str CParticlesObject::Name()
{
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    return (V) ? V->Name() : "";
}

//----------------------------------------------------
void CParticlesObject::Play(BOOL hudMode)
{
    dwLastTime = Device.dwTimeGlobal - 33ul;
    market = Device.dwFrame - 1;

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->SetHudMode(hudMode);
    V->Play();

    DoWork();
}

void CParticlesObject::play_at_pos(const Fvector& pos, BOOL xform)
{
    dwLastTime = Device.dwTimeGlobal - 33ul;
    market = Device.dwFrame - 1;

    Fmatrix m;
    m.translate(pos);

    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->UpdateParent(m, {}, xform);
    V->Play();

    DoWork();
}

void CParticlesObject::Stop(BOOL bDefferedStop)
{
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->Stop(bDefferedStop);
}

void CParticlesObject::shedule_Update(u32 _dt)
{
    inherited::shedule_Update(_dt);

    if (market < Device.dwFrame - (30 + Random.randI(60))) // если не рендерили на прошлом кадре - ГГ не видит
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
    DoWork();
}

void CParticlesObject::SetXFORM(const Fmatrix& m)
{
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->UpdateParent(m, {}, TRUE);
    renderable.xform.set(m);
    UpdateSpatial();
}

void CParticlesObject::UpdateParent(const Fmatrix& m, const Fvector& vel)
{
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    V->UpdateParent(m, vel, FALSE);
    UpdateSpatial();
}

Fvector& CParticlesObject::Position() { return renderable.visual->getVisData().sphere.P; }

float CParticlesObject::shedule_Scale() { return Device.vCameraPosition.distance_to(Position()) / 200.f; }

void CParticlesObject::renderable_Render(u32 context_id, IRenderable* root)
{
    R_ASSERT(renderable.visual);

    ::Render->add_Visual(context_id, root, renderable.visual, renderable.xform);
}

bool CParticlesObject::IsAutoRemove()
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
bool CParticlesObject::IsPlaying()
{
    IParticleCustom* V = smart_cast<IParticleCustom*>(renderable.visual);
    VERIFY(V);
    return !!V->IsPlaying();
}
