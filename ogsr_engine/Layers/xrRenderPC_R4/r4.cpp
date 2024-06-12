#include "stdafx.h"
#include "r4.h"
#include "../xrRender/fbasicvisual.h"
#include "../../xr_3da/xr_object.h"
#include "../../xr_3da/CustomHUD.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../xrRender/SkeletonCustom.h"
#include "../xrRender/LightTrack.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include "../xrRender/dxWallMarkArray.h"
#include "../xrRender/dxUIShader.h"

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"
#include "../xrRender/ShaderResourceTraits.h"

CRender RImplementation;

//////////////////////////////////////////////////////////////////////////
class CGlow : public IRender_Glow
{
public:
    bool bActive;

public:
    CGlow() : bActive(false) {}
    virtual void set_active(bool b) { bActive = b; }
    virtual bool get_active() { return bActive; }
    virtual void set_position(const Fvector& P) {}
    virtual void set_direction(const Fvector& D) {}
    virtual void set_radius(float R) {}
    virtual void set_texture(LPCSTR name) {}
    virtual void set_color(const Fcolor& C) {}
    virtual void set_color(float r, float g, float b) {}
};

float r_dtex_range = 50.f;
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_dynamic(dxRender_Visual* pVisual, float cdist_sq)
{
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == RImplementation.phase)
    {
        //if (RImplementation.val_bHUD)
        //    Msg("--[%s] Detected hud model: [%s]", __FUNCTION__, pVisual->dbg_name.c_str());
        id = (RImplementation.val_bHUD || ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range)) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
    }
    return pVisual->shader->E[id]._get();
}

//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_static(dxRender_Visual* pVisual, float cdist_sq)
{
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == RImplementation.phase)
    {
        id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
    }
    return pVisual->shader->E[id]._get();
}
static class cl_parallax : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        float h = ps_r2_df_parallax_h;
        RCache.set_c(C, h, -h / 2.f, 1.f / r_dtex_range, 1.f / r_dtex_range);
    }
} binder_parallax;

static class cl_LOD : public R_constant_setup
{
    virtual void setup(R_constant* C) { RCache.LOD.set_LOD(C); }
} binder_LOD;

static class cl_pos_decompress_params : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        float VertTan = -1.0f * tanf(deg2rad(Device.fFOV / 2.0f));
        float HorzTan = -VertTan / Device.fASPECT;

        RCache.set_c(C, HorzTan, VertTan, (2.0f * HorzTan) / (float)Device.dwWidth, (2.0f * VertTan) / (float)Device.dwHeight);
    }
} binder_pos_decompress_params;

static class cl_pos_decompress_params2 : public R_constant_setup
{
    virtual void setup(R_constant* C) { RCache.set_c(C, (float)Device.dwWidth, (float)Device.dwHeight, 1.0f / (float)Device.dwWidth, 1.0f / (float)Device.dwHeight); }
} binder_pos_decompress_params2;

static class cl_water_intensity : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        float fValue = E.m_fWaterIntensity;
        RCache.set_c(C, fValue, fValue, fValue, 0);
    }
} binder_water_intensity;

static class cl_sun_shafts_intensity : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        float fValue = E.m_fSunShaftsIntensity;
        RCache.set_c(C, fValue, fValue, fValue, 0);
    }
} binder_sun_shafts_intensity;

static class cl_alpha_ref : public R_constant_setup
{
    virtual void setup(R_constant* C) { StateManager.BindAlphaRef(C); }
} binder_alpha_ref;

