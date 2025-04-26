#include "stdafx.h"

#include "ParticleEffect.h"
#include <xmmintrin.h>

#include "HUDInitializer.h"

using namespace PAPI;
using namespace PS;

static void ApplyTexgen(CBackend& cmd_list, const Fmatrix& mVP)
{
    Fmatrix mTexgen;

    constexpr Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};

    mTexgen.mul(mTexelAdjust, mVP);
    cmd_list.set_c("mVPTexgen", mTexgen);
}

void PS::OnEffectParticleBirth(void* owner, u32, PAPI::Particle& m, u32)
{
    const CParticleEffect* PE = static_cast<CParticleEffect*>(owner);
    VERIFY(PE);
    const CPEDef* PED = PE->GetDefinition();
    if (PED)
    {
        if (PED->m_Flags.is(CPEDef::dfRandomFrame))
            m.frame = (u16)iFloor(Random.randI(PED->m_Frame.m_iFrameCount) * 255.f);
        if (PED->m_Flags.is(CPEDef::dfAnimated) && PED->m_Flags.is(CPEDef::dfRandomPlayback) && Random.randI(2))
            m.flags.set(Particle::ANIMATE_CCW, TRUE);
    }
}

void PS::OnEffectParticleDead(void*, u32, PAPI::Particle&, u32)
{
    //	CPEDef* PE = static_cast<CPEDef*>(owner);
}

//------------------------------------------------------------------------------
// class CParticleEffect
//------------------------------------------------------------------------------
CParticleEffect::CParticleEffect()
{
    m_HandleEffect = ParticleManager()->CreateEffect(1);
    VERIFY(m_HandleEffect >= 0);
    m_HandleActionList = ParticleManager()->CreateActionList();
    VERIFY(m_HandleActionList >= 0);

    m_Def = nullptr;

    m_RT_Flags.zero();
    m_fElapsedLimit = 0.f;
    m_MemDT = 0;
    m_InitialPosition.set(0, 0, 0);
    m_XFORM.identity();
}

CParticleEffect::~CParticleEffect()
{
    OnDeviceDestroy();

    ParticleManager()->DestroyEffect(m_HandleEffect);
    ParticleManager()->DestroyActionList(m_HandleActionList);
}

void CParticleEffect::Play()
{
    if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
        m_fElapsedLimit = m_Def->m_fTimeLimit;

    m_RT_Flags.set(flRT_DefferedStop, FALSE);
    m_RT_Flags.set(flRT_Playing, TRUE);

    ParticleManager()->PlayEffect(m_HandleEffect, m_HandleActionList);
}

void CParticleEffect::Stop(BOOL bDefferedStop)
{
    ParticleManager()->StopEffect(m_HandleEffect, m_HandleActionList, bDefferedStop);

    if (bDefferedStop)
    {
        m_RT_Flags.set(flRT_DefferedStop, TRUE);
    }
    else
    {
        m_RT_Flags.set(flRT_Playing, FALSE);
    }
}

void CParticleEffect::RefreshShader()
{
    OnDeviceDestroy();
    OnDeviceCreate();
}

void CParticleEffect::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
    m_RT_Flags.set(flRT_XFORM, bXFORM);

    if (bXFORM)
    {
        m_XFORM.set(m);
    }
    else
    {
        m_InitialPosition = m.c;
        ParticleManager()->Transform(m_HandleActionList, m, velocity);
    }
}

