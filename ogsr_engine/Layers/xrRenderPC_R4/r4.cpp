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
#include "../xrRender/ParticleGroup.h"
#include "../xrRender/ParticleEffectDef.h"

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"
#include "../xrRender/ShaderResourceTraits.h"

float OLES_SUN_LIMIT_27_01_07 = 100.f;

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

float r_dtex_paralax_range = 50.f;
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_dynamic(dxRender_Visual* pVisual, float cdist_sq, bool hud, u32 phase)
{
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == phase)
    {
        // if (hud)
        //     Msg("--[%s] Detected hud model: [%s]", __FUNCTION__, pVisual->dbg_name.c_str());
        id = (hud || ((_sqrt(cdist_sq) - pVisual->getVisData().sphere.R) < r_dtex_paralax_range)) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
    }
    return pVisual->shader->E[id]._get();
}

//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_static(dxRender_Visual* pVisual, float cdist_sq, u32 phase)
{
    if (!pVisual->shader)
        return nullptr;
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == phase)
    {
        id = ((_sqrt(cdist_sq) - pVisual->getVisData().sphere.R) < r_dtex_paralax_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
    }
    return pVisual->shader->E[id]._get();
}

// custom params export
class cl_custom_export final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, shader_exports.get_custom_params(key));
    }

    shared_str key;

public:
    cl_custom_export(const shared_str& v) : key(v) {}
};


//////////////////////////////////////////////////////////////////////////
// Just two static storage
void CRender::create()
{
    particles_pool.init();
    light_pool.init();
    r_sun.sun_cascade_pool.init();

    Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 10000);

    m_skinning = -1;
    m_SMAPSize = r2_SmapSize;

    // options (smap-pool-size)
    o.smapsize = r2_SmapSize;
    o.rain_smapsize = _min(ps_r3_dyn_wet_surf_sm_res, r2_SmapSize);

    // options
    o.noshadows = (strstr(Core.Params, "-noshadows")) ? TRUE : FALSE;
    o.distortion_enabled = TRUE; // !strstr(Core.Params, "-nodistort");
    o.distortion = o.distortion_enabled;
    o.disasm = (strstr(Core.Params, "-disasm")) ? TRUE : FALSE;

    o.mblur = ps_r2_ls_flags_ext.test(R2FLAGEXT_MOTION_BLUR);

    o.dx11_enable_tessellation = HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 && ps_r2_ls_flags_ext.test(R2FLAGEXT_ENABLE_TESSELLATION);

    // constants
    if (!shader_exports.customExports.empty())
    {
        Msg("Export custom shader params...");

        for (const auto& [key, value] : shader_exports.customExports)
        {
            Msg("Export custom shader param [%s]", key.c_str());

            dxRenderDeviceRender::Instance().Resources->RegisterConstantSetup(key.c_str(), xr_new<cl_custom_export>(key));
        }

        Msg("Export custom shader done!!!");
    }

    Target = xr_new<CRenderTarget>(); // Main target

    Models = xr_new<CModelPool>();
    PSLibrary.OnCreate();

    rmNormal(RCache);

#ifdef DX10_FLUID_ENABLE
    FluidManager.Initialize(70, 70, 70);
    //	FluidManager.Initialize( 100, 100, 100 );
    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);
#endif
}

void CRender::destroy()
{
#ifdef DX10_FLUID_ENABLE
    FluidManager.Destroy();
#endif

    lstRenderables.clear();
    lstParticlesCalculation.clear();
    lstBonesCalculation.clear();

    HWOCC.occq_destroy();
    xr_delete(Models);
    xr_delete(Target);
    PSLibrary.OnDestroy();
    Device.seqFrame.Remove(this);

    get_imm_context().reset();
}

extern u32 reset_frame;

void CRender::reset_begin()
{
    r_main.sync();
    r_sun.sync();
    r_rain.sync();

    light_pool.wait_for_tasks();

    // Update incremental shadowmap-visibility solver
    // BUG-ID: 10646
    {
        for (const auto& it : Lights_LastFrame)
        {
            if (nullptr == it)
                continue;
            //try
            //{
            for (int id = 0; id < R__NUM_CONTEXTS; ++id)
                it->svis[id].resetoccq();
            /*}
            catch (...)
            {
                Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
            }*/
        }
        Lights_LastFrame.clear();
    }

    reset_frame = Device.dwFrame;

    // AVO: let's reload details while changed details options on vid_restart
    if (b_loaded && dm_current_size != dm_size)
    {
        Details->Unload();
        xr_delete(Details);
    }
    //-AVO

    xr_delete(Target);
    HWOCC.occq_destroy();
}