extern ENGINE_API BOOL r2_sun_static;
extern ENGINE_API BOOL r2_advanced_pp; //	advanced post process and effects
//////////////////////////////////////////////////////////////////////////
// Just two static storage
void CRender::create()
{
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 0x12345678);

    m_skinning = -1;
    m_MSAASample = -1;

    // hardware
    o.smapsize = 2048;
    o.mrt = (HW.Caps.raster.dwMRT_count >= 3);
    o.mrtmixdepth = (HW.Caps.raster.b_MRT_mixdepth);

    // SMAP / DST
    o.HW_smap_FETCH4 = FALSE;

    //	DX10 disabled
    o.HW_smap = true;
    o.HW_smap_PCF = o.HW_smap;
    if (o.HW_smap)
    {
        //	For ATI it's much faster on DX10 to use D32F format
        if (HW.Caps.id_vendor == 0x1002)
            o.HW_smap_FORMAT = D3DFMT_D32F_LOCKABLE;
        else
            o.HW_smap_FORMAT = D3DFMT_D24X8;
        Msg("* HWDST/PCF supported and used");
    }

    //	DX10 disabled
    // o.fp16_filter		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_FILTER);
    // o.fp16_blend		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);
    o.fp16_filter = true;
    o.fp16_blend = true;

    // search for ATI formats
    if (!o.HW_smap && (0 == strstr(Core.Params, "-nodf24")))
    {
        o.HW_smap = HW.support((D3DFORMAT)(MAKEFOURCC('D', 'F', '2', '4')), D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
        if (o.HW_smap)
        {
            o.HW_smap_FORMAT = MAKEFOURCC('D', 'F', '2', '4');
            o.HW_smap_PCF = FALSE;
            o.HW_smap_FETCH4 = TRUE;
        }
        Msg("* DF24/F4 supported and used [%X]", o.HW_smap_FORMAT);
    }

    // emulate ATI-R4xx series
    if (strstr(Core.Params, "-r4xx"))
    {
        o.mrtmixdepth = FALSE;
        o.HW_smap = FALSE;
        o.HW_smap_PCF = FALSE;
        o.fp16_filter = FALSE;
        o.fp16_blend = FALSE;
    }

    VERIFY2(o.mrt && (HW.Caps.raster.dwInstructions >= 256), "Hardware doesn't meet minimum feature-level");
    if (o.mrtmixdepth)
        o.albedo_wo = FALSE;
    else if (o.fp16_blend)
        o.albedo_wo = FALSE;
    else
        o.albedo_wo = TRUE;

    // nvstencil on NV40 and up
    o.nvstencil = FALSE;
    // if ((HW.Caps.id_vendor==0x10DE)&&(HW.Caps.id_device>=0x40))	o.nvstencil = TRUE;
    if (strstr(Core.Params, "-nonvs"))
        o.nvstencil = FALSE;

    // nv-dbt
    //	DX10 disabled
    // o.nvdbt				= HW.support	((D3DFORMAT)MAKEFOURCC('N','V','D','B'), D3DRTYPE_SURFACE, 0);
    o.nvdbt = false;
    if (o.nvdbt)
        Msg("* NV-DBT supported and used");

    // options (smap-pool-size)
    o.smapsize = r2_SmapSize;

    // gloss
    char* g = strstr(Core.Params, "-gloss ");
    o.forcegloss = g ? TRUE : FALSE;
    if (g)
    {
        o.forcegloss_v = float(atoi(g + xr_strlen("-gloss "))) / 255.f;
    }

    // options
    o.bug = (strstr(Core.Params, "-bug")) ? TRUE : FALSE;
    o.sunfilter = (strstr(Core.Params, "-sunfilter")) ? TRUE : FALSE;
    //.	o.sunstatic			= (strstr(Core.Params,"-sunstatic"))?	TRUE	:FALSE	;
    o.sunstatic = r2_sun_static;
    o.advancedpp = r2_advanced_pp;
    o.sjitter = (strstr(Core.Params, "-sjitter")) ? TRUE : FALSE;
    o.depth16 = (strstr(Core.Params, "-depth16")) ? TRUE : FALSE;
    o.noshadows = (strstr(Core.Params, "-noshadows")) ? TRUE : FALSE;
    o.Tshadows = (strstr(Core.Params, "-tsh")) ? TRUE : FALSE;
    o.mblur = (strstr(Core.Params, "-mblur")) ? TRUE : FALSE;
    o.distortion_enabled = (strstr(Core.Params, "-nodistort")) ? FALSE : TRUE;
    o.distortion = o.distortion_enabled;
    o.disasm = (strstr(Core.Params, "-disasm")) ? TRUE : FALSE;
    o.forceskinw = (strstr(Core.Params, "-skinw")) ? TRUE : FALSE;

    o.dx10_sm4_1 = ps_r2_ls_flags.test((u32)R3FLAG_USE_DX10_1);
    o.dx10_sm4_1 = o.dx10_sm4_1 && (HW.FeatureLevel >= D3D_FEATURE_LEVEL_10_1);

    //	MSAA option dependencies

    o.dx10_msaa = !!ps_r3_msaa;
    o.dx10_msaa_samples = (1 << ps_r3_msaa);

    o.dx10_msaa_opt = ps_r2_ls_flags.test(R3FLAG_MSAA_OPT);
    o.dx10_msaa_opt = o.dx10_msaa_opt && o.dx10_msaa && (HW.FeatureLevel >= D3D_FEATURE_LEVEL_10_1) || o.dx10_msaa && (HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0);

    // o.dx10_msaa_hybrid	= ps_r2_ls_flags.test(R3FLAG_MSAA_HYBRID);
    o.dx10_msaa_hybrid = ps_r2_ls_flags.test((u32)R3FLAG_USE_DX10_1);
    o.dx10_msaa_hybrid &= !o.dx10_msaa_opt && o.dx10_msaa && (HW.FeatureLevel >= D3D_FEATURE_LEVEL_10_1);

    //	Allow alpha test MSAA for DX10.0

    // o.dx10_msaa_alphatest= ps_r2_ls_flags.test((u32)R3FLAG_MSAA_ALPHATEST);
    // o.dx10_msaa_alphatest= o.dx10_msaa_alphatest && o.dx10_msaa;

    // o.dx10_msaa_alphatest_atoc= (o.dx10_msaa_alphatest && !o.dx10_msaa_opt && !o.dx10_msaa_hybrid);

    o.dx10_msaa_alphatest = 0;
    if (o.dx10_msaa)
    {
        if (o.dx10_msaa_opt || o.dx10_msaa_hybrid)
        {
            if (ps_r3_msaa_atest == 1)
                o.dx10_msaa_alphatest = MSAA_ATEST_DX10_1_ATOC;
            else if (ps_r3_msaa_atest == 2)
                o.dx10_msaa_alphatest = MSAA_ATEST_DX10_1_NATIVE;
        }
        else
        {
            if (ps_r3_msaa_atest)
                o.dx10_msaa_alphatest = MSAA_ATEST_DX10_0_ATOC;
        }
    }

    o.dx10_gbuffer_opt = ps_r2_ls_flags.test(R3FLAG_GBUFFER_OPT);

    o.dx10_minmax_sm = ps_r3_minmax_sm;
    o.dx10_minmax_sm_screenarea_threshold = 1600 * 1200;

    o.dx11_enable_tessellation = HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 && ps_r2_ls_flags_ext.test(R2FLAGEXT_ENABLE_TESSELLATION);

    if (o.dx10_minmax_sm == MMSM_AUTODETECT)
    {
        o.dx10_minmax_sm = MMSM_OFF;

        //	AMD device
        if (HW.Caps.id_vendor == 0x1002)
        {
            if (ps_r_sun_quality >= 3)
                o.dx10_minmax_sm = MMSM_AUTO;
            else if (ps_r_sun_shafts >= 2)
            {
                o.dx10_minmax_sm = MMSM_AUTODETECT;
                //	Check resolution in runtime in use_minmax_sm_this_frame
                o.dx10_minmax_sm_screenarea_threshold = 1600 * 1200;
            }
        }

        //	NVidia boards
        if (HW.Caps.id_vendor == 0x10DE)
        {
            if ((ps_r_sun_shafts >= 2))
            {
                o.dx10_minmax_sm = MMSM_AUTODETECT;
                //	Check resolution in runtime in use_minmax_sm_this_frame
                o.dx10_minmax_sm_screenarea_threshold = 1280 * 1024;
            }
        }
    }

    // constants
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("parallax", &binder_parallax);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("water_intensity", &binder_water_intensity);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("sun_shafts_intensity", &binder_sun_shafts_intensity);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("m_AlphaRef", &binder_alpha_ref);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("pos_decompression_params", &binder_pos_decompress_params);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("pos_decompression_params2", &binder_pos_decompress_params2);
    dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup("triLOD", &binder_LOD);

    Target = xr_new<CRenderTarget>(); // Main target

    Models = xr_new<CModelPool>();
    PSLibrary.OnCreate();

    rmNormal();
    marker = 0;

    ::PortalTraverser.initialize();

    FluidManager.Initialize(70, 70, 70);
    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);
}