void CParticleEffect::OnFrame(u32 frame_dt)
{
    ZoneScoped;

    if (m_Def && m_RT_Flags.is(flRT_Playing))
    {
        m_MemDT += frame_dt;

        int StepCount = 0;
        const u32 uDT_STEP = m_Def->GetUStep();
        const float fDT_STEP = m_Def->GetFStep();
        if (m_MemDT >= uDT_STEP)
        {
            // allow maximum of three steps (99ms) to avoid slowdown after loading
            // it will really skip updates at less than 10fps, which is unplayable
            StepCount = m_MemDT / uDT_STEP;
            m_MemDT = m_MemDT % uDT_STEP;
            clamp(StepCount, 0, 3);
        }

        for (; StepCount; StepCount--)
        {
            if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
            {
                if (!m_RT_Flags.is(flRT_DefferedStop))
                {
                    m_fElapsedLimit -= fDT_STEP;
                    if (m_fElapsedLimit < 0.f)
                    {
                        m_fElapsedLimit = m_Def->m_fTimeLimit;
                        Stop(true);
                        break;
                    }
                }
            }
            ParticleManager()->Update(m_HandleEffect, m_HandleActionList, fDT_STEP);

            Particle* particles;
            u32 p_cnt;
            ParticleManager()->GetParticles(m_HandleEffect, particles, p_cnt);

            // our actions
            if (m_Def->m_Flags.is(CPEDef::dfFramed | CPEDef::dfAnimated))
                m_Def->ExecuteAnimate(particles, p_cnt, fDT_STEP);

            //-move action
            if (p_cnt)
            {
                vis.box.invalidate();
                float p_size = 0.f;
                for (u32 i = 0; i < p_cnt; i++)
                {
                    Particle& m = particles[i];
                    vis.box.modify((Fvector&)m.pos);
                    if (m.size.x > p_size)
                        p_size = m.size.x;
                    if (m.size.y > p_size)
                        p_size = m.size.y;
                    if (m.size.z > p_size)
                        p_size = m.size.z;
                }
                vis.box.grow(p_size);
                vis.box.getsphere(vis.sphere.P, vis.sphere.R);
            }
            if (m_RT_Flags.is(flRT_DefferedStop) && (0 == p_cnt))
            {
                m_RT_Flags.set(flRT_Playing | flRT_DefferedStop, FALSE);
                break;
            }
        }
    }
    else
    {
        vis.box.set(m_InitialPosition, m_InitialPosition);
        vis.box.grow(EPS_L);
        vis.box.getsphere(vis.sphere.P, vis.sphere.R);
    }
}

BOOL CParticleEffect::Compile(CPEDef* def)
{
    m_Def = def;

    if (m_Def)
    {
        // refresh shader
        RefreshShader();

        // append actions
        IReader F(m_Def->m_Actions.pointer(), m_Def->m_Actions.size());

        ParticleManager()->LoadActions(m_HandleActionList, F, m_Def->m_copFormat);
        ParticleManager()->SetMaxParticles(m_HandleEffect, m_Def->m_MaxParticles);
        ParticleManager()->SetCallback(m_HandleEffect, OnEffectParticleBirth, OnEffectParticleDead, this, 0);

        // time limit
        if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
            m_fElapsedLimit = m_Def->m_fTimeLimit;
    }

    return TRUE;
}

void CParticleEffect::SetBirthDeadCB(PAPI::OnBirthParticleCB bc, PAPI::OnDeadParticleCB dc, void* owner, u32 p) const
{
    ParticleManager()->SetCallback(m_HandleEffect, bc, dc, owner, p);
}

u32 CParticleEffect::ParticlesCount() { return ParticleManager()->GetParticlesCount(m_HandleEffect); }

void CParticleEffect::Depart()
{
    dxParticleCustom::Depart();

    ParticleManager()->StopEffect(m_HandleEffect, m_HandleActionList, FALSE);

    m_RT_Flags.zero();
    m_fElapsedLimit = 0.f;
    m_MemDT = 0;
    m_InitialPosition.set(0, 0, 0);
    m_XFORM.identity();
    vis.clear();
}

//------------------------------------------------------------------------------
// Render
//------------------------------------------------------------------------------
void CParticleEffect::Copy(dxRender_Visual*) { FATAL("Can't duplicate particle system - NOT IMPLEMENTED"); }

void CParticleEffect::OnDeviceCreate()
{
    if (m_Def)
    {
        if (m_Def->m_Flags.is(CPEDef::dfSprite))
        {
            geom.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
            shader = m_Def->m_CachedShader;
        }
    }
}

void CParticleEffect::OnDeviceDestroy()
{
    if (m_Def)
    {
        if (m_Def->m_Flags.is(CPEDef::dfSprite))
        {
            geom.destroy();
            shader.destroy();
        }
    }
}

