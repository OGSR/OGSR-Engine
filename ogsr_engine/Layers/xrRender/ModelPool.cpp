#include "stdafx.h"

#include "ModelPool.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/fmesh.h"
#include "fhierrarhyvisual.h"
#include "SkeletonAnimated.h"
#include "fvisual.h"
#include "fprogressive.h"
#include "fskinned.h"
#include "flod.h"
#include "ftreevisual.h"
#include "ParticleGroup.h"
#include "ParticleEffect.h"

void fix_name(const char* name, string_path& low_name)
{
    VERIFY(xr_strlen(name) < sizeof(low_name));
    xr_strcpy(low_name, name);
    strlwr(low_name);
    if (strext(low_name))
        *strext(low_name) = 0;
}

dxRender_Visual* CModelPool::Instance_Create(u32 type)
{
    dxRender_Visual* V = nullptr;

    // Check types
    switch (type)
    {
    case MT_NORMAL: // our base visual
        V = xr_new<Fvisual>();
        break;
    case MT_HIERRARHY: 
        V = xr_new<FHierrarhyVisual>();
        break;
    case MT_PROGRESSIVE: // dynamic-resolution visual
        V = xr_new<FProgressive>();
        break;
    case MT_SKELETON_ANIM: 
        V = xr_new<CKinematicsAnimated>();
        break;
    case MT_SKELETON_RIGID: 
        V = xr_new<CKinematics>();
        break;
    case MT_SKELETON_GEOMDEF_PM: 
        V = xr_new<CSkeletonX_PM>();
        break;
    case MT_SKELETON_GEOMDEF_ST: 
        V = xr_new<CSkeletonX_ST>();
        break;
    case MT_TREE_ST: 
        V = xr_new<FTreeVisual_ST>();
        break;
    case MT_TREE_PM: 
        V = xr_new<FTreeVisual_PM>();
        break;
    case MT_PARTICLE_EFFECT: 
        V = xr_new<PS::CParticleEffect>();
        break;
    case MT_PARTICLE_GROUP: 
        V = xr_new<PS::CParticleGroup>();
        break;
    case MT_LOD: 
        V = xr_new<FLOD>();
        break;

    default: 
        FATAL("Unknown visual type");
        break;
    }

    R_ASSERT(V);
    V->Type = type;

    return V;
}

dxRender_Visual* CModelPool::Instance_Duplicate(dxRender_Visual* V)
{
    R_ASSERT(V);
    dxRender_Visual* N = Instance_Create(V->Type);
    N->Copy(V);
    N->Spawn();
    // inc ref counter
    for (auto& Model : Models)
        if (Model.model == V)
        {
            Model.refs++;
            break;
        }
    return N;
}

dxRender_Visual* CModelPool::Instance_Load(LPCSTR N, BOOL allow_register)
{
    string_path fn;
    string_path name;

    // Add default ext if no ext at all
    if (nullptr == strext(N))
        strconcat(sizeof(name), name, N, ".ogf");
    else
        xr_strcpy(name, sizeof(name), N);

    // Load data from MESHES or LEVEL
    if (!FS.exist(fn, fsgame::level, name))
    {
        if (!FS.exist(fn, fsgame::game_meshes, name))
        {
            FATAL("Can't find model file [%s]", name);
        }
    }

    // Actual loading
#ifdef DEBUG
    if (bLogging)
        Msg("- Uncached model loading: %s", fn);
#endif // DEBUG

    IReader* data = FS.r_open(fn);

    dxRender_Visual* V;

    ogf_header H;
    data->r_chunk_safe(OGF_HEADER, &H, sizeof(H));
    V = Instance_Create(H.type);
    V->Load(N, data, 0);
    FS.r_close(data);

    g_pGamePersistent->RegisterModel(V);

    // Registration
    if (allow_register)
        Instance_Register(N, V);

    return V;
}