void CRender::destroy()
{
    FluidManager.Destroy();

    ::PortalTraverser.destroy();
    /*
    for (u32 i=0; i<HW.Caps.iGPUNum; ++i)
        _RELEASE				(q_sync_point[i]);
    */

    HWOCC.occq_destroy();
    xr_delete(Models);
    xr_delete(Target);
    PSLibrary.OnDestroy();
    Device.seqFrame.Remove(this);
    r_dsgraph_destroy();
}

extern u32 reset_frame;
void CRender::reset_begin()
{
    // Update incremental shadowmap-visibility solver
    // BUG-ID: 10646
    {
        u32 it = 0;
        for (it = 0; it < Lights_LastFrame.size(); it++)
        {
            if (0 == Lights_LastFrame[it])
                continue;
            //try
            //{
                Lights_LastFrame[it]->svis.resetoccq();
            /*}
            catch (...)
            {
                Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
            }*/
        }
        Lights_LastFrame.clear();
    }

    reset_frame = Device.dwFrame;

    if (b_loaded /*&& ((dm_current_size != dm_size) || (ps_r__Detail_density != ps_current_detail_density))*/)
    {
        Details->Unload();
        xr_delete(Details);
    }
    //-AVO

    xr_delete(Target);
    HWOCC.occq_destroy();
    /*
    for (u32 i=0; i<HW.Caps.iGPUNum; ++i)
        _RELEASE				(q_sync_point[i]);
    */
}