IC void FillSprite(FVF::LIT*& pv, const Fvector& T, const Fvector& R, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float r1, float r2, u32 clr, float sina, float cosa)
{
    ZoneScoped;

    __m128 Vr, Vt, _T, _R, _pos, _zz, _sa, _ca, a, b, c, d;

    _sa = _mm_set1_ps(sina);
    _ca = _mm_set1_ps(cosa);

    _T = _mm_load_ss((float*)&T.x);
    _T = _mm_loadh_pi(_T, (__m64*)&T.y);

    _R = _mm_load_ss((float*)&R.x);
    _R = _mm_loadh_pi(_R, (__m64*)&R.y);

    _pos = _mm_load_ss((float*)&pos.x);
    _pos = _mm_loadh_pi(_pos, (__m64*)&pos.y);

    _zz = _mm_setzero_ps();

    Vr = _mm_mul_ps(_mm_set1_ps(r1), _mm_add_ps(_mm_mul_ps(_T, _sa), _mm_mul_ps(_R, _ca)));
    Vt = _mm_mul_ps(_mm_set1_ps(r2), _mm_sub_ps(_mm_mul_ps(_T, _ca), _mm_mul_ps(_R, _sa)));

    a = _mm_sub_ps(Vt, Vr);
    b = _mm_add_ps(Vt, Vr);
    c = _mm_sub_ps(_zz, a);
    d = _mm_sub_ps(_zz, b);

    a = _mm_add_ps(a, _pos);
    d = _mm_add_ps(d, _pos);
    b = _mm_add_ps(b, _pos);
    c = _mm_add_ps(c, _pos);

    _mm_store_ss((float*)&pv->p.x, d);
    _mm_storeh_pi((__m64*)&pv->p.y, d);
    pv->color = clr;
    pv->t.set(lt.x, rb.y);
    pv++;

    _mm_store_ss((float*)&pv->p.x, a);
    _mm_storeh_pi((__m64*)&pv->p.y, a);
    pv->color = clr;
    pv->t.set(lt.x, lt.y);
    pv++;

    _mm_store_ss((float*)&pv->p.x, c);
    _mm_storeh_pi((__m64*)&pv->p.y, c);
    pv->color = clr;
    pv->t.set(rb.x, rb.y);
    pv++;

    _mm_store_ss((float*)&pv->p.x, b);
    _mm_storeh_pi((__m64*)&pv->p.y, b);
    pv->color = clr;
    pv->t.set(rb.x, lt.y);
    pv++;
}

IC void FillSprite(FVF::LIT*& pv, const Fvector& pos, const Fvector& dir, const Fvector2& lt, const Fvector2& rb, float r1, float r2, u32 clr, float sina, float cosa)
{
    ZoneScoped;

    const Fvector& T = dir;
    Fvector R;

    // R.crossproduct(T,Device.vCameraDirection).normalize_safe();

    __m128 _t, _t1, _t2, _r, _r1, _r2;

    // crossproduct

    _t = _mm_load_ss((float*)&T.x);
    _t = _mm_loadh_pi(_t, (__m64*)&T.y);

    _r = _mm_load_ss((float*)&Device.vCameraDirection.x);
    _r = _mm_loadh_pi(_r, (__m64*)&Device.vCameraDirection.y);

    _t1 = _mm_shuffle_ps(_t, _t, _MM_SHUFFLE(0, 3, 1, 2));
    _t2 = _mm_shuffle_ps(_t, _t, _MM_SHUFFLE(2, 0, 1, 3));

    _r1 = _mm_shuffle_ps(_r, _r, _MM_SHUFFLE(2, 0, 1, 3));
    _r2 = _mm_shuffle_ps(_r, _r, _MM_SHUFFLE(0, 3, 1, 2));

    _t1 = _mm_mul_ps(_t1, _r1);
    _t2 = _mm_mul_ps(_t2, _r2);

    _t1 = _mm_sub_ps(_t1, _t2); // z | y | 0 | x

    // normalize_safe

    _t2 = _mm_mul_ps(_t1, _t1); // zz | yy | 00 | xx
    _r1 = _mm_movehl_ps(_t2, _t2); // zz | yy | zz | yy
    _t2 = _mm_add_ss(_t2, _r1); // zz | yy | 00 | xx + yy
    _r1 = _mm_shuffle_ps(_r1, _r1, _MM_SHUFFLE(1, 1, 1, 1)); // zz | zz | zz | zz
    _t2 = _mm_add_ss(_t2, _r1); // zz | yy | 00 | xx + yy + zz

    _r1 = _mm_set_ss(std::numeric_limits<float>::min());

    if (_mm_comigt_ss(_t2, _r1))
    {
        _t2 = _mm_rsqrt_ss(_t2);
        _t2 = _mm_shuffle_ps(_t2, _t2, _MM_SHUFFLE(0, 0, 0, 0));
        _t1 = _mm_mul_ps(_t1, _t2);
    }

    _mm_store_ss((float*)&R.x, _t1);
    _mm_storeh_pi((__m64*)&R.y, _t1);

    FillSprite(pv, T, R, pos, lt, rb, r1, r2, clr, sina, cosa);
}