void CRender::reset_end()
{
    Target = xr_new<CRenderTarget>();

    // AVO: let's reload details while changed details options on vid_restart
    if (b_loaded && dm_current_size != dm_size)
    {
        Details = xr_new<CDetailManager>();
        Details->Load();
    }
    //-AVO

#ifdef DX10_FLUID_ENABLE
    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);
#endif

    cleanup_contexts();

    // Set this flag true to skip the first render frame,
    // that some data is not ready in the first frame (for example device camera position)
    m_bFirstFrameAfterReset = true;
}

void CRender::OnFrame()
{
    ZoneScoped;

    if (Details)
    {
        Details->StartCalculationAsync();
    }

    Models->DeleteQueue();

    if (!IsMainMenuActive() && g_pGameLevel)
    {
        if (Details)
        {
            g_pGamePersistent->GrassBendersUpdateAnimations();
        }
    }
}

// После рендера мира и пост-эффектов --#SM+#-- +SecondVP+
void CRender::AfterWorldRender() { Target->u_setrt(get_imm_context().cmd_list, Device.dwWidth, Device.dwHeight, Target->rt_second_vp->pRT, nullptr, nullptr, nullptr); }

void CRender::AfterUIRender() { Target->u_setrt(get_imm_context().cmd_list, Device.dwWidth, Device.dwHeight, Target->get_base_rt(), nullptr, nullptr, nullptr); }

// Implementation
IRender_ObjectSpecific* CRender::ros_create(IRenderable* parent) { return xr_new<CROS_impl>(); }
void CRender::ros_destroy(IRender_ObjectSpecific*& p) { xr_delete(p); }
IRenderVisual* CRender::model_Create(LPCSTR name, IReader* data) { return Models->Create(name, data); }
IRenderVisual* CRender::model_CreateChild(LPCSTR name, IReader* data) { return Models->CreateChild(name, data); }
IRenderVisual* CRender::model_Duplicate(IRenderVisual* V) { return Models->Instance_Duplicate((dxRender_Visual*)V); }
void CRender::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
    if (V)
    {
        dxRender_Visual* pVisual = (dxRender_Visual*)V;
        Models->Delete(pVisual, bDiscard);
        V = nullptr;
    }
}
IRender_DetailModel* CRender::model_CreateDM(IReader* F) // for rain or thunderbolt only
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
        F = nullptr;
    }
}

IRenderVisual* CRender::model_CreateParticles(LPCSTR name, BOOL bNoPool)
{
    return Models->CreateParticles(name, bNoPool);
}
void CRender::models_Prefetch() { Models->Prefetch(); }
void CRender::models_Clear(BOOL b_complete) { Models->ClearPool(b_complete); }
void CRender::models_savePrefetch() { Models->save_vis_prefetch(); }
void CRender::models_begin_prefetch1(bool val) { Models->begin_prefetch1(val); }

ref_shader CRender::getShader(int id)
{
    return Shaders.at(id);
}
IRenderVisual* CRender::getVisual(int id)
{
    return Visuals.at(id);
}
D3DVERTEXELEMENT9* CRender::getVB_Format(int id, BOOL _alt)
{
    if (_alt)
    {
        return xDC.at(id).begin();
    }
    else
    {
        return nDC.at(id).begin();
    }
}
ID3DVertexBuffer* CRender::getVB(int id, BOOL _alt) const
{
    if (_alt)
    {
        return xVB.at(id);
    }
    else
    {
        return nVB.at(id);
    }
}
ID3DIndexBuffer* CRender::getIB(int id, BOOL _alt)
{
    if (_alt)
    {
        return xIB.at(id);
    }
    else
    {
        return nIB.at(id);
    }
}
FSlideWindowItem* CRender::getSWI(int id)
{
    return &SWIs.at(id);
}

IRender_Target* CRender::getTarget() { return Target; }

IRender_Light* CRender::light_create() { return Lights.Create(); }
IRender_Glow* CRender::glow_create() { return xr_new<CGlow>(); }