dxRender_Visual* CModelPool::Instance_Load(LPCSTR N, IReader* data, BOOL allow_register)
{
    dxRender_Visual* V;

    ogf_header H;
    data->r_chunk_safe(OGF_HEADER, &H, sizeof(H));
    V = Instance_Create(H.type);
    V->Load(N, data, 0);

    // Registration
    if (allow_register)
        Instance_Register(N, V);

    return V;
}

void CModelPool::Instance_Register(LPCSTR N, dxRender_Visual* V)
{
    // Registration
    ModelDef M;
    M.name = N;
    M.model = V;
    Models.push_back(M);
}

void CModelPool::Destroy()
{
    R_ASSERT(Pool.empty()); // pool should be clear at this point

    // Registry
    while (!Registry.empty())
    {
        const REGISTRY_IT it = Registry.begin();
        dxRender_Visual* V = (dxRender_Visual*)it->first;
#ifdef DEBUG
        Msg("ModelPool: Destroy object: '%s'", *V->dbg_name);
#endif
        DeleteInternal(V, TRUE);
    }

    // Base/Reference
    xr_vector<ModelDef>::iterator I = Models.begin();
    const xr_vector<ModelDef>::iterator E = Models.end();
    for (; I != E; ++I)
    {
        I->model->Release();
        xr_delete(I->model);
    }

    Models.clear();

    // cleanup motions container
    g_pMotionsContainer->clean(false);

    if (vis_prefetch_ini)
        vis_prefetch_ini->save_as();

    m_prefetched.clear();
}

CModelPool::CModelPool()
{
    bLogging = TRUE;
    bForceDiscard = FALSE;
    bAllowChildrenDuplicate = TRUE;
    g_pMotionsContainer = xr_new<motions_container>();

    string_path fname;
    FS.update_path(fname, fsgame::app_data_root, "vis_prefetch.ltx");

    if (IReader* F = FS.r_open(fname))
    {
        // Костыль от ситуации когда в редких случаях почему-то у игроков бьётся vis_prefetch.ltx - оказывается набит нулями, в результате чего игра не
        // запускается. Не понятно почему так происходит, поэтому сделал тут обработку такой ситуации.

        if (F->elapsed() >= sizeof(u8) && F->r_u8() == 0) 
        {
            Msg("!![%s] file [%s] broken!", __FUNCTION__, fname);

            FS.r_close(F);

            FS.file_delete(fname);

            F = nullptr;
        }

        if (F)
            FS.r_close(F);
    }

    vis_prefetch_ini = xr_new<CInifile>(fname, FALSE);

    //process_vis_prefetch();
#pragma todo("SIMP: проверить все эти оверрайды, добавить в вики")
    if (pSettings->section_exist("visual_bones_override") && pSettings->line_exist("visual_bones_override", "file"))
    {
        string_path fname;
        FS.update_path(fname, fsgame::game_configs, pSettings->r_string("visual_bones_override", "file"));

        if (FS.exist(fname))
        {
            bone_override_ini = xr_new<CInifile>(fname);
        }
    }

    if (pSettings->section_exist("omf_override") && pSettings->line_exist("omf_override", "file"))
    {
        string_path fname;
        FS.update_path(fname, fsgame::game_configs, pSettings->r_string("omf_override", "file"));

        if (FS.exist(fname))
        {
            omf_override_ini = xr_new<CInifile>(fname);
        }
    }

    if (pSettings->section_exist("visual_userdata_override") && pSettings->line_exist("visual_userdata_override", "file"))
    {
        string_path fname;
        FS.update_path(fname, fsgame::game_configs, pSettings->r_string("visual_userdata_override", "file"));

        if (FS.exist(fname))
        {
            userdata_override_ini = xr_new<CInifile>(fname);
        }
    }
}

