////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_particles.h"
#include "../xr_3da/objectanimator.h"
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/ParticleCustom.h"

CScriptParticlesCustom::CScriptParticlesCustom(CScriptParticles* owner, LPCSTR caParticlesName) : CParticlesObject(caParticlesName, FALSE, true)
{
    //	Msg							("CScriptParticlesCustom: 0x%08x",*(int*)&owner);
    m_owner = owner;
    m_animator = 0;
}

CScriptParticlesCustom::~CScriptParticlesCustom()
{
    //	Msg							("~CScriptParticlesCustom: 0x%08x",*(int*)&m_owner);
    xr_delete(m_animator);
}

void CScriptParticlesCustom::PSI_internal_delete()
{
    if (m_owner)
        m_owner->m_particles = NULL;
    CParticlesObject::PSI_internal_delete();
}

void CScriptParticlesCustom::PSI_destroy()
{
    if (m_owner)
        m_owner->m_particles = NULL;
    CParticlesObject::PSI_destroy();
}

void CScriptParticlesCustom::shedule_Update(u32 _dt)
{
    CParticlesObject::shedule_Update(_dt);
    if (m_animator)
    {
        float dt = float(_dt) / 1000.f;
        Fvector prev_pos = m_animator->XFORM().c;
        m_animator->Update(dt);
        Fvector vel;
        vel.sub(m_animator->XFORM().c, prev_pos).div(dt);
        UpdateParent(m_animator->XFORM(), vel);
    }
}
void CScriptParticlesCustom::LoadPath(LPCSTR caPathName)
{
    if (!m_animator)
        m_animator = xr_new<CObjectAnimator>();
    if ((0 == m_animator->Name()) || (0 != xr_strcmp(m_animator->Name(), caPathName)))
    {
        m_animator->Clear();
        m_animator->Load(caPathName);
    }
}
void CScriptParticlesCustom::StartPath(bool looped)
{
    VERIFY(m_animator);
    m_animator->Play(looped);
}
void CScriptParticlesCustom::PausePath(bool val)
{
    VERIFY(m_animator);
    m_animator->Pause(val);
}
void CScriptParticlesCustom::StopPath()
{
    VERIFY(m_animator);
    m_animator->Stop();
}

void CScriptParticlesCustom::remove_owner()
{
    R_ASSERT(m_owner);
    m_owner = 0;
}


CScriptParticles::CScriptParticles(LPCSTR caParticlesName)
{
    m_particles = xr_new<CScriptParticlesCustom>(this, caParticlesName);
    m_transform.identity();
}

CScriptParticles::~CScriptParticles()
{
    if (m_particles)
    {
        // destroy particles
        m_particles->remove_owner();
        if (!m_particles->IsLooped() && (m_particles->IsPlaying() || m_particles->LifeTime() > 0))
            m_particles->SetAutoRemove(true);
        else
            m_particles->PSI_destroy();
        m_particles = nullptr;
    }
}

void CScriptParticles::Play(bool bHudMode) const
{
    VERIFY(m_particles);

    m_particles->Play(bHudMode);
}

void CScriptParticles::PlayAtPos(const Fvector& position, bool bHudMode)
{
    VERIFY(m_particles);

    // m_particles->play_at_pos(position);

    m_transform.translate_over(position);
    m_particles->UpdateParent(m_transform, {});
    m_particles->Play(bHudMode);
    m_particles->UpdateParent(m_transform, {});
}

void CScriptParticles::Stop() const
{
    VERIFY(m_particles);
    m_particles->Stop(FALSE);
}

void CScriptParticles::StopDeffered() const
{
    VERIFY(m_particles);
    m_particles->Stop(TRUE);
}

void CScriptParticles::MoveTo(const Fvector& pos, const Fvector& vel)
{
    VERIFY(m_particles);
    m_transform.translate_over(pos);
    m_particles->UpdateParent(m_transform, vel);
}

void CScriptParticles::XFORMMoveTo(const Fvector& pos)
{
    VERIFY(m_particles);
    m_transform.translate_over(pos);
    m_particles->SetXFORM(m_transform);
}

void CScriptParticles::SetDirection(const Fvector& dir)
{
    Fmatrix matrix;
    matrix.identity();
    matrix.k.set(dir);
    Fvector::generate_orthonormal_basis_normalized(matrix.k, matrix.j, matrix.i);
    matrix.translate_over(m_transform.c);
    m_transform.set(matrix);
    m_particles->UpdateParent(matrix, {});
}

void CScriptParticles::SetOrientation(float yaw, float pitch, float roll)
{
    Fmatrix matrix;
    matrix.setHPB(yaw, pitch, roll); // ?????????? matrix.c
    matrix.translate_over(m_transform.c);
    m_transform.set(matrix);
    m_particles->UpdateParent(matrix, {});
}

bool CScriptParticles::IsPlaying() const
{
    VERIFY(m_particles);
    return m_particles->IsPlaying();
}

bool CScriptParticles::IsLooped() const
{
    VERIFY(m_particles);
    return m_particles->IsLooped();
}

void CScriptParticles::LoadPath(LPCSTR caPathName) const
{
    VERIFY(m_particles);
    m_particles->LoadPath(caPathName);
}
void CScriptParticles::StartPath(bool looped) const { m_particles->StartPath(looped); }
void CScriptParticles::StopPath() const { m_particles->StopPath(); }
void CScriptParticles::PausePath(bool val) const { m_particles->PausePath(val); }

int CScriptParticles::LifeTime() const { return m_particles->LifeTime(); }
u32 CScriptParticles::Length() const { return LifeTime(); }