//BOOL CRender::occ_visible(vis_data& P) { return HOM.visible(P); }
//BOOL CRender::occ_visible(sPoly& P) { return HOM.visible(P); }
//BOOL CRender::occ_visible(Fbox& P) { return HOM.visible(P); }

void CRender::add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m)
{
    // TODO: this whole function should be replaced by a list of renderables+xforms returned from `renderable_Render` call
    auto& dsgraph = get_context(context_id);
    dsgraph.add_leafs_dynamic(root, (dxRender_Visual*)V, m);
}

void CRender::add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
    if (T->suppress_wm)
        return;
    VERIFY(_valid(P) && _valid(s) && T && verts && (s > EPS_L), "Invalid static wallmark params");
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

void CRender::append_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm) { Wallmarks->AppendSkeletonWallmark(wm); }

void CRender::add_SkeletonWallmark(Fmatrix* xf, CKinematics* obj, ref_shader& sh, Fvector& start, Fvector& dir, float size)
{
    Wallmarks->AddSkeletonWallmark(xf, obj, sh, start, dir, size);
}

void CRender::add_SkeletonWallmark(Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, Fvector& start, Fvector& dir, float size)
{
    dxWallMarkArray* pWMA = (dxWallMarkArray*)pArray;
    ref_shader* pShader = pWMA->dxGenerateWallmark();
    if (pShader)
        add_SkeletonWallmark(xf, (CKinematics*)obj, *pShader, start, dir, size);
}

void CRender::clear_static_wallmarks() { Wallmarks->Clear(); }

void CRender::rmNear(CBackend& cmd_list)
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT viewport = {0, 0, T->get_width(cmd_list), T->get_height(cmd_list), 0.f, 0.02f};
    cmd_list.SetViewport(viewport);
}

void CRender::rmFar(CBackend& cmd_list)
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT viewport = {0, 0, T->get_width(cmd_list), T->get_height(cmd_list), 0.99999f, 1.f};
    cmd_list.SetViewport(viewport);
}

void CRender::rmNormal(CBackend& cmd_list)
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT viewport = {0, 0, T->get_width(cmd_list), T->get_height(cmd_list), 0.f, 1.f};
    cmd_list.SetViewport(viewport);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender() : m_bFirstFrameAfterReset(false) { /*init_cascades();*/ }

CRender::~CRender() {}

#include "../../xr_3da/GameFont.h"

void CRender::Statistics(CGameFont* _F)
{
    CGameFont& F = *_F;
    F.OutNext(" **** LT:%2d,LV:%2d **** ", stats.l_total, stats.l_visible);
    stats.l_visible = 0;
    F.OutNext("    S(%2d)   | (%2d)NS   ", stats.l_shadowed, stats.l_unshadowed);
    F.OutNext(" smap use[%2d], merge[%2d], finalclip[%2d]", stats.s_used, stats.s_merged - stats.s_used, stats.s_finalclip);
    stats.s_used = 0;
    stats.s_merged = 0;
    stats.s_finalclip = 0;
    F.OutSkip();
    F.OutNext(" **** Occ-Q(%03.1f) **** ", 100.f * f32(stats.o_culled) / f32(stats.o_queries ? stats.o_queries : 1));
    F.OutNext(" total  : %2d", stats.o_queries);
    F.OutNext(" culled : %2d", stats.o_culled);
    stats.o_queries = 0;
    stats.o_culled = 0;
    F.OutSkip();
    const u32 ict = stats.ic_total + stats.ic_culled;
    F.OutNext(" **** iCULL(%03.1f) **** ", 100.f * f32(stats.ic_culled) / f32(ict ? ict : 1));
    F.OutNext(" visible: %2d", stats.ic_total);
    F.OutNext(" culled : %2d", stats.ic_culled);
    stats.ic_total = 0;
    stats.ic_culled = 0;

    HOM.stats();
}

void CRender::addShaderOption(const char* name, const char* value) { m_ShaderOptions.emplace_back(D3D_SHADER_MACRO{name, value}); }

void CRender::ParticleEffectFillName(xr_vector<shared_str>& s)
{
    for (const auto& pair : PSLibrary.IteratePEDs())
    {
        s.push_back(pair.first);
    }
}

