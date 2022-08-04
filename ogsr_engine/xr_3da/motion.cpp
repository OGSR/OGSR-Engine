#include "stdafx.h"

#include "motion.h"
#include "envelope.h"

#define EOBJ_OMOTION 0x1100
#define EOBJ_SMOTION 0x1200
#define EOBJ_OMOTION_VERSION 0x0005
#define EOBJ_SMOTION_VERSION 0x0007

#ifdef _LW_EXPORT
extern void ReplaceSpaceAndLowerCase(shared_str& s);
#endif

//------------------------------------------------------------------------------------------
// CCustomMotion
//------------------------------------------------------------------------------------------
CCustomMotion::CCustomMotion()
{
    iFrameStart = 0;
    iFrameEnd = 0;
    fFPS = 30.f;
}

CCustomMotion::CCustomMotion(CCustomMotion* source) { *this = *source; }

CCustomMotion::~CCustomMotion() {}

void CCustomMotion::Save(IWriter& F)
{
#ifdef _LW_EXPORT
    ReplaceSpaceAndLowerCase(name);
#endif
    F.w_stringZ(name);
    F.w_u32(iFrameStart);
    F.w_u32(iFrameEnd);
    F.w_float(fFPS);
}

bool CCustomMotion::Load(IReader& F)
{
    F.r_stringZ(name);
    iFrameStart = F.r_u32();
    iFrameEnd = F.r_u32();
    fFPS = F.r_float();
    return true;
}

//------------------------------------------------------------------------------------------
// Object Motion
//------------------------------------------------------------------------------------------
COMotion::COMotion() : CCustomMotion()
{
    mtype = mtObject;
    for (int ch = 0; ch < ctMaxChannel; ch++)
        envs[ch] = xr_new<CEnvelope>();
}

COMotion::COMotion(COMotion* source) : CCustomMotion(source)
{
    // bone motions
    for (int ch = 0; ch < ctMaxChannel; ch++)
        envs[ch] = xr_new<CEnvelope>(source->envs[ch]);
}

COMotion::~COMotion() { Clear(); }

void COMotion::Clear()
{
    for (int ch = 0; ch < ctMaxChannel; ch++)
        xr_delete(envs[ch]);
}

void COMotion::_Evaluate(float t, Fvector& T, Fvector& R)
{
    T.x = envs[ctPositionX]->Evaluate(t);
    T.y = envs[ctPositionY]->Evaluate(t);
    T.z = envs[ctPositionZ]->Evaluate(t);

    R.y = envs[ctRotationH]->Evaluate(t);
    R.x = envs[ctRotationP]->Evaluate(t);
    R.z = envs[ctRotationB]->Evaluate(t);
}

void COMotion::SaveMotion(const char* buf)
{
    CMemoryWriter F;
    F.open_chunk(EOBJ_OMOTION);
    Save(F);
    F.close_chunk();
    if (!F.save_to(buf))
        Msg("!Can't save object motion: [%s]", buf);
}

bool COMotion::LoadMotion(const char* buf)
{
    destructor<IReader> F(FS.r_open(buf));
    R_ASSERT(F().find_chunk(EOBJ_OMOTION));
    return Load(F());
}

void COMotion::Save(IWriter& F)
{
    CCustomMotion::Save(F);
    F.w_u16(EOBJ_OMOTION_VERSION);
    for (int ch = 0; ch < ctMaxChannel; ch++)
        envs[ch]->Save(F);
}