void CRender::reset_end()
{
    Target = xr_new<CRenderTarget>();

    if (b_loaded /*&& ((dm_current_size != dm_size) || (ps_r__Detail_density != ps_current_detail_density))*/)
    {
        Details = xr_new<CDetailManager>();
        Details->Load();
    }
    //-AVO

    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);

    // Set this flag true to skip the first render frame,
    // that some data is not ready in the first frame (for example device camera position)
    m_bFirstFrameAfterReset = true;
}

void CRender::OnFrame()
{
    Models->DeleteQueue();

    bool b_main_menu_is_active = (g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive());

    if (!b_main_menu_is_active && g_pGameLevel)
    {
        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))
        {
            if (Details)
                Details->StartAsync();

            if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
            {
                // MT-HOM (@front)
                Device.add_to_seq_parallel(fastdelegate::MakeDelegate(&HOM, &CHOM::MT_RENDER));
            }
        }

        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_RAIN))
        {
            g_pGamePersistent->Environment().StartCalculateAsync();
        }

        if (Details)
            g_pGamePersistent->GrassBendersUpdateAnimations();

        calculate_sun_async();
    }
}

// Перед началом рендера мира --#SM+#-- +SecondVP+
void CRender::BeforeWorldRender() {}

// После рендера мира и пост-эффектов --#SM+#-- +SecondVP+
void CRender::AfterWorldRender(const bool save_bb_before_ui)
{
    if (save_bb_before_ui || Device.m_SecondViewport.IsSVPFrame())
    {
        // Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в 3д прицеле либо в рендер-таргет вьюпорта, из которого мы вернем заберем
        // кадр после рендера ui. Именно этот кадр будет позже выведен на экран.)
        ID3DTexture2D* pBuffer{};
        HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
        HW.pContext->CopyResource(save_bb_before_ui ? Target->rt_BeforeUi->pSurface : Target->rt_secondVP->pSurface, pBuffer);
        pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
    }
}

void CRender::AfterUIRender()
{
    // Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в пда)
    ID3DTexture2D* pBuffer{};
    HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
    HW.pContext->CopyResource(Target->rt_secondVP->pSurface, pBuffer);
    pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)

    // Возвращаем на экран кадр, который сохранили до рендера ui для пда
    ID3DTexture2D* pBuffer2{};
    HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer2));
    HW.pContext->CopyResource(pBuffer2, Target->rt_BeforeUi->pSurface);
    pBuffer2->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
}

// Implementation
IRender_ObjectSpecific* CRender::ros_create(IRenderable* parent) { return xr_new<CROS_impl>(); }
void CRender::ros_destroy(IRender_ObjectSpecific*& p) { xr_delete(p); }
IRenderVisual* CRender::model_Create(LPCSTR name, IReader* data) { return Models->Create(name, data); }
IRenderVisual* CRender::model_CreateChild(LPCSTR name, IReader* data) { return Models->CreateChild(name, data); }
IRenderVisual* CRender::model_Duplicate(IRenderVisual* V) { return Models->Instance_Duplicate((dxRender_Visual*)V); }
void CRender::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
    dxRender_Visual* pVisual = (dxRender_Visual*)V;
    Models->Delete(pVisual, bDiscard);
    V = 0;
}
IRender_DetailModel* CRender::model_CreateDM(IReader* F)
{
    CDetail* D = xr_new<CDetail>();
    D->Load(F);
    return D;
}
void CRender::model_Delete(IRender_DetailModel*& F)
{
    if (F)
    {
        CDetail* D = (CDetail*)F;
        D->Unload();
        xr_delete(D);
        F = NULL;
    }
}
IRenderVisual* CRender::model_CreatePE(LPCSTR name)
{
    PS::CPEDef* SE = PSLibrary.FindPED(name);
    R_ASSERT3(SE, "Particle effect doesn't exist", name);
    return Models->CreatePE(SE);
}
IRenderVisual* CRender::model_CreateParticles(LPCSTR name)
{
    PS::CPEDef* SE = PSLibrary.FindPED(name);
    if (SE)
        return Models->CreatePE(SE);
    else
    {
        PS::CPGDef* SG = PSLibrary.FindPGD(name);
        R_ASSERT3(SG, "Particle effect or group doesn't exist", name);
        return Models->CreatePG(SG);
    }
}
void CRender::models_Prefetch() { Models->Prefetch(); }
void CRender::models_Clear(BOOL b_complete) { Models->ClearPool(b_complete); }
void CRender::models_savePrefetch() { Models->save_vis_prefetch(); }
void CRender::models_begin_prefetch1(bool val) { Models->begin_prefetch1(val); }

