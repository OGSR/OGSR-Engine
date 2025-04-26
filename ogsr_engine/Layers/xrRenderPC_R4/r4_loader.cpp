#include "stdafx.h"
#include "r4.h"

#include "../xrRender/ResourceManager.h"
#include "../xrRender/fbasicvisual.h"

#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/xrLevel.h"
#include "../../xr_3da/x_ray.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "../../xrCore/stream_reader.h"

#include "../xrRender/dxRenderDeviceRender.h"
#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../xrRender/FHierrarhyVisual.h"

#include <Utilities/FlexibleVertexFormat.h>

#include "../xrRenderDX10/3DFluid/dx103DFluidVolume.h"

void CRender::level_Load(IReader* fs)
{
    R_ASSERT(0 != g_pGameLevel);
    R_ASSERT(!b_loaded);

    u32 m_base, c_base, m_lmaps, c_lmaps;
    Device.m_pRender->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources load...");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    // Begin
    pApp->LoadBegin();
    Device.m_pRender->DeferredLoad(TRUE);

    IReader* chunk;

    // Shaders
    g_pGamePersistent->LoadTitle("st_loading_shaders");
    {
        chunk = fs->open_chunk(fsL_SHADERS);
        R_ASSERT(chunk, "Level doesn't builded correctly.");
        const u32 count = chunk->r_u32();
        Shaders.resize(count);
        for (u32 i = 0; i < count; i++) // skip first shader as "reserved" one
        {
            string512 n_sh, n_tlist;
            LPCSTR n = LPCSTR(chunk->pointer());
            chunk->skip_stringZ();
            if (0 == n[0])
                continue;
            xr_strcpy(n_sh, n);
            const LPSTR delim = strchr(n_sh, '/');
            *delim = 0;
            xr_strcpy(n_tlist, delim + 1);
            Shaders[i] = dxRenderDeviceRender::Instance().Resources->Create(n_sh, n_tlist);
        }
        chunk->close();
    }

    // Components
    Wallmarks = xr_new<CWallmarksEngine>();
    Details = xr_new<CDetailManager>();

    {
        // VB,IB,SWI
        g_pGamePersistent->LoadTitle("st_loading_geometry");
        {
            CStreamReader* geom = FS.rs_open(fsgame::level, fsgame::level_files::level_geom);
            R_ASSERT(geom, "level.geom");
            LoadBuffers(geom, FALSE);
            LoadSWIs(geom);
            FS.r_close(geom);
        }

        //...and alternate/fast geometry
        {
            CStreamReader* geom = FS.rs_open(fsgame::level, fsgame::level_files::level_geomx);
            R_ASSERT(geom, "level.geomX");
            LoadBuffers(geom, TRUE);
            FS.r_close(geom);
        }

        // Visuals
        g_pGamePersistent->LoadTitle("st_loading_spatial_db");
        chunk = fs->open_chunk(fsL_VISUALS);
        LoadVisuals(chunk);
        chunk->close();

        // Details
        g_pGamePersistent->LoadTitle("st_loading_details");
        Details->Load();
    }

    // Sectors
    g_pGamePersistent->LoadTitle("st_loading_sectors_portals");
    LoadSectors(fs);

    // 3D Fluid
    Load3DFluid();

    // HOM
    HOM.Load();

    // Lights
    // pApp->LoadTitle			("Loading lights...");
    LoadLights(fs);

    current_level_puddles.clear();
    string_path puddles_file{};
    if (FS.exist(puddles_file, "$level$", "level.puddles"))
    {
        CInifile ini(puddles_file);

        current_level_puddles.reserve(ini.sections().size());

        for (const auto& pair : ini.sections())
        {
            const auto& sect = pair.second->Name;

            auto& puddle = current_level_puddles.emplace_back();

            const Fvector position = ini.r_fvector3(sect, "center");
            const float max_height = ini.r_float(sect, "max_depth");

            Fvector2 size_xz = ini.r_fvector2(sect, "radius");
            if (fis_zero(size_xz.y))
                size_xz.y = size_xz.x;

            if (pair.second->line_exist("rotation"))
                puddle.xform.rotateY(ini.r_float(sect, "rotation"));

            puddle.xform.mulB_43(Fmatrix{}.scale(size_xz.x, 1.0f, size_xz.y));

            puddle.xform.translate_over(position);

            puddle.height = max_height;
            puddle.radius = size_xz.magnitude();

            // Msg("~~Loaded puddle with center: [%f,%f,%f], size: [%f,%f]", position.x, position.y, position.z, size_xz.x, size_xz.y);
        }
    }

    // End
    pApp->LoadEnd();

    //u32 m_base, c_base, m_lmaps, c_lmaps;
    Device.m_pRender->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources load completed!");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    // signal loaded
    b_loaded = TRUE;
}