bool COMotion::Load(IReader& F)
{
    CCustomMotion::Load(F);
    u16 vers = F.r_u16();
    if (vers == 0x0003)
    {
        Clear();
        for (int ch = 0; ch < ctMaxChannel; ch++)
        {
            envs[ch] = xr_new<CEnvelope>();
            envs[ch]->Load_1(F);
        }
    }
    else if (vers == 0x0004)
    {
        Clear();
        envs[ctPositionX] = xr_new<CEnvelope>();
        envs[ctPositionX]->Load_2(F);
        envs[ctPositionY] = xr_new<CEnvelope>();
        envs[ctPositionY]->Load_2(F);
        envs[ctPositionZ] = xr_new<CEnvelope>();
        envs[ctPositionZ]->Load_2(F);
        envs[ctRotationP] = xr_new<CEnvelope>();
        envs[ctRotationP]->Load_2(F);
        envs[ctRotationH] = xr_new<CEnvelope>();
        envs[ctRotationH]->Load_2(F);
        envs[ctRotationB] = xr_new<CEnvelope>();
        envs[ctRotationB]->Load_2(F);
    }
    else
    {
        if (vers != EOBJ_OMOTION_VERSION)
            return false;
        Clear();
        for (int ch = 0; ch < ctMaxChannel; ch++)
        {
            envs[ch] = xr_new<CEnvelope>();
            envs[ch]->Load_2(F);
        }
    }
    return true;
}

void SAnimParams::Set(float start_frame, float end_frame, float fps)
{
    min_t = start_frame / fps;
    max_t = end_frame / fps;
}

void SAnimParams::Set(CCustomMotion* M)
{
    Set((float)M->FrameStart(), (float)M->FrameEnd(), M->FPS());
    t = min_t;
    //    bPlay=true;
}
void SAnimParams::Update(float dt, float speed, bool loop)
{
    if (!bPlay)
        return;
    bWrapped = false;
    t += speed * dt;
    if (t > max_t)
    {
        bWrapped = true;
        if (loop)
        {
            float len = max_t - min_t;
            float k = float(iFloor((t - min_t) / len));
            t = t - k * len;
        }
        else
            t = max_t;
    }
}

//------------------------------------------------------------------------------
// Clip
//------------------------------------------------------------------------------
#define EOBJ_CLIP_VERSION 2
#define EOBJ_CLIP_VERSION_CHUNK 0x9000
#define EOBJ_CLIP_DATA_CHUNK 0x9001

void CClip::Save(IWriter& F)
{
    F.open_chunk(EOBJ_CLIP_VERSION_CHUNK);
    F.w_u16(EOBJ_CLIP_VERSION);
    F.close_chunk();

    F.open_chunk(EOBJ_CLIP_DATA_CHUNK);
    F.w_stringZ(name);
    for (int k = 0; k < 4; k++)
    {
        F.w_stringZ(cycles[k].name);
        F.w_u16(cycles[k].slot);
    }
    F.w_stringZ(fx.name);
    F.w_u16(fx.slot);
    F.w_float(fx_power);
    F.w_float(length);
    F.close_chunk();
}
//------------------------------------------------------------------------------

bool CClip::Load(IReader& F)
{
    R_ASSERT(F.find_chunk(EOBJ_CLIP_VERSION_CHUNK));
    u16 ver = F.r_u16();
    if (ver != EOBJ_CLIP_VERSION)
        return false;
    R_ASSERT(F.find_chunk(EOBJ_CLIP_DATA_CHUNK));
    F.r_stringZ(name);
    for (int k = 0; k < 4; k++)
    {
        F.r_stringZ(cycles[k].name);
        cycles[k].slot = F.r_u16();
    }
    F.r_stringZ(fx.name);
    fx.slot = F.r_u16();
    fx_power = F.r_float();
    length = F.r_float();
    return true;
}
//------------------------------------------------------------------------------

bool CClip::Equal(CClip* c)
{
    if (!name.equal(c->name))
        return false;
    if (!cycles[0].equal(c->cycles[0]))
        return false;
    if (!cycles[1].equal(c->cycles[1]))
        return false;
    if (!cycles[2].equal(c->cycles[2]))
        return false;
    if (!cycles[3].equal(c->cycles[3]))
        return false;
    if (!fx.equal(c->fx))
        return false;
    if (length != c->length)
        return false;
    return true;
}
//------------------------------------------------------------------------------
