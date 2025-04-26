#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"
#include "ParticlesObject.h"
#include "script_engine.h"
#include "level.h"
#include "game_cl_base.h"
#include "../xr_3da/x_ray.h"
#include "../xr_3da/gamemtllib.h"
#include "PhysicsCommon.h"
#include "level_sounds.h"
#include "GamePersistent.h"

BOOL CLevel::Load_GameSpecific_Before()
{
    // AI space
    g_pGamePersistent->LoadTitle("st_loading_ai_objects");

    ASSERT_FMT(ai().get_alife(), "ai().get_alife() does not exist!");

    return (TRUE);
}

BOOL CLevel::Load_GameSpecific_After()
{
    // loading static particles
    string_path fn_game;

    if (FS.exist(fn_game, "$level$", "level.ps_static.ltx"))
    {
        CInifile ltXfile = CInifile(fn_game);

        CParticlesObject* pStaticParticles;
        LPCSTR ref_name;
        Fmatrix transform;
        for (const auto& it : ltXfile.sections())
        {
            ref_name = it.second->r_string("particle_name");
            transform.i = it.second->r_fvector3("matrix_1");
            transform.j = it.second->r_fvector3("matrix_2");
            transform.k = it.second->r_fvector3("matrix_3");
            transform.c = it.second->r_fvector3("matrix_4");

            pStaticParticles = CParticlesObject::Create(ref_name, FALSE, false);
            pStaticParticles->UpdateParent(transform, {});
            pStaticParticles->Play();
            m_StaticParticles.push_back(pStaticParticles);
        }
    }
    else if (FS.exist(fn_game, "$level$", "level.ps_static"))
    {
        IReader* F = FS.r_open(fn_game);
        CParticlesObject* pStaticParticles;
        u32 chunk = 0;
        string256 ref_name;
        Fmatrix transform;
        for (IReader* OBJ = F->open_chunk_iterator(chunk); OBJ; OBJ = F->open_chunk_iterator(chunk, OBJ))
        {
            OBJ->r_stringZ(ref_name, sizeof(ref_name));
            OBJ->r(&transform, sizeof(Fmatrix));
            transform.c.y += 0.01f;

            pStaticParticles = CParticlesObject::Create(ref_name, FALSE, false);
            pStaticParticles->UpdateParent(transform, {});
            pStaticParticles->Play();
            m_StaticParticles.push_back(pStaticParticles);
        }
        FS.r_close(F);
    }

    // loading static sounds
    VERIFY(m_level_sound_manager);
    m_level_sound_manager->Load();

    // loading sound environment
    if (FS.exist(fn_game, "$level$", "level.snd_env"))
    {
        IReader* F = FS.r_open(fn_game);
        ::Sound->set_geometry_env(F);
        FS.r_close(F);
    }

    // loading SOM
    if (FS.exist(fn_game, "$level$", "level.som"))
    {
        IReader* F = FS.r_open(fn_game);
        ::Sound->set_geometry_som(F);
        FS.r_close(F);
    }

    // loading random (around player) sounds
    if (pSettings->section_exist("sounds_random"))
    {
        CInifile::Sect& S = pSettings->r_section("sounds_random");
        Sounds_Random.reserve(S.Data.size());
        for (const auto& I : S.Data)
        {
            Sound->create(Sounds_Random.emplace_back(), I.first.c_str(), st_Effect, sg_SourceType);
        }
        Sounds_Random_dwNextTime = Device.TimerAsync() + 50000;
        Sounds_Random_Enabled = FALSE;
    }

    if (pSettings->section_exist("engine_callbacks") && pSettings->line_exist("engine_callbacks", "on_change_weather"))
        on_change_weather_callback = pSettings->r_string("engine_callbacks", "on_change_weather");

    g_pGamePersistent->Environment().SetGameTime(GetEnvironmentGameDayTimeSec(), game->GetEnvironmentGameTimeFactor());

    g_pGamePersistent->Environment().Invalidate();

    return TRUE;
}

struct translation_pair
{
    u32 m_id;
    u16 m_index;

    IC translation_pair(u32 id, u16 index)
    {
        m_id = id;
        m_index = index;
    }

    IC bool operator==(const u16& id) const { return (m_id == id); }

    IC bool operator<(const translation_pair& pair) const { return (m_id < pair.m_id); }

    IC bool operator<(const u16& id) const { return (m_id < id); }
};

void CLevel::Load_GameSpecific_CFORM(CDB::TRI* tris, u32 count)
{
    typedef xr_vector<translation_pair> ID_INDEX_PAIRS;
    ID_INDEX_PAIRS translator;
    translator.reserve(GMLib.CountMaterial());
    u16 default_id = (u16)GMLib.GetMaterialIdx("default");
    translator.push_back(translation_pair(u32(-1), default_id));

    u16 index = 0, static_mtl_count = 1;
    int max_ID = 0;
    int max_static_ID = 0;
    for (GameMtlIt I = GMLib.FirstMaterial(); GMLib.LastMaterial() != I; ++I, ++index)
    {
        if (!(*I)->Flags.test(SGameMtl::flDynamic))
        {
            ++static_mtl_count;
            translator.emplace_back((*I)->GetID(), index);
            if ((*I)->GetID() > max_static_ID)
                max_static_ID = (*I)->GetID();
        }
        if ((*I)->GetID() > max_ID)
            max_ID = (*I)->GetID();
    }
    // Msg("* Material remapping ID: [Max:%d, StaticMax:%d]",max_ID,max_static_ID);
    VERIFY(max_static_ID < 0xFFFF);

    if (static_mtl_count < 128)
    {
        CDB::TRI* I = tris;
        CDB::TRI* E = tris + count;
        for (; I != E; ++I)
        {
            ID_INDEX_PAIRS::iterator i = std::find(translator.begin(), translator.end(), (u16)(*I).material);
            if (i != translator.end())
            {
                (*I).material = (*i).m_index;
                SGameMtl* mtl = GMLib.GetMaterialByIdx((*i).m_index);
                (*I).suppress_shadows = mtl->Flags.is(SGameMtl::flSuppressShadows);
                (*I).suppress_wm = mtl->Flags.is(SGameMtl::flSuppressWallmarks);
                continue;
            }

            Debug.fatal(DEBUG_INFO, "Game material '%d' not found", (*I).material);
        }
        return;
    }

    std::sort(translator.begin(), translator.end());
    {
        CDB::TRI* I = tris;
        CDB::TRI* E = tris + count;
        for (; I != E; ++I)
        {
            ID_INDEX_PAIRS::iterator i = std::lower_bound(translator.begin(), translator.end(), (u16)(*I).material);
            if ((i != translator.end()) && ((*i).m_id == (*I).material))
            {
                (*I).material = (*i).m_index;
                SGameMtl* mtl = GMLib.GetMaterialByIdx((*i).m_index);
                (*I).suppress_shadows = mtl->Flags.is(SGameMtl::flSuppressShadows);
                (*I).suppress_wm = mtl->Flags.is(SGameMtl::flSuppressWallmarks);
                continue;
            }

            Debug.fatal(DEBUG_INFO, "Game material '%d' not found", (*I).material);
        }
    }
}
