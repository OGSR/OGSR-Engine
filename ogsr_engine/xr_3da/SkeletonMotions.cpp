//---------------------------------------------------------------------------
#include "stdafx.h"

#include "SkeletonMotions.h"
//#include 	"SkeletonAnimated.h"
#include "Fmesh.h"
#include "motion.h"
#include "..\Include\xrRender\Kinematics.h"

motions_container* g_pMotionsContainer = 0;

u16 CPartition::part_id(const shared_str& name) const
{
    for (u16 i = 0; i < MAX_PARTS; ++i)
    {
        const CPartDef& pd = part(i);
        if (pd.Name == name)
            return i;
    }
    Msg("!there is no part named [%s]", name.c_str());
    return u16(-1);
}

void CPartition::load(IKinematics* V, LPCSTR model_name)
{
    CInifile* ini = V->LL_UserData();
    if (!ini)
        return;

    if (ini->sections().empty() || !ini->section_exist("part_0"))
        return;

    static const shared_str part_name = "partition_name";
    for (u32 i = 0; i < MAX_PARTS; ++i)
    {
        string64 buff;
        xr_sprintf(buff, sizeof(buff), "part_%d", i);

        const auto& S = ini->r_section(buff);
        if (!S.Data.empty())
            P[i].bones.clear();

        for (const auto& [k, v] : S.Data)
        {
            if (k == part_name)
                P[i].Name = v;
            else
                P[i].bones.push_back(V->LL_BoneID(k));
        }
    }
}

u16 find_bone_id(vecBones* bones, shared_str nm)
{
    for (u16 i = 0; i < (u16)bones->size(); i++)
        if (bones->at(i)->name == nm)
            return i;
    return BI_NONE;
}

//-----------------------------------------------------------------------
BOOL motions_value::load(LPCSTR N, IReader* data, vecBones* bones)
{
    m_id = N;

    bool bRes = true;
    // Load definitions
    U16Vec rm_bones(bones->size(), BI_NONE);
    IReader* MP = data->open_chunk(OGF_S_SMPARAMS);

    if (MP)
    {
        u16 vers = MP->r_u16();
        u16 part_bone_cnt = 0;
        string128 buf;
        R_ASSERT3(vers <= xrOGF_SMParamsVersion, "Invalid OGF/OMF version:", N);

        // partitions
        u16 part_count;
        part_count = MP->r_u16();

        for (u16 part_i = 0; part_i < part_count; part_i++)
        {
            CPartDef& PART = m_partition[part_i];
            MP->r_stringZ(buf, sizeof(buf));
            PART.Name = _strlwr(buf);
            PART.bones.resize(MP->r_u16());

            for (xr_vector<u32>::iterator b_it = PART.bones.begin(); b_it < PART.bones.end(); b_it++)
            {
                MP->r_stringZ(buf, sizeof(buf));
                u16 m_idx = u16(MP->r_u32());
                *b_it = find_bone_id(bones, buf);
                VERIFY3(*b_it != BI_NONE, "Can't find bone:", buf);
                if (bRes)
                    rm_bones[m_idx] = u16(*b_it);
            }
            part_bone_cnt = u16(part_bone_cnt + (u16)PART.bones.size());
        }

        VERIFY3(part_bone_cnt == (u16)bones->size(), "Different bone count '%s'", N);

        if (bRes)
        {
            // motion defs (cycle&fx)
            u16 mot_count = MP->r_u16();
            m_mdefs.resize(mot_count);

            m_motion_map.reserve(mot_count);
            m_cycle.reserve(mot_count);
            m_fx.reserve(mot_count);

            for (u16 mot_i = 0; mot_i < mot_count; mot_i++)
            {
                MP->r_stringZ(buf, sizeof(buf));
                shared_str nm = _strlwr(buf);
                R_ASSERT(MP->elapsed() >= static_cast<int>(sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float)),
                         "[%s] Something strange with file [%s]. This file broken!", __FUNCTION__, N);
                u32 dwFlags = MP->r_u32();
                CMotionDef& D = m_mdefs[mot_i];
                D.Load(MP, dwFlags, vers);

                if (dwFlags & esmFX)
                    m_fx.emplace(nm, mot_i);
                else
                    m_cycle.emplace(nm, mot_i);

                m_motion_map.emplace(nm, mot_i);
            }
        }
        MP->close();
    }
    else
    {
        Debug.fatal(DEBUG_INFO, "Old skinned model version unsupported! (%s)", N);
    }
    if (!bRes)
        return false;

    // Load animation
    IReader* MS = data->open_chunk(OGF_S_MOTIONS);
    if (!MS)
        return false;

    u32 dwCNT = 0;
    MS->r_chunk_safe(0, &dwCNT, sizeof(dwCNT));
    VERIFY(dwCNT < 0x3FFF); // MotionID 2 bit - slot, 14 bit - motion index

    m_motions.reserve(bones->size());

    // set per bone motion size
    for (u32 i = 0; i < bones->size(); i++)
        m_motions[bones->at(i)->name].resize(dwCNT);

    // load motions
    for (u16 m_idx = 0; m_idx < (u16)dwCNT; m_idx++)
    {
        string128 mname;
        R_ASSERT(MS->find_chunk(m_idx + 1));
        MS->r_stringZ(mname, sizeof(mname));
#ifdef DEBUG
        // sanity check
        xr_strlwr(mname);
        auto I = m_motion_map.find(mname);
        VERIFY3(I != m_motion_map.end(), "Can't find motion:", mname);
        VERIFY3(I->second == m_idx, "Invalid motion index:", mname);
#endif
        u32 dwLen = MS->r_u32();
        for (u32 i = 0; i < bones->size(); i++)
        {
            u16 bone_id = rm_bones[i];
            VERIFY2(bone_id != BI_NONE, "Invalid remap index.");
            CMotion& M = m_motions[bones->at(bone_id)->name][m_idx];
            M.set_count(dwLen);
            M.set_flags(MS->r_u8());

            if (M.test_flag(flRKeyAbsent))
            {
                CKeyQR* r = (CKeyQR*)MS->pointer();
                //u32 crc_q = crc32(r, sizeof(CKeyQR));
                M._keysR.create(1, r);
                MS->advance(1 * sizeof(CKeyQR));
            }
            else
            {
                /*u32 crc_q = */MS->r_u32();
                M._keysR.create(dwLen, (CKeyQR*)MS->pointer());
                MS->advance(dwLen * sizeof(CKeyQR));
            }
            if (M.test_flag(flTKeyPresent))
            {
                /*u32 crc_t = */MS->r_u32();
                if (M.test_flag(flTKey16IsBit))
                {
                    M._keysT16.create(dwLen, (CKeyQT16*)MS->pointer());
                    MS->advance(dwLen * sizeof(CKeyQT16));
                }
                else
                {
                    M._keysT8.create(dwLen, (CKeyQT8*)MS->pointer());
                    MS->advance(dwLen * sizeof(CKeyQT8));
                };

                MS->r_fvector3(M._sizeT);
                MS->r_fvector3(M._initT);
            }
            else
            {
                MS->r_fvector3(M._initT);
            }
        }
    }
    //	Msg("Motions %d/%d %4d/%4d/%d, %s",p_cnt,m_cnt, m_load,m_total,m_r,N);
    MS->close();

    return bRes;
}