void CRender::level_Unload()
{
    if (nullptr == g_pGameLevel)
        return;

    if (!b_loaded)
        return;
    
    u32 m_base, c_base, m_lmaps, c_lmaps;
    Device.m_pRender->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources unload...");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    u32 I;

    // HOM
    HOM.Unload();

    //*** Details
    Details->Unload();

    //*** Sectors
    // 1.
    xr_delete(rmPortals);
    last_sector_id = IRender_Sector::INVALID_SECTOR_ID;
    Device.vCameraPositionSaved.set(0, 0, 0);

    cleanup_contexts();

    for (auto& id : contexts_pool)
    {
        id.unload();
    }

    //*** Lights
    // Glows.Unload			();
    Lights.Unload();

    //*** Visuals
    for (I = 0; I < Visuals.size(); I++)
    {
        Visuals[I]->Release();
        xr_delete(Visuals[I]);
    }
    Visuals.clear();

    //*** SWI
    for (I = 0; I < SWIs.size(); I++)
        xr_free(SWIs[I].sw);
    SWIs.clear();

    //*** VB/IB
    for (I = 0; I < nVB.size(); I++)
        _RELEASE(nVB[I]);
    for (I = 0; I < xVB.size(); I++)
        _RELEASE(xVB[I]);
    nVB.clear();
    xVB.clear();
    for (I = 0; I < nIB.size(); I++)
        _RELEASE(nIB[I]);
    for (I = 0; I < xIB.size(); I++)
        _RELEASE(xIB[I]);
    nIB.clear();
    xIB.clear();
    nDC.clear();
    xDC.clear();

    //*** Components
    xr_delete(Details);
    xr_delete(Wallmarks);

    //*** Shaders
    Shaders.clear();

    // u32 m_base, c_base, m_lmaps, c_lmaps;
    Device.m_pRender->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

    Msg("~ LevelResources unload completed!");
    Msg("~ LevelResources - base: %d, %d K", c_base, m_base / 1024);
    Msg("~ LevelResources - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);

    b_loaded = FALSE;
}

void CRender::LoadBuffers(CStreamReader* base_fs, BOOL _alternative)
{
    R_ASSERT(base_fs, "Could not load geometry. File not found.");

    xr_vector<VertexDeclarator>& _DC = _alternative ? xDC : nDC;
    xr_vector<ID3DVertexBuffer*>& _VB = _alternative ? xVB : nVB;
    xr_vector<ID3DIndexBuffer*>& _IB = _alternative ? xIB : nIB;

    // Vertex buffers
    {
        // Use DX9-style declarators
        CStreamReader* fs = base_fs->open_chunk(fsL_VB);
        R_ASSERT(fs, "Could not load geometry. File 'level.geom?' corrupted.");
        const u32 count = fs->r_u32();
        _DC.resize(count);
        _VB.resize(count);

        // decl
        const u32 buffer_size = (MAXD3DDECLLENGTH + 1) * sizeof(D3DVERTEXELEMENT9);
        auto* dcl = (D3DVERTEXELEMENT9*)_alloca(buffer_size);

        for (u32 i = 0; i < count; i++)
        {
            fs->r(dcl, buffer_size);
            fs->advance(-(int)buffer_size);

            const u32 dcl_len = FVF::GetDeclLength(dcl) + 1;
            _DC[i].resize(dcl_len);
            fs->r(_DC[i].begin(), dcl_len * sizeof(D3DVERTEXELEMENT9));

            // count, size
            const u32 vCount = fs->r_u32();
            const u32 vSize = FVF::ComputeVertexSize(dcl, 0);
            Msg("* [Loading VB] %d verts, %d Kb", vCount, (vCount * vSize) / 1024);

            //	TODO: DX10: Check fragmentation.
            //	Check if buffer is less then 2048 kb
            BYTE* pData = xr_alloc<BYTE>(vCount * vSize);
            fs->r(pData, vCount * vSize);
            dx10BufferUtils::CreateVertexBuffer(&_VB[i], pData, vCount * vSize);
            xr_free(pData);

            //			fs->advance			(vCount*vSize);
        }
        fs->close();
    }

    // Index buffers
    {
        CStreamReader* fs = base_fs->open_chunk(fsL_IB);
        const u32 count = fs->r_u32();
        _IB.resize(count);
        for (u32 i = 0; i < count; i++)
        {
            const u32 iCount = fs->r_u32();
            Msg("* [Loading IB] %d indices, %d Kb", iCount, (iCount * 2) / 1024);

            //	TODO: DX10: Check fragmentation.
            //	Check if buffer is less then 2048 kb
            BYTE* pData = xr_alloc<BYTE>(iCount * 2);
            fs->r(pData, iCount * 2);
            dx10BufferUtils::CreateIndexBuffer(&_IB[i], pData, iCount * 2);
            xr_free(pData);

            //			fs().advance		(iCount*2);
        }
        fs->close();
    }
}

void CRender::LoadVisuals(IReader* fs)
{
    u32 index = 0;
    dxRender_Visual* V = nullptr;
    ogf_header H;

    IReader* chunk;

    while ((chunk = fs->open_chunk(index)) != nullptr)
    {
        chunk->r_chunk_safe(OGF_HEADER, &H, sizeof(H));

        V = Models->Instance_Create(H.type);
        V->Load(nullptr, chunk, 0);
        V->is_level_static = true;

        Visuals.push_back(V);

        chunk->close();

        index++;
    }

    Msg("Loaded [%d] models from level.", Visuals.size());
}

void CRender::LoadLights(IReader* fs)
{
    // lights
    Lights.Load(fs);
    Lights.LoadHemi();
}

void CRender::LoadSectors(IReader* fs)
{
    // allocate memory for portals
    const u32 size = fs->find_chunk(fsL_PORTALS);
    R_ASSERT(0 == size % sizeof(CPortal::level_portal_data_t));

    // load sectors
    xr_vector<CSector::level_sector_data_t> sectors_data;

    float largest_sector_vol = 0.0f;

    // load sectors
    IReader* S = fs->open_chunk(fsL_SECTORS);
    for (u32 i = 0;; i++)
    {
        IReader* P = S->open_chunk(i);
        if (!P)
            break;

        {
            u32 size = P->find_chunk(fsP_Portals);
            R_ASSERT(0 == (size & 1));
            u32 portals_in_sector = size / sizeof(u16);

            auto& sector_data = sectors_data.emplace_back();
            sector_data.portals_id.reserve(portals_in_sector);
            while (portals_in_sector)
            {
                const u16 ID = P->r_u16();
                sector_data.portals_id.emplace_back(ID);
                --portals_in_sector;
            }

            size = P->find_chunk(fsP_Root);
            R_ASSERT(size == 4);
            sector_data.root_id = P->r_u32();

            // Search for default sector - assume "default" or "outdoor" sector is the largest one
            // XXX: hack: need to know real outdoor sector
            auto* V = static_cast<dxRender_Visual*>(RImplementation.getVisual(sector_data.root_id));
            const float vol = V->getVisData().box.getvolume();
            if (vol > largest_sector_vol)
            {
                largest_sector_vol = vol;
                largest_sector_id = /*static_cast<IRender_Sector::sector_id_t>*/(i);
            }
        }

        P->close();
    }
    S->close();

    const u32 portals_count = size / sizeof(CPortal::level_portal_data_t);
    xr_vector<CPortal::level_portal_data_t> portals_data{portals_count};

    // load portals
    if (portals_count)
    {
        CDB::Collector CL;
        fs->find_chunk(fsL_PORTALS);
        for (u32 i = 0; i < portals_count; i++)
        {
            auto& P = portals_data[i];
            fs->r(&P, sizeof(P));

            {
                for (u32 j = 2; j < P.vertices.size(); j++)
                    CL.add_face_packed_D(P.vertices[0], P.vertices[j - 1], P.vertices[j], u32(i));
            }
        }

        if (CL.getTS() < 2)
        {
            constexpr Fvector v1{-20000.f, -20000.f, -20000.f};
            constexpr Fvector v2{-20001.f, -20001.f, -20001.f};
            constexpr Fvector v3{-20002.f, -20002.f, -20002.f};
            CL.add_face_packed_D(v1, v2, v3, 0);
        }

        //if (!SectorsLoadDisabled)
        {
            // build portal model
            rmPortals = xr_new<CDB::MODEL>();
            rmPortals->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()), nullptr, nullptr, false);
        }
    }
    else
    {
        rmPortals = nullptr;
    }

    Msg("Level sector data:");
    for (const auto& sector_data : sectors_data)
    {
        auto* V = static_cast<dxRender_Visual*>(RImplementation.getVisual(sector_data.root_id));
        const float vol = V->getVisData().box.getvolume();

        Msg("root_id=[%d] volume=[%f]", sector_data.root_id, vol);
    }

    // debug
    //	for (int d=0; d<Sectors.size(); d++)
    //		Sectors[d]->DebugDump	();

    for (int id = 0; id < R__NUM_PARALLEL_CONTEXTS; ++id)
    {
        auto& dsgraph = contexts_pool[id];
        dsgraph.reset();
        dsgraph.load(sectors_data, portals_data);
        contexts_used.set(id, false);
    }

    auto& dsgraph = get_imm_context();
    dsgraph.reset();
    dsgraph.load(sectors_data, portals_data);

    last_sector_id = IRender_Sector::INVALID_SECTOR_ID;
}

