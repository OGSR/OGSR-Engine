//---------------------------------------------------------------------------
#include "stdafx.h"


#include "SkeletonAnimated.h"

#include "AnimationKeyCalculate.h"
#include "SkeletonX.h"
#include "../../xr_3da/fmesh.h"

#ifdef DEBUG
#include "../../xrcore/dump_string.h"
#endif

extern int psSkeletonUpdate;

using namespace animation;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void CBlendInstance::construct() { Blend.clear(); }

void CBlendInstance::blend_add(CBlend* H)
{
    if (Blend.size() == MAX_BLENDED)
    {
        if (H->fall_at_end)
            return;
        BlendSVecIt _d = Blend.begin();
        for (BlendSVecIt it = Blend.begin() + 1; it != Blend.end(); it++)
            if ((*it)->blendAmount < (*_d)->blendAmount)
                _d = it;
        Blend.erase(_d);
    }
    VERIFY(Blend.size() < MAX_BLENDED);
    Blend.push_back(H);
}

void CBlendInstance::blend_remove(CBlend* H)
{
    CBlend** I = std::find(Blend.begin(), Blend.end(), H);
    if (I != Blend.end())
        Blend.erase(I);
}

// Motion control
void CKinematicsAnimated::Bone_Motion_Start(CBoneData* bd, CBlend* handle)
{
    LL_GetBlendInstance(bd->GetSelfID()).blend_add(handle);
    for (const auto& I : bd->children)
        Bone_Motion_Start(I, handle);
}
void CKinematicsAnimated::Bone_Motion_Stop(CBoneData* bd, CBlend* handle)
{
    LL_GetBlendInstance(bd->GetSelfID()).blend_remove(handle);
    for (const auto& I : bd->children)
        Bone_Motion_Stop(I, handle);
}

void CKinematicsAnimated::Bone_Motion_Start_IM(CBoneData* bd, CBlend* handle) const { LL_GetBlendInstance(bd->GetSelfID()).blend_add(handle); }
void CKinematicsAnimated::Bone_Motion_Stop_IM(CBoneData* bd, CBlend* handle) { LL_GetBlendInstance(bd->GetSelfID()).blend_remove(handle); }

std::pair<LPCSTR, LPCSTR> CKinematicsAnimated::LL_MotionDefName_dbg(MotionID ID)
{
    shared_motions& s_mots = m_Motions[ID.slot].motions;
    const auto _E = s_mots.motion_map()->end();
    for (auto _I = s_mots.motion_map()->begin(); _I != _E; ++_I)
        if (_I->second == ID.idx)
            return std::make_pair(*_I->first, *s_mots.id());
    return std::make_pair((LPCSTR)nullptr, (LPCSTR)nullptr);
}

static LPCSTR name_bool(BOOL v)
{
    constexpr xr_token token_bool[] = {{"false", 0}, {"true", 1}};
    return get_token_name(token_bool, v);
}

static LPCSTR name_blend_type(CBlend::ECurvature blend)
{
    constexpr xr_token token_blend[] = {{"eFREE_SLOT", CBlend::eFREE_SLOT}, {"eAccrue", CBlend::eAccrue}, {"eFalloff", CBlend::eFalloff}, {"eFORCEDWORD", CBlend::eFORCEDWORD}};
    return get_token_name(token_blend, blend);
}

static void dump_blend(CKinematicsAnimated* K, CBlend& B, u32 index)
{
    VERIFY(K);
    Msg("----------------------------------------------------------");
    Msg("blend index: %d, poiter: %p ", index, &B);
    Msg("time total: %f, speed: %f , power: %f ", B.timeTotal, B.speed, B.blendPower);
    Msg("ammount: %f, time current: %f, frame %d ", B.blendAmount, B.timeCurrent, B.dwFrame);
    Msg("accrue: %f, fallof: %f ", B.blendAccrue, B.blendFalloff);

    Msg("bonepart: %d, channel: %d, stop_at_end: %s, fall_at_end: %s ", B.bone_or_part, B.channel, name_bool(B.stop_at_end), name_bool(B.fall_at_end));
    Msg("state: %s, playing: %s, stop_at_end_callback: %s ", name_blend_type(B.blend_state()), name_bool(B.playing), name_bool(B.stop_at_end_callback));
    Msg("callback: %p callback param: %p", B.Callback, B.CallbackParam);

    if (B.blend_state() != CBlend::eFREE_SLOT)
    {
        Msg("motion : name %s, set: %s ", K->LL_MotionDefName_dbg(B.motionID).first, K->LL_MotionDefName_dbg(B.motionID).second);
    }
    Msg("----------------------------------------------------------");
}

