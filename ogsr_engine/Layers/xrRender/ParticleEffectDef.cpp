#include "stdafx.h"


#include "ParticleEffectDef.h"
#include "ParticleEffect.h"
#include "ParticleEffectActions.h"

//---------------------------------------------------------------------------
using namespace PAPI;
using namespace PS;

extern float ps_particle_update_coeff;

//------------------------------------------------------------------------------
// class CParticleEffectDef
//------------------------------------------------------------------------------
CPEDef::CPEDef()
{
    m_Frame.InitDefault();
    m_uStep = 33;
    m_fStep = float(m_uStep) / 1000.f;
    m_MaxParticles = 0;
    m_CachedShader = nullptr;
    m_fTimeLimit = 0.f;
    // collision
    m_fCollideOneMinusFriction = 1.f;
    m_fCollideResilience = 0.f;
    m_fCollideSqrCutoff = 0.f;
    // velocity scale
    m_VelocityScale.set(0.f, 0.f, 0.f);
    // align to path
    m_APDefaultRotation.set(-PI_DIV_2, 0.f, 0.f);
    // flags
    m_Flags.zero();
}

CPEDef::~CPEDef()
{
    for (auto& it : m_EActionList)
        xr_delete(it);
}

u32 CPEDef::GetUStep() const { return m_uStep * ps_particle_update_coeff; }

float CPEDef::GetFStep() { return m_fStep * ps_particle_update_coeff; }


void CPEDef::CreateShader()
{
    if (*m_ShaderName && *m_TextureName)
    {
        m_CachedShader.create(*m_ShaderName, *m_TextureName);
    }
    else
        Msg("! ParticleEffect [%s] with empty texture or shader. Cannot create shader for Visual!", m_Name.c_str());
}

void CPEDef::DestroyShader() { m_CachedShader.destroy(); }
void CPEDef::SetName(LPCSTR name) { m_Name = name; }

void CPEDef::ExecuteAnimate(Particle* particles, u32 p_cnt, float dt)
{
    const float speedFac = m_Frame.m_fSpeed * dt;
    for (u32 i = 0; i < p_cnt; i++)
    {
        Particle& m = particles[i];
        float f = (float(m.frame) / 255.f + ((m.flags.is(Particle::ANIMATE_CCW)) ? -1.f : 1.f) * speedFac);
        if (f > m_Frame.m_iFrameCount)
            f -= m_Frame.m_iFrameCount;
        if (f < 0.f)
            f += m_Frame.m_iFrameCount;
        m.frame = (u16)iFloor(f * 255.f);
    }
}