ref_shader CRender::getShader(int id)
{
    VERIFY(id < int(Shaders.size()));
    return Shaders[id];
}
IRender_Portal* CRender::getPortal(int id)
{
    VERIFY(id < int(Portals.size()));
    return Portals[id];
}
IRender_Sector* CRender::getSector(int id)
{
    VERIFY(id < int(Sectors.size()));
    return Sectors[id];
}
IRender_Sector* CRender::getSectorActive() { return pLastSector; }
IRenderVisual* CRender::getVisual(int id)
{
    VERIFY(id < int(Visuals.size()));
    return Visuals[id];
}
D3DVERTEXELEMENT9* CRender::getVB_Format(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xDC.size()));
        return xDC[id].begin();
    }
    else
    {
        VERIFY(id < int(nDC.size()));
        return nDC[id].begin();
    }
}
ID3DVertexBuffer* CRender::getVB(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xVB.size()));
        return xVB[id];
    }
    else
    {
        VERIFY(id < int(nVB.size()));
        return nVB[id];
    }
}
ID3DIndexBuffer* CRender::getIB(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xIB.size()));
        return xIB[id];
    }
    else
    {
        VERIFY(id < int(nIB.size()));
        return nIB[id];
    }
}
FSlideWindowItem* CRender::getSWI(int id)
{
    VERIFY(id < int(SWIs.size()));
    return &SWIs[id];
}
IRender_Target* CRender::getTarget() { return Target; }

IRender_Light* CRender::light_create() { return Lights.Create(); }
IRender_Glow* CRender::glow_create() { return xr_new<CGlow>(); }

void CRender::flush() { r_dsgraph_render_graph(0); }

BOOL CRender::occ_visible(vis_data& P) { return HOM.visible(P); }
BOOL CRender::occ_visible(sPoly& P) { return HOM.visible(P); }
BOOL CRender::occ_visible(Fbox& P) { return HOM.visible(P); }

void CRender::add_Visual(IRenderVisual* V) { add_leafs_Dynamic((dxRender_Visual*)V); }
void CRender::add_Geometry(IRenderVisual* V) { add_Static((dxRender_Visual*)V, View->getMask()); }
void CRender::add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
    if (T->suppress_wm)
        return;
    VERIFY2(_valid(P) && _valid(s) && T && verts && (s > EPS_L), "Invalid static wallmark params");
    Wallmarks->AddStaticWallmark(T, verts, P, &*S, s);
}

void CRender::add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
    dxWallMarkArray* pWMA = (dxWallMarkArray*)pArray;
    ref_shader* pShader = pWMA->dxGenerateWallmark();
    if (pShader)
        add_StaticWallmark(*pShader, P, s, T, V);
}

void CRender::add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
    dxUIShader* pShader = (dxUIShader*)&*S;
    add_StaticWallmark(pShader->hShader, P, s, T, V);
}

void CRender::clear_static_wallmarks() { Wallmarks->clear(); }

void CRender::add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm) { Wallmarks->AddSkeletonWallmark(wm); }
void CRender::add_SkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
    Wallmarks->AddSkeletonWallmark(xf, obj, sh, start, dir, size);
}
void CRender::add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size)
{
    dxWallMarkArray* pWMA = (dxWallMarkArray*)pArray;
    ref_shader* pShader = pWMA->dxGenerateWallmark();
    if (pShader)
        add_SkeletonWallmark(xf, (CKinematics*)obj, *pShader, start, dir, size);
}

void CRender::add_Occluder(Fbox2& bb_screenspace) { HOM.occlude(bb_screenspace); }
void CRender::set_Object(IRenderable* O) { val_pObject = O; }

void CRender::rmNear()
{
    IRender_Target* T = getTarget();
    D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0, 0.02f};

    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void CRender::rmFar()
{
    IRender_Target* T = getTarget();
    D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0.99999f, 1.f};

    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void CRender::rmNormal()
{
    IRender_Target* T = getTarget();
    D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0, 1.f};

    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender() : m_bFirstFrameAfterReset(false) { init_cacades(); }

CRender::~CRender() {}