MotionVec* motions_value::bone_motions(shared_str bone_name)
{
    auto I = m_motions.find(bone_name);
    if (I == m_motions.end())
        return nullptr;

    return &(*I).second;
}
//-----------------------------------
motions_container::motions_container() {}

motions_container::~motions_container()
{
    //	clean	(false);
    //	clean	(true);
    //	dump	();
    VERIFY(container.empty());
}

bool motions_container::has(shared_str key) { return (container.find(key) != container.end()); }

motions_value* motions_container::dock(shared_str key, IReader* data, vecBones* bones)
{
    motions_value* result = 0;
    auto I = container.find(key);
    if (I != container.end())
        result = I->second;
    if (0 == result)
    {
        // loading motions
        VERIFY(data);
        result = xr_new<motions_value>();
        result->m_dwReference = 0;
        BOOL bres = result->load(key.c_str(), data, bones);
        if (bres)
            container.emplace(key, result);
        else
            xr_delete(result);
    }
    return result;
}
void motions_container::clean(bool force_destroy)
{
    auto it = container.begin();
    auto _E = container.end();
    if (force_destroy)
    {
        for (; it != _E; it++)
        {
            motions_value* sv = it->second;
            xr_delete(sv);
        }
        container.clear();
    }
    else
    {
        for (; it != _E;)
        {
            motions_value* sv = it->second;
            if (0 == sv->m_dwReference)
            {
                auto i_current = it;
                auto i_next = ++it;
                xr_delete(sv);
                container.erase(i_current);
                it = i_next;
            }
            else
            {
                it++;
            }
        }
    }
}
void motions_container::dump()
{
    auto it = container.begin();
    auto _E = container.end();
    Log("--- motion container --- begin:");
    u32 sz = sizeof(*this);
    for (u32 k = 0; it != _E; k++, it++)
    {
        sz += it->second->mem_usage();
        Msg("#%3d: [%3d/%5d Kb] - %s", k, it->second->m_dwReference, it->second->mem_usage() / 1024, it->first.c_str());
    }
    Msg("--- items: %d, mem usage: %d Kb ", container.size(), sz / 1024);
    Log("--- motion container --- end.");
}