CModelPool::~CModelPool()
{
    Destroy();

    xr_delete(g_pMotionsContainer);
    xr_delete(vis_prefetch_ini);

    xr_delete(bone_override_ini);
    xr_delete(omf_override_ini);
    xr_delete(userdata_override_ini);
}

dxRender_Visual* CModelPool::Instance_Find(LPCSTR N)
{
    dxRender_Visual* Model = nullptr;
    xr_vector<ModelDef>::iterator I;
    for (I = Models.begin(); I != Models.end(); ++I)
    {
        if (I->name[0] && (0 == xr_strcmp(*I->name, N)))
        {
            Model = I->model;
            break;
        }
    }
    return Model;
}

dxRender_Visual* CModelPool::Create(const char* name, IReader* data)
{
    string_path low_name;
    fix_name(name, low_name);

    dxRender_Visual* Model{};

    {
        ModelsPool_lock.lock();

        // 0. Search POOL
        const POOL_IT it = Pool.find(low_name);
        if (it != Pool.end())
        {
            // 1. Instance found
            Model = it->second;
            Model->Spawn();
            Pool.erase(it);
            refresh_prefetch(low_name, Model->IsHudVisual);
        }

        ModelsPool_lock.unlock();
    }

    if (!Model)
    {
        // 1. Search for already loaded model (reference, base model)
        dxRender_Visual* Base = Instance_Find(low_name);

        if (nullptr == Base)
        {
            const bool prefetch = Device.dwPrecacheFrame > 0;

            // 2. If not found
            bAllowChildrenDuplicate = FALSE;
            if (data)
                Base = Instance_Load(low_name, data, TRUE);
            else
                Base = Instance_Load(low_name, TRUE);
            bAllowChildrenDuplicate = TRUE;

            if (!prefetch)
                MsgDbg("~ %s name=%s", __FUNCTION__, low_name);
        }

        // 3. If found - return (cloned) reference
        Model = Instance_Duplicate(Base);

        refresh_prefetch(low_name, Model->IsHudVisual);

        Registry_lock.lock();
        Registry.emplace(Model, low_name);
        Registry_lock.unlock();
    }

    return Model;
}

void CModelPool::refresh_prefetch(const char* low_name, const bool is_hud_visual)
{
    if (now_prefetch2)
        return;

    std::string s(low_name);
    if (m_prefetched.find(s) != m_prefetched.end())
        return;

    if (now_prefetch1)
    {
        m_prefetched.emplace(s, true);
    }
    else if (vis_prefetch_ini)
    {
        shared_str fname;
        bool is_global = !!FS.exist(fsgame::game_meshes, *fname.sprintf("%s.ogf", low_name));
        if (is_global)
            vis_prefetch_ini->w_fvector2("prefetch", low_name, Fvector2{2.f, is_hud_visual ? 2.f : 1.f});
    }
}

dxRender_Visual* CModelPool::CreateChild(LPCSTR name, IReader* data)
{
    string_path low_name;
    fix_name(name, low_name);

    // 1. Search for already loaded model
    dxRender_Visual* Base = Instance_Find(low_name);
    //.	if (0==Base) Base	 	= Instance_Load(name,data,FALSE);
    if (nullptr == Base)
    {
        if (data)
            Base = Instance_Load(low_name, data, FALSE);
        else
            Base = Instance_Load(low_name, FALSE);
    }

    dxRender_Visual* Model = bAllowChildrenDuplicate ? Instance_Duplicate(Base) : Base;
    return Model;
}

extern BOOL ENGINE_API g_bRendering;

void CModelPool::DeleteInternal(dxRender_Visual*& V, BOOL bDiscard)
{
    VERIFY(!g_bRendering);
    if (!V)
        return;

    V->Depart();

    if (bDiscard || bForceDiscard)
    {
        Discard(V, TRUE);
    }
    else
    {
        //
        const REGISTRY_IT it = Registry.find(V);
        if (it != Registry.end())
        {
            // Registry entry found - move it to pool
            ModelsPool_lock.lock();
            Pool.emplace(it->second, V);
            ModelsPool_lock.unlock();
        }
        else
        {
            // Registry entry not-found - just special type of visual / particles / etc.
            xr_delete(V);
        }
    }
    V = nullptr;
}

