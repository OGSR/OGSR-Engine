//---------------------------------------------------------------------------
#include "stdafx.h"

#include "GameMtlLib.h"

void DestroySounds(SoundVec& lst)
{
    for (auto& it : lst)
        it.destroy();
}
/*
void DestroyMarks(ShaderVec& lst)
{
    for (ShaderIt it=lst.begin(); lst.end() != it; ++it)
        it->destroy();
}
*/

void DestroyPSs(PSVec& lst)
{
    //	for (PSIt it=lst.begin(); lst.end() != it; ++it)
    //		Device.Resources->Delete(*it);
}

void CreateSounds(SoundVec& lst, xr_vector<std::string>& buf)
{
    int cnt = buf.size();
    R_ASSERT(cnt <= GAMEMTL_SUBITEM_COUNT + 2);
    lst.resize(cnt);
    for (int k = 0; k < cnt; ++k)
        lst[k].create(buf[k].c_str(), st_Effect, sg_SourceType);
}

void CreateSoundNames(xr_vector<std::string>& lst, LPCSTR buf)
{
    string128 tmp;
    int cnt = _GetItemCount(buf);
    R_ASSERT(cnt <= GAMEMTL_SUBITEM_COUNT + 2);
    lst.reserve(cnt);
    for (int k = 0; k < cnt; ++k)
    {
        std::string s{_GetItem(buf, k, tmp)};
        lst.push_back(s);
    }
}

/*
void CreateMarks(ShaderVec& lst, LPCSTR buf)
{
    string256	tmp;
    int cnt		=_GetItemCount(buf);	R_ASSERT(cnt<=GAMEMTL_SUBITEM_COUNT);
    ref_shader	s;
    for (int k=0; k<cnt; ++k)
    {
        s.create		("effects\\wallmark",_GetItem(buf,k,tmp));
        lst.push_back	(s);
    }
}
*/

void CreateMarks(IWallMarkArray* pMarks, LPCSTR buf)
{
    string256 tmp;
    int cnt = _GetItemCount(buf);
    R_ASSERT(cnt <= GAMEMTL_SUBITEM_COUNT);
    for (int k = 0; k < cnt; ++k)
        pMarks->AppendMark(_GetItem(buf, k, tmp));
}

void CreatePSs(PSVec& lst, LPCSTR buf)
{
    string256 tmp;
    int cnt = _GetItemCount(buf);
    R_ASSERT(cnt <= GAMEMTL_SUBITEM_COUNT);
    for (int k = 0; k < cnt; ++k)
    {
        lst.emplace_back(_GetItem(buf, k, tmp));
    }
}

SGameMtlPair::~SGameMtlPair()
{
    // destroy all media
    DestroySounds(BreakingSounds);
    DestroySounds(StepSounds);
    DestroySounds(CollideSounds);
    DestroyPSs(CollideParticles);
}

void SGameMtlPair::Load(IReader& fs)
{
    shared_str buf;

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_PAIR));
    mtl0 = fs.r_u32();
    mtl1 = fs.r_u32();
    ID = fs.r_u32();
    ID_parent = fs.r_u32();
    OwnProps.assign(fs.r_u32());

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_BREAKING));
    fs.r_stringZ(buf);
    CreateSoundNames(BreakingSoundNames, *buf);

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_STEP));
    fs.r_stringZ(buf);
    CreateSoundNames(StepSoundNames, *buf);

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_COLLIDE));
    fs.r_stringZ(buf);
    CreateSoundNames(CollideSoundNames, *buf);

    fs.r_stringZ(buf);
    CreatePSs(CollideParticles, *buf);

    fs.r_stringZ(buf);
    CreateMarks(&*m_pCollideMarks, *buf);
}

void SGameMtlPair::CreateAllSounds()
{
    CreateSounds(BreakingSounds, BreakingSoundNames);
    CreateSounds(StepSounds, StepSoundNames);
    CreateSounds(CollideSounds, CollideSoundNames);
}

void SGameMtlPair::CreateAllParticles()
{
    for (const auto& collide_particle : CollideParticles)
    {
        for (u32 i = 0; i < 4; i++) // 16 ??
        {
            IRenderVisual* vis = Render->model_CreateParticles(collide_particle.c_str(), TRUE);
            Render->model_Delete(vis);
        }
    }
}