void CPEDef::ExecuteCollision(PAPI::Particle* particles, u32 p_cnt, float dt, CParticleEffect* owner, CollisionCallback cb)
{
    pVector pt, n;
    // Must traverse list in reverse order so Remove will work
    for (int i = p_cnt - 1; i >= 0; i--)
    {
        Particle& m = particles[i];

        bool pick_needed;
        int pick_cnt = 0;
        do
        {
            pick_needed = false;
            Fvector dir;
            dir.sub(m.pos, m.posB);
            const float dist = dir.magnitude();
            if (dist >= EPS)
            {
                dir.div(dist);

                collide::rq_result RQ;
                const collide::rq_target RT = m_Flags.is(dfCollisionDyn) ? collide::rqtBoth : collide::rqtStatic;
                if (g_pGameLevel->ObjectSpace.RayPick(m.posB, dir, dist, RT, RQ, nullptr))
                {
                    pt.mad(m.posB, dir, RQ.range);
                    if (RQ.O)
                    {
                        n.set(0.f, 1.f, 0.f);
                    }
                    else
                    {
                        const CDB::TRI* T = g_pGameLevel->ObjectSpace.GetStaticTris() + RQ.element;
                        const Fvector* verts = g_pGameLevel->ObjectSpace.GetStaticVerts();
                        n.mknormal(verts[T->verts[0]], verts[T->verts[1]], verts[T->verts[2]]);
                    }

                    pick_cnt++;
                    if (cb && (pick_cnt == 1))
                        if (!cb(owner, m, pt, n))
                            break;
                    if (m_Flags.is(dfCollisionDel))
                    {
                        ParticleManager()->RemoveParticle(owner->m_HandleEffect, i);
                    }
                    else
                    {
                        // Compute tangential and normal components of velocity
                        const float nmag = m.vel * n;
                        pVector vn(n * nmag); // Normal Vn = (V.N)N
                        pVector vt(m.vel - vn); // Tangent Vt = V - Vn

                        // Compute _new velocity heading out:
                        // Don't apply friction if tangential velocity < cutoff
                        if (vt.length2() <= m_fCollideSqrCutoff)
                        {
                            m.vel = vt - vn * m_fCollideResilience;
                        }
                        else
                        {
                            m.vel = vt * m_fCollideOneMinusFriction - vn * m_fCollideResilience;
                        }
                        m.pos = m.posB + m.vel * dt;
                        pick_needed = true;
                    }
                }
            }
            else
            {
                m.pos = m.posB;
            }
        } while (pick_needed && (pick_cnt < 2));
    }
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPEDef::Load(IReader& F)
{
    R_ASSERT(F.find_chunk(PED_CHUNK_VERSION));
    const u16 version = F.r_u16();

    if (version != PED_VERSION)
        return FALSE;

    R_ASSERT(F.find_chunk(PED_CHUNK_NAME));
    F.r_stringZ(m_Name);

    R_ASSERT(F.find_chunk(PED_CHUNK_EFFECTDATA));
    m_MaxParticles = F.r_u32();

    {
        const u32 action_list = F.find_chunk(PED_CHUNK_ACTIONLIST);
        R_ASSERT(action_list);
        m_Actions.w(F.pointer(), action_list);
    }

    F.r_chunk(PED_CHUNK_FLAGS, &m_Flags);

    if (m_Flags.is(dfSprite))
    {
        R_ASSERT(F.find_chunk(PED_CHUNK_SPRITE));
        F.r_stringZ(m_ShaderName);
        F.r_stringZ(m_TextureName);
    }

    if (m_Flags.is(dfFramed))
    {
        static_assert(sizeof(SFrame) == 28);
        R_ASSERT(F.find_chunk(PED_CHUNK_FRAME));
        F.r(&m_Frame, sizeof(SFrame));
    }

    if (m_Flags.is(dfTimeLimit))
    {
        R_ASSERT(F.find_chunk(PED_CHUNK_TIMELIMIT));
        m_fTimeLimit = F.r_float();
    }

    if (m_Flags.is(dfCollision))
    {
        R_ASSERT(F.find_chunk(PED_CHUNK_COLLISION));
        m_fCollideOneMinusFriction = F.r_float();
        m_fCollideResilience = F.r_float();
        m_fCollideSqrCutoff = F.r_float();
    }

    if (m_Flags.is(dfVelocityScale))
    {
        R_ASSERT(F.find_chunk(PED_CHUNK_VEL_SCALE));
        F.r_fvector3(m_VelocityScale);
    }

    if (m_Flags.is(dfAlignToPath))
    {
        if (F.find_chunk(PED_CHUNK_ALIGN_TO_PATH))
        {
            F.r_fvector3(m_APDefaultRotation);
        }
    }

    if (F.find_chunk(PED_CHUNK_EDATA))
    {
        m_EActionList.resize(F.r_u32());
        bool valid = false;
        for (auto& it : m_EActionList)
        {
            const PAPI::PActionEnum type = (PAPI::PActionEnum)F.r_u32();
            it = pCreateEAction(type);
            valid = it->Load(F);
            if (!valid)
                break;
        }
        //if (valid)
        //    Compile(m_EActionList);
        //else
        //    m_EActionList.clear();
    }

    return TRUE;
}

void PS::CPEDef::Compile(EPAVec& v)
{
    m_Actions.clear();
    m_Actions.w_u32(v.size());
    int cnt = 0;
    EPAVecIt it = v.begin();
    const EPAVecIt it_e = v.end();

    for (; it != it_e; ++it)
    {
        if ((*it)->flags.is(EParticleAction::flEnabled))
        {
            (*it)->Compile(m_Actions);
            cnt++;
        }
    }

    m_Actions.seek(0);
    m_Actions.w_u32(cnt);
}

BOOL CPEDef::Load2(CInifile& ini)
{
    //.	u16 version		= ini.r_u16("_effect", "version");
    if (ini.line_exist("_effect", "update_step"))
    {
        m_uStep = ini.r_u32("_effect", "update_step");
        m_fStep = float(m_uStep) / 1000.f;
    }
    m_MaxParticles = ini.r_u32("_effect", "max_particles");
    m_Flags.assign(ini.r_u32("_effect", "flags"));

    if (m_Flags.is(dfSprite))
    {
        m_ShaderName = ini.r_string("sprite", "shader");
        m_TextureName = ini.r_string("sprite", "texture");
    }

    if (m_Flags.is(dfFramed))
    {
        m_Frame.m_fTexSize = ini.r_fvector2("frame", "tex_size");
        m_Frame.reserved = ini.r_fvector2("frame", "reserved");
        m_Frame.m_iFrameDimX = ini.r_s32("frame", "dim_x");
        m_Frame.m_iFrameCount = ini.r_s32("frame", "frame_count");
        m_Frame.m_fSpeed = ini.r_float("frame", "speed");
    }

    if (m_Flags.is(dfTimeLimit))
    {
        m_fTimeLimit = ini.r_float("timelimit", "value");
    }

    if (m_Flags.is(dfCollision))
    {
        m_fCollideOneMinusFriction = ini.r_float("collision", "one_minus_friction");
        m_fCollideResilience = ini.r_float("collision", "collide_resilence");
        m_fCollideSqrCutoff = ini.r_float("collision", "collide_sqr_cutoff");
    }

    if (m_Flags.is(dfVelocityScale))
    {
        m_VelocityScale = ini.r_fvector3("velocity_scale", "value");
    }

    if (m_Flags.is(dfAlignToPath))
    {
        m_APDefaultRotation = ini.r_fvector3("align_to_path", "default_rotation");
    }

    const u32 count = ini.r_u32("_effect", "action_count");
    m_EActionList.resize(count);
    u32 action_id = 0;
    for (EPAVecIt it = m_EActionList.begin(); it != m_EActionList.end(); ++it, ++action_id)
    {
        string256 sect;
        xr_sprintf(sect, sizeof(sect), "action_%04d", action_id);
        const PAPI::PActionEnum type = (PAPI::PActionEnum)(ini.r_u32(sect, "action_type"));
        (*it) = pCreateEAction(type);
        (*it)->Load2(ini, sect);
    }

    Compile(m_EActionList);

    return TRUE;
}

void CPEDef::Save2(CInifile& ini)
{
    ini.w_u16("_effect", "version", PED_VERSION);
    ini.w_u32("_effect", "max_particles", m_MaxParticles);
    ini.w_u32("_effect", "flags", m_Flags.get());

    if (m_Flags.is(dfSprite))
    {
        ini.w_string("sprite", "shader", m_ShaderName.c_str());
        ini.w_string("sprite", "texture", m_TextureName.c_str());
    }

    if (m_Flags.is(dfFramed))
    {
        ini.w_fvector2("frame", "tex_size", m_Frame.m_fTexSize);
        ini.w_fvector2("frame", "reserved", m_Frame.reserved);
        ini.w_s32("frame", "dim_x", m_Frame.m_iFrameDimX);
        ini.w_s32("frame", "frame_count", m_Frame.m_iFrameCount);
        ini.w_float("frame", "speed", m_Frame.m_fSpeed);
    }

    if (m_Flags.is(dfTimeLimit))
    {
        ini.w_float("timelimit", "value", m_fTimeLimit);
    }

    if (m_Flags.is(dfCollision))
    {
        ini.w_float("collision", "one_minus_friction", m_fCollideOneMinusFriction);
        ini.w_float("collision", "collide_resilence", m_fCollideResilience);
        ini.w_float("collision", "collide_sqr_cutoff", m_fCollideSqrCutoff);
    }

    if (m_Flags.is(dfVelocityScale))
    {
        ini.w_fvector3("velocity_scale", "value", m_VelocityScale);
    }

    if (m_Flags.is(dfAlignToPath))
    {
        ini.w_fvector3("align_to_path", "default_rotation", m_APDefaultRotation);
    }

    ini.w_u32("_effect", "action_count", m_EActionList.size());
    u32 action_id = 0;
    for (EPAVecIt it = m_EActionList.begin(); it != m_EActionList.end(); ++it, ++action_id)
    {
        string256 sect;
        xr_sprintf(sect, sizeof(sect), "action_%04d", action_id);
        ini.w_u32(sect, "action_type", (*it)->type);
        (*it)->Save2(ini, sect);
    }

}

void CPEDef::Save(IWriter& F)
{
    F.open_chunk(PED_CHUNK_VERSION);
    F.w_u16(PED_VERSION);
    F.close_chunk();

    F.open_chunk(PED_CHUNK_NAME);
    F.w_stringZ(m_Name);
    F.close_chunk();

    F.open_chunk(PED_CHUNK_EFFECTDATA);
    F.w_u32(m_MaxParticles);
    F.close_chunk();

    F.open_chunk(PED_CHUNK_ACTIONLIST);
    F.w(m_Actions.pointer(), m_Actions.size());
    F.close_chunk();

    F.w_chunk(PED_CHUNK_FLAGS, &m_Flags, sizeof(m_Flags));

    if (m_Flags.is(dfSprite))
    {
        F.open_chunk(PED_CHUNK_SPRITE);
        F.w_stringZ(m_ShaderName);
        F.w_stringZ(m_TextureName);
        F.close_chunk();
    }

    if (m_Flags.is(dfFramed))
    {
        F.open_chunk(PED_CHUNK_FRAME);
        F.w(&m_Frame, sizeof(SFrame));
        F.close_chunk();
    }

    if (m_Flags.is(dfTimeLimit))
    {
        F.open_chunk(PED_CHUNK_TIMELIMIT);
        F.w_float(m_fTimeLimit);
        F.close_chunk();
    }

    if (m_Flags.is(dfCollision))
    {
        F.open_chunk(PED_CHUNK_COLLISION);
        F.w_float(m_fCollideOneMinusFriction);
        F.w_float(m_fCollideResilience);
        F.w_float(m_fCollideSqrCutoff);
        F.close_chunk();
    }

    if (m_Flags.is(dfVelocityScale))
    {
        F.open_chunk(PED_CHUNK_VEL_SCALE);
        F.w_fvector3(m_VelocityScale);
        F.close_chunk();
    }

    if (m_Flags.is(dfAlignToPath))
    {
        F.open_chunk(PED_CHUNK_ALIGN_TO_PATH);
        F.w_fvector3(m_APDefaultRotation);
        F.close_chunk();
    }

    F.open_chunk(PED_CHUNK_EDATA);
    F.w_u32(m_EActionList.size());
    for (const auto& it : m_EActionList)
    {
        F.w_u32(it->type);
        it->Save(F);
    }
    F.close_chunk();
}