void CModelPool::Delete(dxRender_Visual*& V, BOOL bDiscard)
{
    if (nullptr == V)
        return;

    if (g_bRendering)
    {
        VERIFY(!bDiscard);

        ModelsToDelete_lock.lock();
        ModelsToDelete.push_back(V);
        ModelsToDelete_lock.unlock();
    }
    else
    {
        DeleteInternal(V, bDiscard);
    }

    V = nullptr;
}

void CModelPool::DeleteQueue()
{
    for (auto& it : ModelsToDelete)
        DeleteInternal(it);

    ModelsToDelete.clear();
}

void CModelPool::Discard(dxRender_Visual*& V, BOOL b_complete)
{
    const REGISTRY_IT it = Registry.find(V);
    if (it != Registry.end())
    {
        // Pool - OK

        // Base
        const shared_str& name = it->second;

        xr_vector<ModelDef>::iterator I = Models.begin();
        const xr_vector<ModelDef>::iterator I_e = Models.end();

        for (; I != I_e; ++I)
        {
            if (I->name == name)
            {
                if (b_complete || strchr(*name, '#'))
                {
                    VERIFY(I->refs > 0);
                    I->refs--;
                    if (0 == I->refs)
                    {
                        const bool prefetch = Device.dwPrecacheFrame > 0;

                        if (!prefetch)
                            MsgDbg("~ %s name=%s", __FUNCTION__, name.c_str());

                        bForceDiscard = TRUE;
                        I->model->Release();
                        xr_delete(I->model);
                        Models.erase(I);
                        bForceDiscard = FALSE;
                    }
                    break;
                }

                if (I->refs > 0)
                    I->refs--;

                break;
            }
        }
        // Registry
        Registry.erase(it);
    }

    xr_delete(V);
    V = nullptr;
}

void CModelPool::Prefetch()
{
    Logging(FALSE);
    begin_prefetch1(true);
    // prefetch visuals
    CTimer timer;
    timer.Start();
    u32 cnt = 0;

    string256 section;
    strconcat(sizeof(section), section, "prefetch_visuals_", g_pGamePersistent->m_game_params.m_game_type);
    if (pSettings->section_exist(section))
    {
        const auto& sect = pSettings->r_section(section);
        for (const auto& pair: sect.Data)
        {
            const auto& low_name = pair.first;
            if (!Instance_Find(low_name.c_str()))
            {
                shared_str fname;
                fname.sprintf("%s.ogf", low_name.c_str());
                if (FS.exist(fsgame::game_meshes, fname.c_str()))
                {
                    dxRender_Visual* V = Create(low_name.c_str());
                    Delete(V, FALSE);
                    cnt++;
                }
                else
                    Msg("! [%s]: %s not found in $game_meshes$", __FUNCTION__, fname.c_str());
            }
        }
    }
    begin_prefetch1(false);

    if (!vis_prefetch_ini || !vis_prefetch_ini->section_exist("prefetch"))
    {
        Msg("[%s] models prefetching time (%zi): [%.2f s.]", __FUNCTION__, cnt, timer.GetElapsed_sec());
        return;
    }

    now_prefetch2 = true;
    const auto& sect = vis_prefetch_ini->r_section("prefetch");
    for (const auto& [low_name, val] : sect.Data)
    {
        float val1{}, val2{};
        sscanf(val.c_str(), "%f,%f", &val1, &val2);

        if (!Instance_Find(low_name.c_str()))
        {
            shared_str fname;
            fname.sprintf("%s.ogf", low_name.c_str());
            if (FS.exist(fsgame::game_meshes, fname.c_str()))
            {
                ::Render->hud_loading = val2 == 2.f;
                //if (::Render->hud_loading)
                //    Msg("--[%s] loading hud model [%s]", __FUNCTION__, fname.c_str());
                dxRender_Visual* V = Create(low_name.c_str());
                ::Render->hud_loading = false;
                Delete(V, FALSE);
                cnt++;
            }
            else
                Msg("! [%s]: %s not found in $game_meshes$", __FUNCTION__, fname.c_str());
        }
    }

    now_prefetch2 = false;
    Logging(TRUE);
    Msg("[%s] models prefetching time (%zi): [%.2f s.]", __FUNCTION__, cnt, timer.GetElapsed_sec());
}