void CRender::ParticleGroupFillName(xr_vector<shared_str>& s)
{
    for (const auto& pair : PSLibrary.IteratePGDs())
    {
        s.push_back(pair.first);
    }
}
float CRender::GetParticlesTimeLimit(LPCSTR name)
{
    if (const PS::CPEDef* effect = RImplementation.PSLibrary.FindPED(name))
    {
        return effect->m_Flags.is(PS::CPEDef::dfTimeLimit) ? effect->m_fTimeLimit : -1.f;
    }

    PS::CPGDef* group = RImplementation.PSLibrary.FindPGD(name);
    return group->m_fTimeLimit;
}


u32 CRender::GetCacheStatPolys()
{
    return RCache.stat.polys;
}

template <typename T>
static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, T*& result, bool const disasm, const char* dbg_name)
{
    result->sh = ShaderTypeTraits<T>::CreateHWShader(buffer, buffer_size);

    if (result->sh)
    {
        DXUT_SetDebugName(result->sh, dbg_name);
    }

    ID3DShaderReflection* pReflection = nullptr;

    HRESULT const _hr = D3DReflect(buffer, buffer_size, IID_ID3DShaderReflection, (void**)&pReflection);
    if (SUCCEEDED(_hr) && pReflection)
    {
        // Parse constant table data
        result->constants.parse(pReflection, ShaderTypeTraits<T>::GetShaderDest());

        _RELEASE(pReflection);
    }
    else
    {
        Msg("! D3DReflectShader [%s] hr == 0x%08x", file_name, _hr);
    }

    return _hr;
}