//////////////////////////////////////////////////////////////////////////
// High level control
void CMotionDef::Load(IReader* MP, u32 fl, u16 version)
{
    // params
    bone_or_part = MP->r_u16(); // bCycle?part_id:bone_id;
    motion = MP->r_u16(); // motion_id
    speed = MP->r_float();
    power = MP->r_float();
    accrue = MP->r_float();
    falloff = MP->r_float();
    flags = (u16)fl;
    constexpr float fQuantizerRangeExt = 1.5f; //Какое-то магическое число
    /*//Dbg
        Log("############################################################################");
        constexpr auto Dequantize = [](u16 V) { return  float(V) / 655.35f; };
        auto Quantize = [](float V) { s32 t = iFloor(V * 655.35f); clamp(t, 0, 65535); return u16(t); };
        Msg("!![%s] speed: [%f], power: [%f], accrue: [%f], fallof: [%f]", __FUNCTION__, Dequantize(Quantize(speed)), Dequantize(Quantize(power)), fQuantizerRangeExt *
       Dequantize(Quantize(accrue)), fQuantizerRangeExt * Dequantize(Quantize(falloff))); Msg("--[%s] speed: [%f], power: [%f], accrue: [%f], fallof: [%f]", __FUNCTION__, speed,
       power, fQuantizerRangeExt * accrue, fQuantizerRangeExt * falloff); if (!(flags & esmFX) && (Quantize(falloff) >= Quantize(accrue))) Msg("!![%s] fallof set to [%f]",
       __FUNCTION__, fQuantizerRangeExt * Dequantize(u16(Quantize(accrue) - 1)));
    */
    if (!(flags & esmFX) && (falloff >= accrue))
    {
        falloff = accrue /* - 0.003f*/; // KRodin: 0.003f наиболее приближённо к тому что было до этого. Разница в результате буквально в тысячных долях. При Quantize/Dequantize
                                        // точность в любом случае терялась, так что это не сильно важно.
        if (/*negative(falloff)*/ negative(accrue - 0.003f))
            falloff = 100.f; //И вообще это были какие-то костыли от ПЫС. Если при вычитании falloff становился меньше нуля (при том что он был unsigned!!!), то после
                             //Quantize/Dequantize всегда получалось 100.
        // Msg("--[%s] fallof set to [%f]", __FUNCTION__, fQuantizerRangeExt * falloff);
    }
    // Log("############################################################################");
    accrue *= fQuantizerRangeExt;
    falloff *= fQuantizerRangeExt;

    if (version >= 4)
    {
        u32 cnt = MP->r_u32();
        if (cnt > 0)
        {
            marks.resize(cnt);

            for (u32 i = 0; i < cnt; ++i)
                marks[i].Load(MP);
        }
    }
}

bool CMotionDef::StopAtEnd() const { return !!(flags & esmStopAtEnd); }

bool shared_motions::create(shared_str key, IReader* data, vecBones* bones)
{
    motions_value* v = g_pMotionsContainer->dock(key, data, bones);
    if (0 != v)
        v->m_dwReference++;
    destroy();
    p_ = v;
    return (0 != v);
}

bool shared_motions::create(shared_motions const& rhs)
{
    motions_value* v = rhs.p_;
    if (0 != v)
        v->m_dwReference++;
    destroy();
    p_ = v;
    return (0 != v);
}

const motion_marks::interval* motion_marks::pick_mark(const float& t) const
{
    C_ITERATOR it = intervals.begin();
    C_ITERATOR it_e = intervals.end();

    for (; it != it_e; ++it)
    {
        const interval& I = (*it);
        if (I.first <= t && I.second >= t)
            return &I;

        if (I.first > t)
            break;
    }
    return NULL;
}

bool motion_marks::is_mark_between(float const& t0, float const& t1) const
{
    VERIFY(t0 <= t1);

    C_ITERATOR i = intervals.begin();
    C_ITERATOR e = intervals.end();
    for (; i != e; ++i)
    {
        VERIFY((*i).first <= (*i).second);

        if ((*i).first == t0)
            return (true);

        if ((*i).first > t0)
        {
            if ((*i).second <= t1)
                return (true);

            if ((*i).first <= t1)
                return (true);

            return (false);
        }

        if ((*i).second < t0)
            continue;

        if ((*i).second == t0)
            return (true);

        return (true);
    }

    return (false);
}

float motion_marks::time_to_next_mark(float time) const
{
    C_ITERATOR i = intervals.begin();
    C_ITERATOR e = intervals.end();
    float result_dist = FLT_MAX;
    for (; i != e; ++i)
    {
        float dist = (*i).first - time;
        if (dist > 0.f && dist < result_dist)
            result_dist = dist;
    }
    return result_dist;
}

void ENGINE_API motion_marks::Load(IReader* R)
{
    xr_string tmp;
    R->r_string(tmp);
    name = tmp.c_str();
    u32 cnt = R->r_u32();
    intervals.resize(cnt);
    for (u32 i = 0; i < cnt; ++i)
    {
        interval& item = intervals[i];
        item.first = R->r_float();
        item.second = R->r_float();
    }
}