#include "../../xr_3da/GameFont.h"
void CRender::Statistics(CGameFont* _F)
{
    CGameFont& F = *_F;
    F.OutNext(" **** LT:%2d,LV:%2d **** ", stats.l_total, stats.l_visible);
    stats.l_visible = 0;
    F.OutNext("    S(%2d)   | (%2d)NS   ", stats.l_shadowed, stats.l_unshadowed);
    F.OutNext("smap use[%2d], merge[%2d], finalclip[%2d]", stats.s_used, stats.s_merged - stats.s_used, stats.s_finalclip);
    stats.s_used = 0;
    stats.s_merged = 0;
    stats.s_finalclip = 0;
    F.OutSkip();
    F.OutNext(" **** Occ-Q(%03.1f) **** ", 100.f * f32(stats.o_culled) / f32(stats.o_queries ? stats.o_queries : 1));
    F.OutNext(" total  : %2d", stats.o_queries);
    stats.o_queries = 0;
    F.OutNext(" culled : %2d", stats.o_culled);
    stats.o_culled = 0;
    F.OutSkip();
    u32 ict = stats.ic_total + stats.ic_culled;
    F.OutNext(" **** iCULL(%03.1f) **** ", 100.f * f32(stats.ic_culled) / f32(ict ? ict : 1));
    F.OutNext(" visible: %2d", stats.ic_total);
    stats.ic_total = 0;
    F.OutNext(" culled : %2d", stats.ic_culled);
    stats.ic_culled = 0;
#ifdef DEBUG
    HOM.stats();
#endif
}


void CRender::addShaderOption(const char* name, const char* value)
{
    D3D_SHADER_MACRO macro = {name, value};
    m_ShaderOptions.push_back(macro);
}

template <typename T>
static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, T*& result, bool const disasm, const char* dbg_name)
{
    result->sh = ShaderTypeTraits<T>::CreateHWShader(buffer, buffer_size);

    if (result->sh)
    {
        result->sh->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
    }

    ID3DShaderReflection* pReflection = 0;

    HRESULT const _hr = D3DReflect(buffer, buffer_size, IID_ID3DShaderReflection, (void**)&pReflection);
    if (SUCCEEDED(_hr) && pReflection)
    {
        // Parse constant table data
        result->constants.parse(pReflection, ShaderTypeTraits<T>::GetShaderDest());

        _RELEASE(pReflection);
    }
    else
    {
        Msg("! D3DReflectShader %s hr == 0x%08x", file_name, _hr);
    }

    return _hr;
}