static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, void*& result, bool const disasm)
{
    string128 dbg_name{}, dbg_ext{};
    _splitpath(file_name, nullptr, nullptr, dbg_name, dbg_ext);
    strcat_s(dbg_name, dbg_ext);

    HRESULT _result;
    if (pTarget[0] == 'p')
    {
        SPS* sps_result = (SPS*)result;
        _result = HW.pDevice->CreatePixelShader(buffer, buffer_size, nullptr, &sps_result->sh);
        if (FAILED(_result))
        {
            Msg("! PS: [%s]", file_name);
            Msg("! CreatePixelShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (sps_result->sh)
        {
            DXUT_SetDebugName(sps_result->sh, dbg_name);
        }

        ID3DShaderReflection* pReflection = nullptr;

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

        _result = HW.pDevice->CreateVertexShader(buffer, buffer_size, nullptr, &svs_result->sh);

        if (FAILED(_result))
        {
            Msg("! VS: [%s]", file_name);
            Msg("! CreatePixelShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (svs_result->sh)
        {
            DXUT_SetDebugName(svs_result->sh, dbg_name);
        }

        ID3DShaderReflection* pReflection = nullptr;

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
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SGS*&)result, disasm, dbg_name);
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
        ID3DBlob* disasm = nullptr;
        D3DDisassemble(buffer, buffer_size, FALSE, nullptr, &disasm);
        // Пусть дизассемблятся по пути шейдеркэша, всё равно его теперь нету.
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
        strconcat(sizeof(pname), pname, RImplementation.getShaderPath(), pFileName);
        R = FS.r_open(fsgame::game_shaders, pname);
        if (!R)
        {
            // possibly in shared directory or somewhere else - open directly
            R = FS.r_open(fsgame::game_shaders, pFileName);
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
    defines.reserve(50);

    // options:
    const auto appendShaderOption = [&](u32 option, const char* macro, const char* value) {
        if (option)
            defines.emplace_back(macro, value);
    };

    // Don't move these variables to lower scope!
    char c_smapsize[10]{};
    char c_sun_shafts[10]{};
    char c_ssao[10]{};
    char c_sun_quality[10]{};
    char c_rain_quality[10]{};
    char c_ssfx_pom_refine[10]{};

    {
        sprintf_s(c_smapsize, "%d", m_SMAPSize);
        defines.emplace_back("SMAP_size", c_smapsize);
    }

    // FP16 Filter
    appendShaderOption(1, "FP16_FILTER", "1");

    // FP16 Blend
    appendShaderOption(1, "FP16_BLEND", "1");

    // HW smap
    appendShaderOption(1, "USE_HWSMAP", "1");

    // HW smap PCF
    appendShaderOption(1, "USE_HWSMAP_PCF", "1");

    // Branching
    appendShaderOption(HW.Caps.raster_major >= 3, "USE_BRANCHING", "1");

    // Vertex texture fetch
    appendShaderOption(HW.Caps.geometry.bVTF, "USE_VTF", "1");

    // Motion blur
    appendShaderOption(o.mblur, "USE_MBLUR", "1");

    appendShaderOption(ps_r_ao_mode == AO_MODE_GTAO, "USE_GTAO", "1");

    if (ps_r_ao_quality)
    {
        sprintf_s(c_ssao, "%d", ps_r_ao_quality);
        defines.emplace_back("SSAO_QUALITY", c_ssao);
    }

    // skinning
    // SKIN_NONE
    appendShaderOption(m_skinning < 0, "SKIN_NONE", "1");

    // SKIN_0
    appendShaderOption(0 == m_skinning, "SKIN_0", "1");

    // SKIN_1
    appendShaderOption(1 == m_skinning, "SKIN_1", "1");

    // SKIN_2
    appendShaderOption(2 == m_skinning, "SKIN_2", "1");

    // SKIN_3
    appendShaderOption(3 == m_skinning, "SKIN_3", "1");

    // SKIN_4
    appendShaderOption(4 == m_skinning, "SKIN_4", "1");

    // Soft water
    appendShaderOption(TRUE, "USE_SOFT_WATER", "1");

    // Soft particles
    appendShaderOption(TRUE, "USE_SOFT_PARTICLES", "1");

    // Depth of field
    {
        const bool dof = ps_r2_ls_flags.test(R2FLAG_DOF);
        appendShaderOption(dof, "USE_DOF", "1");
    }

    if (ps_r_sun_shafts)
    {
        sprintf_s(c_sun_shafts, "%d", ps_r_sun_shafts);
        defines.emplace_back("SUN_SHAFTS_QUALITY", c_sun_shafts);
    }

    if (ps_r_sun_quality)
    {
        sprintf_s(c_sun_quality, "%d", ps_r_sun_quality);
        defines.emplace_back("SUN_QUALITY", c_sun_quality);
    }

    // Steep parallax
    {
        const bool steepParallax = ps_r2_ls_flags.test(R2FLAG_STEEP_PARALLAX);
        appendShaderOption(steepParallax, "ALLOW_STEEPPARALLAX", "1");
    }

    // Geometry buffer optimization
    appendShaderOption(TRUE, "GBUFFER_OPTIMIZATION", "1");

    // Shader Model 5.0
    appendShaderOption(HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0, "SM_5", "1");

    // SSS

    appendShaderOption(ps_r2_ls_flags_ext.test(R2FLAGEXT_SSLR), "SSLR_ENABLED", "1");

    appendShaderOption(ps_r2_ls_flags.test(R2FLAG_SSFX_HEIGHT_FOG), "SSFX_FOG", "1");

    appendShaderOption(ps_r2_ls_flags.test(R2FLAG_SSFX_SKY_DEBANDING), "SSFX_DEBAND", "1");

    appendShaderOption(ps_r2_ls_flags.test(R2FLAG_SSFX_INDIRECT_LIGHT), "SSFX_INDIRECT_LIGHT", "1");

    appendShaderOption(ps_r2_ls_flags.test(R2FLAG_SSFX_BLOOM), "SSFX_BLOOM", "1");

    appendShaderOption(ps_r2_ls_flags_ext.test(R2FLAGEXT_TERRAIN_PARALLAX), "TERRAIN_PARALLAX_ENABNLED", "1");

    appendShaderOption(ps_r2_ls_flags_ext.test(R2FLAGEXT_REFLECTIONS_ONLY_ON_TERRAIN), "REFLECTIONS_ONLY_ON_TERRAIN", "1");

    appendShaderOption(ps_r2_ls_flags_ext.test(R2FLAGEXT_REFLECTIONS_ONLY_ON_PUDDLES), "REFLECTIONS_ONLY_ON_PUDDLES", "1");

    if (ps_ssfx_rain_1.w > 0.f)
    {
        sprintf_s(c_rain_quality, "%.0f", ps_ssfx_rain_1.w);
        defines.emplace_back("SSFX_RAIN_QUALITY", c_rain_quality);
    }

    if (ps_ssfx_pom_refine)
    {
        sprintf_s(c_ssfx_pom_refine, "%d", ps_ssfx_pom_refine);
        defines.emplace_back("SSFX_POM_REFINE", c_ssfx_pom_refine);
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
    FS.update_path(file_name, fsgame::app_data_root, file);

    {
        includer Includer;
        LPD3DBLOB pShaderBuf{};
        LPD3DBLOB pErrorBuf{};

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
                Msg("! Can't compile shader hr=0x%08x", _result);
        }
        return _result;
    }
}

void CRender::ClearTarget()
{
    RCache.ClearRT(RCache.get_RT(), {}); // black
}

void CRender::SetCacheXform(Fmatrix& mView, Fmatrix& mProject)
{
    for (auto& id : contexts_pool)
    {
        id.cmd_list.set_xform_view(mView);
        id.cmd_list.set_xform_project(mProject);
    }
}

void CRender::SetCacheXformOld(Fmatrix& mView, Fmatrix& mProject)
{
    for (auto& id : contexts_pool)
    {
        id.cmd_list.set_xform_view_old(mView);
        id.cmd_list.set_xform_project_old(mProject);
    }
}

void CRender::OnDeviceCreate()
{
    for (int id = 0; id < R__NUM_CONTEXTS; ++id)
    {
        contexts_pool[id].cmd_list.context_id = id;
        contexts_pool[id].cmd_list.OnDeviceCreate();
    }
}

void CRender::OnDeviceDestroy()
{
    for (auto& id : contexts_pool)
    {
        id.cmd_list.OnDeviceDestroy();
    }
}

void CRender::OnCameraUpdated(bool from_actor)
{
    ::Render->ViewBase.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

    if (from_actor)
    {
        if (g_pGameLevel)
        {
            r_main.wait_static();

            if (!ShouldSkipRender())
            {
                r_main.init();
                r_main.calculate_static();
            }
        }
    }
}

void CRender::Begin()
{
    for (auto& id : contexts_pool)
    {
        id.cmd_list.OnFrameBegin();
        id.cmd_list.set_CullMode(CULL_CW);
        id.cmd_list.set_CullMode(CULL_CCW);
    }

    Vertex.Flush();
    Index.Flush();
}

void CRender::Clear()
{
    RCache.ClearZB(RCache.get_ZB(), 1.0f, 0);
    if (psDeviceFlags.test(rsClearBB))
    {
        RCache.ClearRT(RCache.get_RT(), {}); // black
    }
}

void CRender::End()
{
    r_main.sync();

    // DoAsyncScreenshot();

    R_ASSERT(HW.pDevice);

    {
        ZoneScopedN("Cleanup");

        for (auto& id : contexts_pool)
        {
            id.cmd_list.OnFrameEnd();
        }

        cleanup_contexts();
    }

    {
        ZoneScopedN("PresentWait");

        HW.WaitOnSwapChain(); // wait for prev Present to finish. not sure that it is best place to wait, but it works
    }

    {
        ZoneScopedN("fps_lock");

        const auto now = std::chrono::high_resolution_clock::now();
        static auto s_LastPresentTime = now;

        constexpr long long menuFPSlimit{60ll}, pauseFPSlimit{60ll};
        const long long curFPSLimit = IsMainMenuActive() ? menuFPSlimit : Device.Paused() ? pauseFPSlimit : g_dwFPSlimit;

        if (curFPSLimit > 0ll)
        {
            const auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(now - s_LastPresentTime).count();
            // конверсия лимита фпс в микросекунды
            const auto targetFrameTime = 1000000ll / curFPSLimit;

            if (frameTime < targetFrameTime)
            {
                // считаем сколько спать
                const auto remainingTime = targetFrameTime - frameTime;
                const auto sleepTime = remainingTime / 1000ll;

                if (sleepTime > 2ll)
                {
                    Sleep(static_cast<DWORD>(sleepTime - 2ll)); // поспать на 2мс меньше, чтоб не переспать
                }

                // spinwait оставшееся время
                const auto spinWaitEnd = now + std::chrono::microseconds(targetFrameTime - frameTime);
                while (std::chrono::high_resolution_clock::now() < spinWaitEnd)
                {
                    YieldProcessor();
                }
            }
        }

        s_LastPresentTime = std::chrono::high_resolution_clock::now();
    }

    {
        ZoneScopedN("Present");

        if (psDeviceFlags.test(rsVSync))
            {
                // 512: DXGI_PRESENT_ALLOW_TEARING - allows for true V-Sync off with flip model
                HW.m_pSwapChain->Present(1, 0);
            }
        else
            {
                HW.m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
            }
    }

    TracyD3D11Collect(HW.profiler_ctx);

    Target->reset_target_dimensions();
}