void CKinematicsAnimated::LL_DumpBlends_dbg()
{
    Msg("==================dump blends=================================================");
    CBlend *I = blend_pool.begin(), *E = blend_pool.end();
    for (; I != E; I++)
        dump_blend(this, *I, u32(I - blend_pool.begin()));
}

u32 CKinematicsAnimated::LL_PartBlendsCount(u32 bone_part_id) { return blend_cycle(bone_part_id).size(); }

CBlend* CKinematicsAnimated::LL_PartBlend(u32 bone_part_id, u32 n)
{
    if (LL_PartBlendsCount(bone_part_id) <= n)
        return nullptr;
    return blend_cycle(bone_part_id)[n];
}
void CKinematicsAnimated::LL_IterateBlends(IterateBlendsCallback& callback)
{
    CBlend *I = blend_pool.begin(), *E = blend_pool.end();
    for (; I != E; I++)
        if (I->blend_state() != CBlend::eFREE_SLOT)
            callback(*I);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MotionID CKinematicsAnimated::LL_MotionID(LPCSTR B)
{
    MotionID motion_ID;
    for (int k = int(m_Motions.size()) - 1; k >= 0; --k)
    {
        shared_motions* s_mots = &m_Motions[k].motions;
        auto I = s_mots->motion_map()->find(B);
        if (I != s_mots->motion_map()->end())
        {
            motion_ID.set(u16(k), I->second);
            break;
        }
    }
    return motion_ID;
}

u16 CKinematicsAnimated::LL_PartID(LPCSTR B)
{
    if (nullptr == m_Partition)
        return BI_NONE;
    for (u16 id = 0; id < MAX_PARTS; id++)
    {
        CPartDef& P = (*m_Partition)[id];
        if (P.Name == nullptr)
            continue;
        if (0 == stricmp(B, *P.Name))
            return id;
    }
    return BI_NONE;
}

// cycles
MotionID CKinematicsAnimated::ID_Cycle(const shared_str& N)
{
    const MotionID motion_ID = ID_Cycle_Safe(N);
    ASSERT_FMT(motion_ID.valid(), "! MODEL [%s]: can't find cycle: [%s]", dbg_name.c_str(), N.c_str());
    return motion_ID;
}

MotionID CKinematicsAnimated::ID_Cycle_Safe(const shared_str& N)
{
    MotionID motion_ID;
    for (int k = int(m_Motions.size()) - 1; k >= 0; --k)
    {
        shared_motions* s_mots = &m_Motions[k].motions;
        auto I = s_mots->cycle()->find(N);
        if (I != s_mots->cycle()->end())
        {
            motion_ID.set(u16(k), I->second);
            break;
        }
    }
    return motion_ID;
}

void CKinematicsAnimated::LL_FadeCycle(u16 part, float falloff, u8 mask_channel /*= (1<<0)*/)
{
    BlendSVec& Blend = blend_cycles[part];

    for (const auto& I : Blend)
    {
        CBlend& B = *I;
        if (!(mask_channel & (1 << B.channel)))
            continue;
        // B.blend				= CBlend::eFalloff;
        B.set_falloff_state();
        B.blendFalloff = falloff;
        // B.blendAccrue		= B.timeCurrent;
        if (B.stop_at_end)
            B.stop_at_end_callback = FALSE; // callback не должен приходить!
    }
}

void CKinematicsAnimated::LL_CloseCycle(u16 part, u8 mask_channel /*= (1<<0)*/)
{
    if (BI_NONE == part)
        return;
    if (part >= MAX_PARTS)
        return;

    // destroy cycle(s)
    BlendSVecIt I = blend_cycles[part].begin(), E = blend_cycles[part].end();
    for (; I != E; I++)
    {
        CBlend& B = *(*I);
        if (!(mask_channel & (1 << B.channel)))
            continue;
        // B.blend = CBlend::eFREE_SLOT;
        B.set_free_state();

        CPartDef& P = (*m_Partition)[B.bone_or_part];
        for (const unsigned int bone : P.bones)
            Bone_Motion_Stop_IM((*bones)[bone], *I);

        blend_cycles[part].erase(I); // ?
        E = blend_cycles[part].end();
        I--;
    }
    // blend_cycles[part].clear	(); // ?
}

float CKinematicsAnimated::get_animation_length(MotionID motion_ID)
{
    VERIFY(motion_ID.slot < m_Motions.size());

    SMotionsSlot& slot = m_Motions[motion_ID.slot];

    VERIFY(LL_GetBoneRoot() < slot.bone_motions.size());

    MotionVec* bone_motions = slot.bone_motions[LL_GetBoneRoot()];

    VERIFY(motion_ID.idx < bone_motions->size());

    const CMotionDef* const m_def = slot.motions.motion_def(motion_ID.idx);

    float const anim_speed = m_def ? m_def->Speed() : 1.f;

    return bone_motions->at(motion_ID.idx).GetLength() / anim_speed;
}

void CKinematicsAnimated::IBlendSetup(CBlend& B, u16 part, u8 channel, MotionID motion_ID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)
{
    VERIFY(B.channel < MAX_CHANNELS);
    // Setup blend params
    if (bMixing)
    {
        B.set_accrue_state();
        B.blendAmount = EPS_S;
    }
    else
    {
        B.set_accrue_state();
        B.blendAmount = 1;
    }
    B.blendAccrue = blendAccrue;
    B.blendFalloff = 0; // blendFalloff used for previous cycles
    B.blendPower = 1;
    B.speed = Speed;
    B.motionID = motion_ID;
    B.timeCurrent = 0;

    const auto& motions = m_Motions[B.motionID.slot];
    auto& bone_motions = motions.bone_motions[LL_GetBoneRoot()];
    B.timeTotal = bone_motions->at(motion_ID.idx).GetLength();

    B.bone_or_part = part;
    B.stop_at_end = noloop;
    B.playing = TRUE;
    B.stop_at_end_callback = TRUE;
    B.Callback = Callback;
    B.CallbackParam = CallbackParam;

    B.channel = channel;
    B.fall_at_end = B.stop_at_end && (channel > 1);
}

void CKinematicsAnimated::IFXBlendSetup(CBlend& B, MotionID motion_ID, float blendAccrue, float blendFalloff, float Power, float Speed, u16 bone)
{
    // B.blend			= CBlend::eAccrue;
    B.set_accrue_state();
    B.blendAmount = EPS_S;
    B.blendAccrue = blendAccrue;
    B.blendFalloff = blendFalloff;
    B.blendPower = Power;
    B.speed = Speed;
    B.motionID = motion_ID;
    B.timeCurrent = 0;
    B.timeTotal = m_Motions[B.motionID.slot].bone_motions[bone]->at(motion_ID.idx).GetLength();
    B.bone_or_part = bone;

    B.playing = TRUE;
    B.stop_at_end_callback = TRUE;
    B.stop_at_end = FALSE;
    //
    B.Callback = nullptr;
    B.CallbackParam = nullptr;

    B.channel = 0;
    B.fall_at_end = FALSE;
}

CBlend* CKinematicsAnimated::LL_PlayCycle(u16 part, MotionID motion_ID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*=0*/)
{
    // validate and unroll
    if (!motion_ID.valid())
        return nullptr;
    if (BI_NONE == part)
    {
        for (u16 i = 0; i < MAX_PARTS; i++)
            LL_PlayCycle(i, motion_ID, bMixing, blendAccrue, blendFalloff, Speed, noloop, Callback, CallbackParam, channel);
        return nullptr;
    }
    if (part >= MAX_PARTS)
        return nullptr;
    if (m_Partition->part(part).Name == nullptr)
        return nullptr;

    // Process old cycles and create _new_
    if (channel == 0)
    {
        if (bMixing)
            LL_FadeCycle(part, blendFalloff, 1 << channel);
        else
            LL_CloseCycle(part, 1 << channel);
    }
    const CPartDef& P = (*m_Partition)[part];
    CBlend* B = IBlend_Create();

    IBlendSetup(*B, part, channel, motion_ID, bMixing, blendAccrue, blendFalloff, Speed, noloop, Callback, CallbackParam);
    for (const unsigned int bone : P.bones)
    {
		if (!(*bones)[bone])
			FATAL("! MODEL: missing bone/wrong armature? : %s", *getDebugName());

        Bone_Motion_Start_IM((*bones)[bone], B);
    }
    blend_cycles[part].push_back(B);
    return B;
}

CBlend* CKinematicsAnimated::LL_PlayCycle(u16 part, MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*=0*/)
{
    VERIFY(motion_ID.valid());
    const CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY(m_def);
    return LL_PlayCycle(part, motion_ID, bMixIn, m_def->Accrue(), m_def->Falloff(), m_def->Speed(), m_def->StopAtEnd(), Callback, CallbackParam, channel);
}

CBlend* CKinematicsAnimated::PlayCycle(const shared_str& N, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
    const MotionID motion_ID = ID_Cycle(N);
    if (motion_ID.valid())
    {
        return PlayCycle(motion_ID, bMixIn, Callback, CallbackParam, channel);
    }
    FATAL("! MODEL: can't find cycle: %s", N.c_str());
    return nullptr;
}

CBlend* CKinematicsAnimated::PlayCycle(MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
    VERIFY(motion_ID.valid());
    const CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY(m_def);
    return LL_PlayCycle(m_def->bone_or_part, motion_ID, bMixIn, m_def->Accrue(), m_def->Falloff(), m_def->Speed(), m_def->StopAtEnd(), Callback, CallbackParam, channel);
}

CBlend* CKinematicsAnimated::PlayCycle(u16 partition, MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
    VERIFY(motion_ID.valid());
    const CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY(m_def);
    return LL_PlayCycle(partition, motion_ID, bMixIn, m_def->Accrue(), m_def->Falloff(), m_def->Speed(), m_def->StopAtEnd(), Callback, CallbackParam, channel);
}

// fx'es
MotionID CKinematicsAnimated::ID_FX_Safe(LPCSTR N)
{
    MotionID motion_ID;
    for (int k = int(m_Motions.size()) - 1; k >= 0; --k)
    {
        shared_motions* s_mots = &m_Motions[k].motions;
        auto I = s_mots->fx()->find(N);
        if (I != s_mots->fx()->end())
        {
            motion_ID.set(u16(k), I->second);
            break;
        }
    }
    return motion_ID;
}

MotionID CKinematicsAnimated::ID_FX(LPCSTR N)
{
    const MotionID motion_ID = ID_FX_Safe(N);
    ASSERT_FMT_DBG(motion_ID.valid(), "!![%s] MODEL: can't find FX: [%s]", __FUNCTION__, N);
    return motion_ID;
}

CBlend* CKinematicsAnimated::PlayFX(MotionID motion_ID, float power_scale)
{
    if (!motion_ID.valid())
        return nullptr;

    const CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY(m_def);
    return LL_PlayFX(m_def->bone_or_part, motion_ID, m_def->Accrue(), m_def->Falloff(), m_def->Speed(), m_def->Power() * power_scale);
}

CBlend* CKinematicsAnimated::PlayFX(LPCSTR N, float power_scale)
{
    const MotionID motion_ID = ID_FX(N);
    return PlayFX(motion_ID, power_scale);
}

CBlend* CKinematicsAnimated::LL_PlayFX(u16 bone, MotionID motion_ID, float blendAccrue, float blendFalloff, float Speed, float Power)
{
    if (!motion_ID.valid())
        return nullptr;
    if (blend_fx.size() >= MAX_BLENDED)
        return nullptr;
    if (BI_NONE == bone)
        bone = iRoot;

    CBlend* B = IBlend_Create();

    IFXBlendSetup(*B, motion_ID, blendAccrue, blendFalloff, Power, Speed, bone);
    Bone_Motion_Start((*bones)[bone], B);

    blend_fx.push_back(B);
    return B;
}

void CKinematicsAnimated::DestroyCycle(CBlend& B)
{
    if (GetBlendDestroyCallback())
        GetBlendDestroyCallback()->BlendDestroy(B);
    // B.blend 		= CBlend::eFREE_SLOT;
    B.set_free_state();
    const CPartDef& P = m_Partition->part(B.bone_or_part);
    for (const unsigned int bone : P.bones)
        Bone_Motion_Stop_IM((*bones)[bone], &B);
}

// returns true if play time out
void CKinematicsAnimated::LL_UpdateTracks(float dt, bool b_force, bool leave_blends)
{
    BlendSVecIt I, E;
    // Cycles
    for (u16 part = 0; part < MAX_PARTS; part++)
    {
        if (m_Partition->part(part).Name == nullptr)
            continue;
        I = blend_cycles[part].begin();
        E = blend_cycles[part].end();
        for (; I != E; I++)
        {
            CBlend& B = *(*I);
            if (!b_force && B.dwFrame == Device.dwFrame)
                continue;
            B.dwFrame = Device.dwFrame;
            if (B.update(dt, B.Callback) && !leave_blends)
            {
                DestroyCycle(B);
                blend_cycles[part].erase(I);
                E = blend_cycles[part].end();
                I--;
            }
            // else{
            //	CMotionDef* m_def						= m_Motions[B.motionID.slot].motions.motion_def(B.motionID.idx);
            //	float timeCurrent						= B.timeCurrent;
            //	xr_vector<motion_marks>::iterator it	= m_def->marks.begin();
            //	xr_vector<motion_marks>::iterator it_e	= m_def->marks.end();
            //	for(;it!=it_e; ++it)
            //	{
            //		if( (*it).pick_mark(timeCurrent) )
            //	}
            // }
        }
    }

    LL_UpdateFxTracks(dt);
}

void CKinematicsAnimated::LL_UpdateFxTracks(float dt)
{
    // FX
    BlendSVecIt I, E;
    I = blend_fx.begin();
    E = blend_fx.end();
    for (; I != E; I++)
    {
        CBlend& B = *(*I);
        if (!B.stop_at_end_callback)
        {
            B.playing = FALSE;
            continue;
        }
        // B.timeCurrent += dt*B.speed;
        B.update_time(dt);
        switch (B.blend_state())
        {
        case CBlend::eFREE_SLOT: NODEFAULT;

        case CBlend::eAccrue:
            B.blendAmount += dt * B.blendAccrue * B.blendPower * B.speed;
            if (B.blendAmount >= B.blendPower)
            {
                // switch to fixed
                B.blendAmount = B.blendPower;
                // B.blend			= CBlend::eFalloff;//CBlend::eFixed;
                B.set_falloff_state();
            }
            break;
        case CBlend::eFalloff:
            B.blendAmount -= dt * B.blendFalloff * B.blendPower * B.speed;
            if (B.blendAmount <= 0)
            {
                // destroy fx
                // B.blend = CBlend::eFREE_SLOT;
                B.set_free_state();
                Bone_Motion_Stop((*bones)[B.bone_or_part], *I);
                blend_fx.erase(I);
                E = blend_fx.end();
                I--;
            }
            break;
        default: NODEFAULT;
        }
    }
}

void CKinematicsAnimated::UpdateTracks()
{
    if (Update_LastTime == Device.dwTimeGlobal)
        return;

    ZoneScoped;

    u32 DT = Device.dwTimeGlobal - Update_LastTime;
    if (DT > 66)
        DT = 66;
    const float dt = float(DT) / 1000.f;

    if (GetUpdateTracksCalback())
    {
        if ((*GetUpdateTracksCalback())(float(Device.dwTimeGlobal - Update_LastTime) / 1000.f, *this))
            Update_LastTime = Device.dwTimeGlobal;
        return;
    }
    Update_LastTime = Device.dwTimeGlobal;
    LL_UpdateTracks(dt, false, false);
}

void CKinematicsAnimated::Release()
{
    inherited::Release();
}

CKinematicsAnimated::~CKinematicsAnimated() { IBoneInstances_Destroy(); }
CKinematicsAnimated::CKinematicsAnimated()
    : CKinematics(), IKinematicsAnimated(), blend_instances(nullptr), m_Partition(nullptr), m_blend_destroy_callback(nullptr), m_update_tracks_callback(nullptr), Update_LastTime(0)
{}

void CKinematicsAnimated::IBoneInstances_Create()
{
    inherited::IBoneInstances_Create();
    const u32 size = bones->size();
    blend_instances = xr_alloc<CBlendInstance>(size);
    for (u32 i = 0; i < size; i++)
        blend_instances[i].construct();
}

void CKinematicsAnimated::IBoneInstances_Destroy()
{
    inherited::IBoneInstances_Destroy();
    if (blend_instances)
    {
        xr_free(blend_instances);
        blend_instances = nullptr;
    }
}

#define PCOPY(a) a = pFrom->a

void CKinematicsAnimated::Copy(dxRender_Visual* P)
{
    inherited::Copy(P);

    const CKinematicsAnimated* pFrom = (CKinematicsAnimated*)P;
    PCOPY(m_Motions);
    PCOPY(m_Partition);

    IBlend_Startup();
}

void CKinematicsAnimated::Spawn()
{
    inherited::Spawn();

    IBlend_Startup();

    for (u32 i = 0; i < bones->size(); i++)
        blend_instances[i].construct();
    m_update_tracks_callback = nullptr;
    channels.init();
}

void CKinematicsAnimated::ChannelFactorsStartup() { channels.init(); }

void CKinematicsAnimated::LL_SetChannelFactor(u16 channel, float factor) { channels.set_factor(channel, factor); }

void CKinematicsAnimated::IBlend_Startup()
{
    ZeroMemory(&blend_pool, sizeof blend_pool);
    blend_pool.resize(MAX_BLENDED_POOL);

    // cycles+fx clear
    for (auto& blend_cycle : blend_cycles)
        blend_cycle.clear();
    blend_fx.clear();
    ChannelFactorsStartup();
}

CBlend* CKinematicsAnimated::IBlend_Create()
{
    UpdateTracks();

    CBlend *I = blend_pool.begin(), *E = blend_pool.end();
    for (; I != E; I++)
        if (I->blend_state() == CBlend::eFREE_SLOT)
            return I;
    FATAL("Too many blended motions requisted");
    return nullptr;
}

void CKinematicsAnimated::Load(const char* N, IReader* data, u32 dwFlags)
{
    inherited::Load(N, data, dwFlags);

    // Globals
    blend_instances = nullptr;
    m_Partition = nullptr;
    Update_LastTime = 0;

    // Load animation
    xr_vector<xr_string> omfs;
#pragma todo("SIMP: пересмотреть это и добавить в вики.")
    auto add_omf = [&](LPCSTR nm) {
        if (strstr(nm, "*.omf"))
        {
            FS_FileSet fset;

            FS.file_list(fset, fsgame::game_meshes, FS_ListFiles, nm);
            // FS.file_list(fset, fsgame::level, FS_ListFiles, nm);

            if (strstr(Core.Params, "-dev_reference_copy"))
            {
                FS.file_list(fset, fsgame::game_meshes_reference, FS_ListFiles, nm); // FS_FileSet is actually a set )
            }
            
            omfs.reserve(omfs.size() + fset.size() - 1);

            for (const auto& it : fset)
            {
                omfs.emplace_back(it.name.c_str());
            }
        }
        else
        {
            xr_string s = nm;
            if (!strext(nm))
            {
                s += ".omf";
            }
            omfs.push_back(s);
        }
    };

    if (pUserData && pUserData->section_exist("omf_override"))
    {
        for (const auto& it : pUserData->r_section("omf_override").Ordered_Data)
        {
            LPCSTR nm = it.first.c_str();

            add_omf(nm);
        }
    }
    else if (data->find_chunk(OGF_S_MOTION_REFS))
    {
        string4096 items_nm;
        data->r_stringZ(items_nm, sizeof(items_nm));
        const u32 set_cnt = _GetItemCount(items_nm);
        for (u32 k = 0; k < set_cnt; ++k)
        {
            string_path nm;
            _GetItem(items_nm, k, nm);
            xr_strcat(nm, ".omf");
            omfs.emplace_back(nm);
        }
    }
    else if (data->find_chunk(OGF_S_MOTION_REFS2))
    {
        string_path val;

        const u32 set_cnt = data->r_u32();
        for (u32 k = 0; k < set_cnt; ++k)
        {
            data->r_stringZ(val, sizeof(val));

            LPCSTR nm = val;

            add_omf(nm);
        }
    }

#pragma todo("SIMP: пересмотреть это и добавить в вики.")
    if (const auto omf_override_ini = RImplementation.Models->omf_override_ini)
    {
        for (const auto& pair : omf_override_ini->sections())
        {
            // pair.second->Name
            if (!strncmp(N, pair.second->Name.c_str(), strlen(pair.second->Name.c_str())))
            {
                MsgDbg("Loading additional omf files for %s...", N);

                for (const auto& it : pair.second->Ordered_Data)
                {
                    LPCSTR nm = it.first.c_str();

                    add_omf(nm);
                }
            }
        }
    }

    if (omfs.size())
    {
        // R_ASSERT( omfs.size() < MAX_ANIM_SLOT );
        m_Motions.reserve(omfs.size());
        for (const auto& s : omfs)
        {
            const auto nm = s.c_str();
            string_path fn;
            if (!FS.exist(fn, fsgame::level, nm))
            {
                if (!FS.exist(fn, fsgame::game_meshes, nm))
                {
                    FATAL("Can't find motion file [%s]", nm);
                }
            }
            // Check compatibility
            m_Motions.emplace_back();
            bool create_res = true;
            if (!g_pMotionsContainer->has(nm))
            { // optimize fs operations
                IReader* MS = FS.r_open(fn);
                create_res = m_Motions.back().motions.create(nm, MS, bones);
                FS.r_close(MS);
            }
            if (create_res)
                m_Motions.back().motions.create(nm, nullptr, bones);
            else
            {
                m_Motions.pop_back();
                Msg("! error in model [%s]. Unable to load motion file '%s'.", N, nm);
            }
        }
    }
    else
    {
        string_path nm;
        strconcat(sizeof(nm), nm, N, ".ogf");
        m_Motions.emplace_back().motions.create(nm, data, bones);
    }

    R_ASSERT(m_Motions.size());

    m_Partition = m_Motions[0].motions.partition();
    m_Partition->load(this, N);

    // initialize motions
    for (auto& MS : m_Motions)
    {
        MS.bone_motions.resize(bones->size());
        for (u32 i = 0; i < bones->size(); i++)
        {
            const CBoneData* BD = (*bones)[i];
            MS.bone_motions[i] = MS.motions.bone_motions(BD->name);
        }
    }

    // Init blend pool
    IBlend_Startup();
}

void CKinematicsAnimated::LL_BuldBoneMatrixDequatize(const CBoneData* bd, u8 channel_mask, SKeyTable& keys)
{
    const u16 SelfID = bd->GetSelfID();
    CBlendInstance& BLEND_INST = LL_GetBlendInstance(SelfID);
    const CBlendInstance::BlendSVec& Blend = BLEND_INST.blend_vector();
    CKey BK[MAX_CHANNELS][MAX_BLENDED]; // base keys
    BlendSVecCIt BI;
    for (BI = Blend.begin(); BI != Blend.end(); BI++)
    {
        CBlend* B = *BI;
        int& b_count = keys.chanel_blend_conts[B->channel];
        CKey* D = &keys.keys[B->channel][b_count];
        if (!(channel_mask & (1 << B->channel)))
            continue;
        const u8 channel = B->channel;
        // keys.blend_factors[channel][b_count]	=  B->blendAmount;
        keys.blends[channel][b_count] = B;
        CMotion& M = *LL_GetMotion(B->motionID, SelfID);
        Dequantize(*D, *B, M);
        QR2Quat(M._keysR[0], BK[channel][b_count].Q);
        if (M.test_flag(flTKeyPresent))
        {
            if (M.test_flag(flTKey16IsBit))
                QT16_2T(M._keysT16[0], M, BK[channel][b_count].T);
            else
                QT8_2T(M._keysT8[0], M, BK[channel][b_count].T);
        }
        else
            BK[channel][b_count].T.set(M._initT);
        ++b_count;
    }
    for (u16 j = 0; MAX_CHANNELS > j; ++j)
        if (channels.rule(j).extern_ == animation::add)
            keys_substruct(keys.keys[j], BK[j], keys.chanel_blend_conts[j]);
}

// calculate single bone with key blending
void CKinematicsAnimated::LL_BoneMatrixBuild(CBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys)
{
    // Blend them together
    CKey channel_keys[MAX_CHANNELS];
    animation::channel_def BC[MAX_CHANNELS];
    u16 ch_count = 0;

    for (u16 j = 0; MAX_CHANNELS > j; ++j)
    {
        if (j != 0 && keys.chanel_blend_conts[j] == 0)
            continue;
        // data for channel mix cycle based on ch_count
        channels.get_def(j, BC[ch_count]);
        process_single_channel(channel_keys[ch_count], BC[ch_count], keys.keys[j], keys.blends[j], keys.chanel_blend_conts[j]);
        ++ch_count;
    }
    CKey Result;
    // Mix channels
    MixChannels(Result, channel_keys, BC, ch_count);

    Fmatrix RES;
    RES.mk_xform(Result.Q, Result.T);
    bi.mTransform.mul_43(*parent, RES);
#ifdef DEBUG
    if (!check_scale(RES))
    {
        VERIFY(check_scale(bi.mTransform));
    }
    VERIFY(_valid(bi.mTransform));
    Fbox dbg_box;
    float box_size = 100000.f;
    dbg_box.set(-box_size, -box_size, -box_size, box_size, box_size, box_size);
    // VERIFY(dbg_box.contains(bi.mTransform.c));
    VERIFY(dbg_box.contains(bi.mTransform.c), (make_string("model: %s has strange bone position, matrix : ", getDebugName().c_str()) + get_string(bi.mTransform)).c_str());

    // if(!is_similar(PrevTransform,RES,0.3f))
    //{
    //	Msg("bone %s",*bd->name)	;
    // }
    // BONE_INST.mPrevTransform.set(RES);
#endif
}

void CKinematicsAnimated::BuildBoneMatrix(const CBoneData* bd, CBoneInstance& bi, const Fmatrix* parent, u8 channel_mask /*= (1<<0)*/)
{
    SKeyTable keys;
    LL_BuldBoneMatrixDequatize(bd, channel_mask, keys);
    LL_BoneMatrixBuild(bi, parent, keys);
}

void CKinematicsAnimated::OnCalculateBones() { UpdateTracks(); }

IBlendDestroyCallback* CKinematicsAnimated::GetBlendDestroyCallback() { return m_blend_destroy_callback; }

void CKinematicsAnimated::SetUpdateTracksCalback(IUpdateTracksCallback* callback) { m_update_tracks_callback = callback; }

void CKinematicsAnimated::SetBlendDestroyCallback(IBlendDestroyCallback* cb) { m_blend_destroy_callback = cb; }