void CModelPool::ClearPool(BOOL b_complete)
{
    for (auto& I : Pool)
    {
        if (!b_complete && vis_prefetch_ini)
        {
            std::string s(I.first.c_str());
            if (m_prefetched.find(s) == m_prefetched.end() && !vis_prefetch_ini->line_exist("prefetch", I.first.c_str()))
                b_complete = TRUE;
        }

        Discard(I.second, b_complete);
    }
    Pool.clear();
}

dxRender_Visual* CModelPool::CreatePE(PS::CPEDef* source)
{
    PS::CParticleEffect* V = (PS::CParticleEffect*)Instance_Create(MT_PARTICLE_EFFECT);
    V->Compile(source);
    return V;
}

dxRender_Visual* CModelPool::CreatePG(PS::CPGDef* source)
{
    PS::CParticleGroup* V = (PS::CParticleGroup*)Instance_Create(MT_PARTICLE_GROUP);
    V->Compile(source);
    return V;
}

dxRender_Visual* CModelPool::CreateParticles(LPCSTR name, BOOL bNoPool)
{
    ZoneScoped;

    string_path low_name;
    fix_name(name, low_name);

    dxRender_Visual* Model{};

    if (!bNoPool)
    {
        ModelsPool_lock.lock();

        // 0. Search POOL
        const POOL_IT it = Pool.find(low_name);
        if (it != Pool.end())
        {
            // 1. Instance found
            Model = it->second;
            Model->Spawn();
            Pool.erase(it);

            //Msg("used pool model [%s]", name);
        }

        ModelsPool_lock.unlock();
    }

    if (!Model)
    {
        const bool prefetch = Device.dwPrecacheFrame > 0;

        if (PS::CPEDef* effect = RImplementation.PSLibrary.FindPED(name))
        {
            Model = CreatePE(effect);
        }
        else
        {
            PS::CPGDef* group = RImplementation.PSLibrary.FindPGD(name);
            R_ASSERT(group, "Particle effect or group doesn't exist", name);
            Model = CreatePG(group);
        }

        if (!prefetch)
            MsgDbg("~ %s name=%s", __FUNCTION__, low_name);

        Registry_lock.lock();
        Registry.emplace(Model, low_name);
        Registry_lock.unlock();
    }

    return Model;
}

dxRender_Visual* CModelPool::CreateParticleEffect(LPCSTR name)
{
    // disabled polling for direct effects

    ZoneScoped;

    string_path low_name;
    fix_name(name, low_name);

    dxRender_Visual* Model{};

    //{
    //    ModelsPool_lock.lock();

    //    // 0. Search POOL
    //    const POOL_IT it = Pool.find(low_name);
    //    if (it != Pool.end())
    //    {
    //        // 1. Instance found
    //        Model = it->second;
    //        Model->Spawn();
    //        Pool.erase(it);
    //    }

    //    ModelsPool_lock.unlock();
    //}

    if (!Model)
    {
        PS::CPEDef* effect = RImplementation.PSLibrary.FindPED(name);
        R_ASSERT(effect, "Particle effect doesn't exist", name);
        Model = CreatePE(effect);

        //Registry_lock.lock();
        //Registry.emplace(Model, low_name);
        //Registry_lock.unlock();
    }

    return Model;
}