static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, void*& result, bool const disasm)
{
    string128 dbg_name{}, dbg_ext{};
    _splitpath(file_name, nullptr, nullptr, dbg_name, dbg_ext);
    strcat_s(dbg_name, dbg_ext);

    HRESULT _result = E_FAIL;
    if (pTarget[0] == 'p')
    {
        SPS* sps_result = (SPS*)result;
        _result = HW.pDevice->CreatePixelShader(buffer, buffer_size, 0, &sps_result->ps);
        if (!SUCCEEDED(_result))
        {
            Msg("! PS: [%s]", file_name);
            Msg("! CreatePixelShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (sps_result->ps)
        {
            sps_result->ps->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_ID3DShaderReflection, (void**)&pReflection);

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	Let constant table parse it's data
            sps_result->constants.parse(pReflection, RC_dest_pixel);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! PS: [%s]", file_name);
            Msg("! D3DReflectShader hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'v')
    {
        SVS* svs_result = (SVS*)result;

        _result = HW.pDevice->CreateVertexShader(buffer, buffer_size, 0, &svs_result->vs);

        if (!SUCCEEDED(_result))
        {
            Msg("! VS: [%s]", file_name);
            Msg("! CreatePixelShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (svs_result->vs)
        {
            svs_result->vs->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_ID3DShaderReflection, (void**)&pReflection);

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	TODO: DX10: share the same input signatures

            //	Store input signature (need only for VS)
            // CHK_DX( D3DxxGetInputSignatureBlob(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &_vs->signature) );
            ID3DBlob* pSignatureBlob;
            CHK_DX(D3DGetInputSignatureBlob(buffer, buffer_size, &pSignatureBlob));
            VERIFY(pSignatureBlob);

            svs_result->signature = dxRenderDeviceRender::Instance().Resources->_CreateInputSignature(pSignatureBlob);

            _RELEASE(pSignatureBlob);

            //	Let constant table parse it's data
            svs_result->constants.parse(pReflection, RC_dest_vertex);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! VS: [%s]", file_name);
            Msg("! D3DXFindShaderComment hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'g')
    {
        SGS* sgs_result = (SGS*)result;
        _result = HW.pDevice->CreateGeometryShader(buffer, buffer_size, 0, &sgs_result->gs);
        if (!SUCCEEDED(_result))
        {
            Msg("! GS: [%s]", file_name);
            Msg("! CreateGeometryShaderhr == 0x%08x", _result);
            return E_FAIL;
        }

        if (sgs_result->gs)
        {
            sgs_result->gs->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_ID3DShaderReflection, (void**)&pReflection);

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	Let constant table parse it's data
            sgs_result->constants.parse(pReflection, RC_dest_geometry);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! PS: [%s]", file_name);
            Msg("! D3DReflectShader hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'c')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SCS*&)result, disasm, dbg_name);
    }
    else if (pTarget[0] == 'h')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SHS*&)result, disasm, dbg_name);
    }
    else if (pTarget[0] == 'd')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SDS*&)result, disasm, dbg_name);
    }
    else
    {
        NODEFAULT;
    }

    if (disasm)
    {
        ID3DBlob* disasm = 0;
        D3DDisassemble(buffer, buffer_size, FALSE, 0, &disasm);
        //Пусть дизассемблятся по пути шейдеркэша, всё равно его теперь нету.
        string_path dname;
        xr_strconcat(dname, file_name, ".txt");
        IWriter* W = FS.w_open(dname);
        W->w(disasm->GetBufferPointer(), (u32)disasm->GetBufferSize());
        FS.w_close(W);
        _RELEASE(disasm);
    }

    return _result;
}

//--------------------------------------------------------------------------------------------------------------
class includer final : public ID3DInclude
{
    IReader* R{};

public:
    HRESULT __stdcall Open(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
    {
        string_path pname;
        strconcat(sizeof(pname), pname, ::Render->getShaderPath(), pFileName);
        R = FS.r_open("$game_shaders$", pname);
        if (!R)
        {
            // possibly in shared directory or somewhere else - open directly
            R = FS.r_open("$game_shaders$", pFileName);
            if (!R)
                return E_FAIL;
        }

        R->skip_bom(pFileName);

        *ppData = R->pointer();
        *pBytes = R->elapsed();
        return D3D_OK;
    }

    HRESULT __stdcall Close(LPCVOID) override
    {
        if (R)
            FS.r_close(R);
        return D3D_OK;
    }
};

HRESULT CRender::shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result)
{
    xr_vector<D3D_SHADER_MACRO> defines{m_ShaderOptions};
    defines.reserve(55);

    char c_smapsize[10]{};
    char c_sun_shafts[10]{};
    char c_sun_quality[10]{};
    char c_ssao[10]{};
    char samples[10]{};
    char c_rain_quality[10]{};

    sprintf_s(c_smapsize, "%d", o.smapsize);
    defines.emplace_back("SMAP_size", c_smapsize);

    if (o.fp16_filter)
        defines.emplace_back("FP16_FILTER", "1");

    if (o.fp16_blend)
        defines.emplace_back("FP16_BLEND", "1");

    if (o.HW_smap)
        defines.emplace_back("USE_HWSMAP", "1");

    if (o.HW_smap_PCF)
        defines.emplace_back("USE_HWSMAP_PCF", "1");

    if (o.HW_smap_FETCH4)
        defines.emplace_back("USE_FETCH4", "1");

    if (o.sjitter)
        defines.emplace_back("USE_SJITTER", "1");

    if (HW.Caps.raster_major >= 3)
        defines.emplace_back("USE_BRANCHING", "1");

    if (HW.Caps.geometry.bVTF)
        defines.emplace_back("USE_VTF", "1");

    if (o.Tshadows)
        defines.emplace_back("USE_TSHADOWS", "1");

    if (o.mblur)
        defines.emplace_back("USE_MBLUR", "1");

    if (o.sunfilter)
        defines.emplace_back("USE_SUNFILTER", "1");

    if (o.sunstatic)
        defines.emplace_back("USE_R2_STATIC_SUN", "1");

    if (o.forceskinw)
        defines.emplace_back("SKIN_COLOR", "1");

    if (o.dx10_msaa)
        defines.emplace_back("ISAMPLE", "0");

    // skinning
    if (m_skinning < 0)
        defines.emplace_back("SKIN_NONE", "1");
    else if (0 == m_skinning)
        defines.emplace_back("SKIN_0", "1");
    else if (1 == m_skinning)
        defines.emplace_back("SKIN_1", "1");
    else if (2 == m_skinning)
        defines.emplace_back("SKIN_2", "1");
    else if (3 == m_skinning)
        defines.emplace_back("SKIN_3", "1");
    else if (4 == m_skinning)
        defines.emplace_back("SKIN_4", "1");

    defines.emplace_back("USE_SOFT_WATER", "1");

    defines.emplace_back("USE_SOFT_PARTICLES", "1");

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_DOF))
        defines.emplace_back("USE_DOF", "1");

    if (RImplementation.o.advancedpp && ps_r_sun_shafts)
    {
        sprintf_s(c_sun_shafts, "%d", ps_r_sun_shafts);
        defines.emplace_back("SUN_SHAFTS_QUALITY", c_sun_shafts);
    }

    if (RImplementation.o.advancedpp && ps_r_ao_mode == AO_MODE_GTAO)
        defines.emplace_back("USE_GTAO", "1");

    if (RImplementation.o.advancedpp && ps_r_ssao)
    {
        sprintf_s(c_ssao, "%d", ps_r_ssao);
        defines.emplace_back("SSAO_QUALITY", c_ssao);
    }

    if (RImplementation.o.advancedpp && ps_r_sun_quality)
    {
        sprintf_s(c_sun_quality, "%d", ps_r_sun_quality);
        defines.emplace_back("SUN_QUALITY", c_sun_quality);
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_STEEP_PARALLAX))
        defines.emplace_back("ALLOW_STEEPPARALLAX", "1");

    if (o.dx10_gbuffer_opt)
        defines.emplace_back("GBUFFER_OPTIMIZATION", "1");

    if (o.dx10_sm4_1)
        defines.emplace_back("SM_4_1", "1");

    if (HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        defines.emplace_back("SM_5", "1");

    if (o.dx10_minmax_sm)
        defines.emplace_back("USE_MINMAX_SM", "1");

    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_SSLR))
        defines.emplace_back("SSLR_ENABLED", "1");

    if (ps_r2_ls_flags_ext.test(SSFX_HEIGHT_FOG))
        defines.emplace_back("SSFX_FOG", "1");

    if (ps_r2_ls_flags_ext.test(SSFX_SKY_DEBANDING))
        defines.emplace_back("SSFX_DEBAND", "1");

    if (ps_r2_ls_flags_ext.test(SSFX_INDIRECT_LIGHT))
        defines.emplace_back("SSFX_INDIRECT_LIGHT", "1");

    if (ps_r2_ls_flags_ext.test(REFLECTIONS_ONLY_ON_TERRAIN))
        defines.emplace_back("REFLECTIONS_ONLY_ON_TERRAIN", "1");

    if (ps_r2_ls_flags_ext.test(REFLECTIONS_ONLY_ON_PUDDLES))
        defines.emplace_back("REFLECTIONS_ONLY_ON_PUDDLES", "1");

    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_TERRAIN_PARALLAX))
        defines.emplace_back("TERRAIN_PARALLAX_ENABNLED", "1");

    if (ps_ssfx_rain_1.w > 0.f)
    {
        sprintf_s(c_rain_quality, "%.0f", ps_ssfx_rain_1.w);
        defines.emplace_back("SSFX_RAIN_QUALITY", c_rain_quality);
    }

    if (o.dx10_msaa)
    {
        defines.emplace_back("USE_MSAA", "1");

        sprintf_s(samples, "%d", o.dx10_msaa_samples);
        defines.emplace_back("MSAA_SAMPLES", samples);

        if (o.dx10_msaa_opt)
            defines.emplace_back("MSAA_OPTIMIZATION", "1");

        switch (o.dx10_msaa_alphatest)
        {
        case MSAA_ATEST_DX10_0_ATOC: defines.emplace_back("MSAA_ALPHATEST_DX10_0_ATOC", "1"); break;
        case MSAA_ATEST_DX10_1_ATOC: defines.emplace_back("MSAA_ALPHATEST_DX10_1_ATOC", "1"); break;
        case MSAA_ATEST_DX10_1_NATIVE: defines.emplace_back("MSAA_ALPHATEST_DX10_1", "1"); break;
        }
    }

    // finish
    defines.emplace_back(nullptr, nullptr);

    char extension[3]{};
    strncpy_s(extension, pTarget, 2);

    string_path file_name{};

    string_path file{};
    xr_strcpy(file, "shaders_cache\\r4\\");
    xr_strcat(file, name);
    xr_strcat(file, ".");
    xr_strcat(file, extension);
    FS.update_path(file_name, "$app_data_root$", file);

    includer Includer;
    LPD3DBLOB pShaderBuf{};
    LPD3DBLOB pErrorBuf{};

    // Msg("--Compiling shader [%s] %s] [%s]", name, pTarget, pFunctionName);

    auto _result = D3DCompile(pSrcData, SrcDataLen, "", defines.data(), &Includer, pFunctionName, pTarget, Flags, 0, &pShaderBuf, &pErrorBuf);

    if (SUCCEEDED(_result))
    {
        _result = create_shader(pTarget, (DWORD*)pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize(), file_name, result, o.disasm);
    }
    else
    {
        Msg("! %s", file_name);
        if (pErrorBuf)
            Log("! error: " + std::string{reinterpret_cast<const char*>(pErrorBuf->GetBufferPointer())});
        else
            Msg("Can't compile shader hr=0x%08x", _result);
    }

    return _result;
}