void CRender::LoadSWIs(CStreamReader* base_fs)
{
    // allocate memory for portals
    if (base_fs->find_chunk(fsL_SWIS))
    {
        CStreamReader* fs = base_fs->open_chunk(fsL_SWIS);
        const u32 item_count = fs->r_u32();

        xr_vector<FSlideWindowItem>::iterator it = SWIs.begin();
        const xr_vector<FSlideWindowItem>::iterator it_e = SWIs.end();

        for (; it != it_e; ++it)
            xr_free((*it).sw);

        SWIs.clear();

        SWIs.resize(item_count);
        for (u32 c = 0; c < item_count; c++)
        {
            FSlideWindowItem& swi = SWIs[c];
            swi.reserved[0] = fs->r_u32();
            swi.reserved[1] = fs->r_u32();
            swi.reserved[2] = fs->r_u32();
            swi.reserved[3] = fs->r_u32();
            swi.count = fs->r_u32();
            VERIFY(NULL == swi.sw);
            swi.sw = xr_alloc<FSlideWindow>(swi.count);
            fs->r(swi.sw, sizeof(FSlideWindow) * swi.count);
        }
        fs->close();
    }
}

void CRender::Load3DFluid()
{
    if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;

#ifdef DX10_FLUID_ENABLE

    string_path fn_game;
    if (FS.exist(fn_game, fsgame::level, fsgame::level_files::level_fog_vol))
    {
        IReader* F = FS.r_open(fn_game);
        const u16 version = F->r_u16();

        if (version == 3)
        {
            const u32 cnt = F->r_u32();
            for (u32 i = 0; i < cnt; ++i)
            {
                dx103DFluidVolume* pVolume = xr_new<dx103DFluidVolume>();
                pVolume->Load("", F, 0);

                const auto& v = pVolume->getVisData().sphere.P;

                Msg("~ Loading fog volume with profile [%s]. Position x=[%f] y=[%f] z=[%f]", pVolume->getProfileName().c_str(), v.x, v.y, v.z);

                 auto& dsgraph = get_imm_context();

                //	Attach to sector's static geometry
                const auto sector_id = dsgraph.detect_sector(pVolume->getVisData().sphere.P);
                auto* pSector = dynamic_cast<CSector*>(dsgraph.get_sector(sector_id));
                if (!pSector)
                {
                    Msg("Cannot find sector for fog volume. Position x=[%f] y=[%f] z=[%f]!", v.x, v.y, v.z);

                    xr_delete(pVolume);

                    continue;
                }

                //	3DFluid volume must be in render sector
                R_ASSERT(pSector);

                dxRender_Visual* pRoot = pSector->root();
                //	Sector must have root
                R_ASSERT(pRoot);
                R_ASSERT(pRoot->getType() == MT_HIERRARHY);

                ((FHierrarhyVisual*)pRoot)->children.push_back(pVolume);
            }
        }

        FS.r_close(F);
    }

#endif
}