void CModelPool::dump()
{
    Log("--- model pool --- begin:");
    u32 sz = 0;
    u32 k = 0;
    for (auto& Model : Models)
    {
        CKinematics* K = PCKinematics(Model.model);
        if (K)
        {
            const u32 cur = K->mem_usage(false);
            sz += cur;
            Msg("#%3d: [%3d/%5d Kb] - %s", k++, Model.refs, cur / 1024, Model.name.c_str());
        }
    }
    Msg("--- models: %d, mem usage: %d Kb ", k, sz / 1024);
    sz = 0;
    k = 0;
    int free_cnt = 0;
    for (auto& it : Registry)
    {
        CKinematics* K = PCKinematics((dxRender_Visual*)it.first);
        VERIFY(K);
        if (K)
        {
            const u32 cur = K->mem_usage(true);
            sz += cur;
            const bool b_free = (Pool.contains(it.second));
            if (b_free)
                ++free_cnt;
            Msg("#%3d: [%s] [%5d Kb] - %s", k++, (b_free) ? "free" : "used", cur / 1024, it.second.c_str());
        }
    }
    Msg("--- instances: %d, free %d, mem usage: %d Kb ", k, free_cnt, sz / 1024);
    Log("--- model pool --- end.");
}

void CModelPool::memory_stats(u32& vb_mem_video, u32& vb_mem_system, u32& ib_mem_video, u32& ib_mem_system)
{
    vb_mem_video = 0;
    vb_mem_system = 0;
    ib_mem_video = 0;
    ib_mem_system = 0;

    xr_vector<ModelDef>::iterator it = Models.begin();
    const xr_vector<ModelDef>::const_iterator en = Models.end();

    for (; it != en; ++it)
    {
        dxRender_Visual* ptr = it->model;
        const Fvisual* vis_ptr = dynamic_cast<Fvisual*>(ptr);

        if (vis_ptr == nullptr)
            continue;

        D3D_BUFFER_DESC IB_desc;
        D3D_BUFFER_DESC VB_desc;

        vis_ptr->m_fast->p_rm_Indices->GetDesc(&IB_desc);

        ib_mem_video += IB_desc.ByteWidth;
        ib_mem_system += IB_desc.ByteWidth;

        vis_ptr->m_fast->p_rm_Vertices->GetDesc(&VB_desc);

        vb_mem_video += IB_desc.ByteWidth;
        vb_mem_system += IB_desc.ByteWidth;
    }
}


void CModelPool::save_vis_prefetch() const
{
    if (vis_prefetch_ini)
    {
        process_vis_prefetch();
        vis_prefetch_ini->save_as();
    }
}

void CModelPool::process_vis_prefetch() const
{
    if (!vis_prefetch_ini->section_exist("prefetch"))
        return;
    xr_vector<const char*> expired;
    const auto& sect = vis_prefetch_ini->r_section("prefetch");
    for (const auto& [key, val] : sect.Data)
    {
        float val1{}, val2{};
        sscanf(val.c_str(), "%f,%f", &val1, &val2);
        // Msg("--[%s] sscanf returns: [%f,%f]", __FUNCTION__, val1, val2);
        const float need = val1 * 0.8f; // скорость уменьшение популярности визуала
        // -0.5..+0.5 - добавить случайность, чтобы не было общего выключения
        const float rnd = Random.randF() - 0.5f;
        val1 = need + rnd * 0.1f;
        if (val1 > 0.1f && val2 > 0.f)
            vis_prefetch_ini->w_fvector2("prefetch", key.c_str(), Fvector2{val1, val2});
        else
            expired.emplace_back(key.c_str());
    }
    for (const char* s : expired)
        vis_prefetch_ini->remove_line("prefetch", s);
}

void CModelPool::begin_prefetch1(bool val) { now_prefetch1 = val; }