extern ENGINE_API float psHUD_FOV;

__forceinline void magnitude_sse(Fvector& vec, float& res)
{
    __m128 tv, tu;

    tv = _mm_load_ss((float*)&vec.x); // tv = 0 | 0 | 0 | x
    tv = _mm_loadh_pi(tv, (__m64*)&vec.y); // tv = z | y | 0 | x
    tv = _mm_mul_ps(tv, tv); // tv = zz | yy | 0 | xx
    tu = _mm_movehl_ps(tv, tv); // tu = zz | yy | zz | yy
    tv = _mm_add_ss(tv, tu); // tv = zz | yy | 0 | xx + yy
    tu = _mm_shuffle_ps(tu, tu, _MM_SHUFFLE(1, 1, 1, 1)); // tu = zz | zz | zz | zz
    tv = _mm_add_ss(tv, tu); // tv = zz | yy | 0 | xx + yy + zz
    tv = _mm_sqrt_ss(tv); // tv = zz | yy | 0 | sqrt( xx + yy + zz )

    _mm_store_ss((float*)&res, tv);
}

void ParticleRenderStream(CParticleEffect& pPE, PAPI::Particle* particles, FVF::LIT* pv, u32 p_from, u32 p_to)
{
    float sina = 0.0f, cosa = 0.0f;
    // Xottab_DUTY: changed angle to be float instead of DWORD
    // But it must be 0xFFFFFFFF or otherwise some particles won't play
    float angle = float(0xFFFFFFFF); // XXX: check if we can replace with flt_max

    for (u32 i = p_from; i < p_to; i++)
    {
        Particle& m = particles[i];

        Fvector2 lt, rb;
        lt.set(0.f, 0.f);
        rb.set(1.f, 1.f);

        _mm_prefetch((char*)&particles[i + 1], _MM_HINT_NTA);

        if (angle != m.rot.x)
        {
            angle = m.rot.x;
            sina = std::sinf(angle);
            cosa = std::cosf(angle);
        }

        _mm_prefetch(64 + (char*)&particles[i + 1], _MM_HINT_NTA);

        if (pPE.m_Def->m_Flags.is(CPEDef::dfFramed))
            pPE.m_Def->m_Frame.CalculateTC(iFloor(float(m.frame) / 255.f), lt, rb);

        float r_x = m.size.x * 0.5f;
        float r_y = m.size.y * 0.5f;
        float speed = 0.0f;

        BOOL speed_calculated = FALSE;

        if (pPE.m_Def->m_Flags.is(CPEDef::dfVelocityScale))
        {
            magnitude_sse(m.vel, speed);
            speed_calculated = TRUE;
            r_x += speed * pPE.m_Def->m_VelocityScale.x;
            r_y += speed * pPE.m_Def->m_VelocityScale.y;
        }

        if (pPE.m_Def->m_Flags.is(CPEDef::dfAlignToPath))
        {
            if (!speed_calculated)
                magnitude_sse(m.vel, speed);

            if ((speed < EPS_S) && pPE.m_Def->m_Flags.is(CPEDef::dfWorldAlign))
            {
                Fmatrix M;
                M.setXYZ(pPE.m_Def->m_APDefaultRotation);
                if (pPE.m_RT_Flags.is(CParticleEffect::flRT_XFORM))
                {
                    Fvector p;
                    pPE.m_XFORM.transform_tiny(p, m.pos);
                    M.mulA_43(pPE.m_XFORM);
                    FillSprite(pv, M.k, M.i, p, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
                else
                {
                    FillSprite(pv, M.k, M.i, m.pos, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
            }
            else if ((speed >= EPS_S) && pPE.m_Def->m_Flags.is(CPEDef::dfFaceAlign))
            {
                Fmatrix M;
                M.identity();
                M.k.div(m.vel, speed);
                M.j.set(0, 1, 0);
                if (_abs(M.j.dotproduct(M.k)) > .99f)
                    M.j.set(0, 0, 1);
                M.i.crossproduct(M.j, M.k);
                M.i.normalize();
                M.j.crossproduct(M.k, M.i);
                M.j.normalize();

                if (pPE.m_RT_Flags.is(CParticleEffect::flRT_XFORM))
                {
                    Fvector p;
                    pPE.m_XFORM.transform_tiny(p, m.pos);
                    M.mulA_43(pPE.m_XFORM);
                    FillSprite(pv, M.j, M.i, p, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
                else
                {
                    FillSprite(pv, M.j, M.i, m.pos, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
            }
            else
            {
                Fvector dir;
                if (speed >= EPS_S)
                    dir.div(m.vel, speed);
                else
                    dir.setHP(-pPE.m_Def->m_APDefaultRotation.y, -pPE.m_Def->m_APDefaultRotation.x);

                if (pPE.m_RT_Flags.is(CParticleEffect::flRT_XFORM))
                {
                    Fvector p, d;
                    pPE.m_XFORM.transform_tiny(p, m.pos);
                    pPE.m_XFORM.transform_dir(d, dir);
                    FillSprite(pv, p, d, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
                else
                {
                    FillSprite(pv, m.pos, dir, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
                }
            }
        }
        else
        {
            if (pPE.m_RT_Flags.is(CParticleEffect::flRT_XFORM))
            {
                Fvector p;
                pPE.m_XFORM.transform_tiny(p, m.pos);
                FillSprite(pv, Device.vCameraTop, Device.vCameraRight, p, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
            }
            else
            {
                FillSprite(pv, Device.vCameraTop, Device.vCameraRight, m.pos, lt, rb, r_x, r_y, color_rgba_f(m.colorR, m.colorG, m.colorB, m.colorA), sina, cosa);
            }
        }
    }
}

void CParticleEffect::Render(CBackend& cmd_list, float, bool)
{
    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_PARTICLES))
        return;

    // Get a pointer to the particles in gp memory
    PAPI::Particle* particles;
    u32 p_cnt;
    ParticleManager()->GetParticles(m_HandleEffect, particles, p_cnt);

    if (p_cnt > 0)
    {
        if (m_Def && m_Def->m_Flags.is(CPEDef::dfSprite))
        {
            u32 dwOffset, dwCount;

            FVF::LIT* pv_start = (FVF::LIT*)RImplementation.Vertex.Lock(p_cnt * 4 * 4, geom->vb_stride, dwOffset);

            ParticleRenderStream(*this, particles, pv_start, 0, p_cnt);

            dwCount = p_cnt << 2;

            RImplementation.Vertex.Unlock(dwCount, geom->vb_stride);
            if (dwCount)
            {
                const CHUDTransformHelper initializer(cmd_list, false);

                if (GetHudMode())
                {
                    initializer.SetHUDMode();

                    RImplementation.rmNear(cmd_list);
                    ApplyTexgen(cmd_list, Device.mFullTransform);
                }

                cmd_list.set_xform_world(Fidentity);
                cmd_list.set_Geometry(geom);

                cmd_list.set_CullMode(m_Def->m_Flags.is(CPEDef::dfCulling) ? (m_Def->m_Flags.is(CPEDef::dfCullCCW) ? CULL_CCW : CULL_CW) : CULL_NONE);
                cmd_list.Render(D3DPT_TRIANGLELIST, dwOffset, 0, dwCount, 0, dwCount / 2);
                cmd_list.set_CullMode(CULL_CCW);

                if (GetHudMode())
                {
                    initializer.SetDefaultMode();

                    RImplementation.rmNormal(cmd_list);
                    ApplyTexgen(cmd_list, Device.mFullTransform);
                }
            }
        }
    }
}
