#pragma once

//#define RBackend_PGO

#ifdef RBackend_PGO
#define PGO(a) a
#else
#define PGO(a)
#endif

#include "r_DStreams.h"
#include "r_constants_cache.h"
#include "r_backend_xform.h"
#include "r_backend_hemi.h"

#include "../xrRenderPC_R4/r_backend_lod.h"
#include "../xrRenderDX10/StateManager/dx10StateManager.h"
#include "../xrRenderDX10/StateManager/dx10ShaderResourceStateCache.h"

#include "fvf.h"
#include "HW.h"
#include "SH_Texture.h"
#include "smap_allocator.h"
#include <comdef.h> // for _com_error

constexpr u32 CULL_CCW = D3DCULL_CCW;
constexpr u32 CULL_CW = D3DCULL_CW;
constexpr u32 CULL_NONE = D3DCULL_NONE;

///		detailed statistic
struct R_statistics_element
{
    u32 verts{}, draw_calls{};
    ICF void add(const u32 _verts)
    {
        verts += _verts;
        draw_calls++;
    }
};

struct R_statistics_element_instanced
{
    u32 instances_count{}, draw_calls_count{};
    ICF void add(u32 inst_cnt)
    {
        instances_count += inst_cnt;
        draw_calls_count++;
    }
};

struct R_statistics
{
    R_statistics_element s_static;
    R_statistics_element_instanced s_flora;
    R_statistics_element s_flora_lods;
    R_statistics_element_instanced s_details;
    R_statistics_element s_dynamic;
    R_statistics_element s_dynamic_inst;
    R_statistics_element s_dynamic_1B;
    R_statistics_element s_dynamic_2B;
    R_statistics_element s_dynamic_3B;
    R_statistics_element s_dynamic_4B;
};

struct FloraVertData
{
    float data[20];
};

#pragma warning(push)
#pragma warning(disable : 4324)
class ECORE_API CBackend
{
public:
    enum
    {
        MaxCBuffers = 14
    };

public:
    R_xforms xforms;
    R_hemi hemi;
    R_lod lod;

    ref_cbuffer m_aVertexConstants[MaxCBuffers];
    ref_cbuffer m_aPixelConstants[MaxCBuffers];
    ref_cbuffer m_aGeometryConstants[MaxCBuffers];
    ref_cbuffer m_aHullConstants[MaxCBuffers];
    ref_cbuffer m_aDomainConstants[MaxCBuffers];
    ref_cbuffer m_aComputeConstants[MaxCBuffers];

    D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
    ID3DInputLayout* m_pInputLayout;
    DWORD dummy0; //	Padding to avoid warning
    DWORD dummy1; //	Padding to avoid warning
    DWORD dummy2; //	Padding to avoid warning

private:
    // Render-targets
    ID3DRenderTargetView* pRT[4];
    ID3DDepthStencilView* pZB;

    // Vertices/Indices/etc
    SDeclaration* decl;
    ID3DVertexBuffer* vb;
    ID3DIndexBuffer* ib;
    u32 vb_stride;

    // Pixel/Vertex constants
    ALIGN(16) R_constants constants;
    R_constant_table* ctable;

    // Shaders/State
    ID3DState* state;
    ID3DPixelShader* ps;
    ID3DVertexShader* vs;
    ID3DGeometryShader* gs;
    ID3D11HullShader* hs;
    ID3D11DomainShader* ds;
    ID3D11ComputeShader* cs;

#ifdef DEBUG
    LPCSTR ps_name;
    LPCSTR vs_name;
    LPCSTR gs_name;
    LPCSTR hs_name;
    LPCSTR ds_name;
    LPCSTR cs_name;
#endif

    // Lists
    STextureList* T;
    SMatrixList* M;
    SConstantList* C;

    // Lists-expanded
    CTexture* textures_ps[CTexture::mtMaxPixelShaderTextures]; // stages
    CTexture* textures_vs[CTexture::mtMaxVertexShaderTextures]; // 4 vs
    CTexture* textures_gs[CTexture::mtMaxGeometryShaderTextures]; // 4 vs
    CTexture* textures_hs[CTexture::mtMaxHullShaderTextures]; // 4 vs
    CTexture* textures_ds[CTexture::mtMaxDomainShaderTextures]; // 4 vs
    CTexture* textures_cs[CTexture::mtMaxComputeShaderTextures]; // 4 vs

    static constexpr u32 FloraVbufSizes[]{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 1500, 2048, 3000, 4096, 5000, 6000, 7000, 8192};
    std::map<u32, ID3DVertexBuffer*> FloraVbuffers;

public:
    struct _stats
    {
        u32 calls;
        u32 polys;
        u32 verts;
        u32 vs;
        u32 ps;
        u32 gs;
        u32 hs;
        u32 ds;
        u32 cs;

        u32 decl;
        u32 vb;
        u32 ib;
        u32 states; // Number of times the shader-state changes
        u32 textures; // Number of times the shader-tex changes
        u32 constants; // Number of times the shader-consts changes
        u32 xforms;
        u32 target_rt;
        u32 target_zb;

        R_statistics r;
    } stat{};

    u32 context_id{CHW::INVALID_CONTEXT_ID};

public:
    CTexture* get_ActiveTexture(u32 stage) const
    {
        if (stage < CTexture::rstVertex)
            return textures_ps[stage];
        else if (stage < CTexture::rstGeometry)
            return textures_vs[stage - CTexture::rstVertex];
        else if (stage < CTexture::rstHull)
            return textures_gs[stage - CTexture::rstGeometry];
        else if (stage < CTexture::rstDomain)
            return textures_hs[stage - CTexture::rstHull];
        else if (stage < CTexture::rstCompute)
            return textures_ds[stage - CTexture::rstDomain];
        else if (stage < CTexture::rstInvalid)
            return textures_cs[stage - CTexture::rstCompute];
        else
        {
            VERIFY(!"Invalid texture stage");
            return nullptr;
        }
    }

    IC void get_ConstantDirect(const shared_str& n, u32 DataSize, void** pVData, void** pGData, void** pPData, bool assert = false);

    IC void set_xform_world(const Fmatrix& M) { xforms.set_W(M); } // position of item 
    IC void set_xform_view(const Fmatrix& M) { xforms.set_V(M); }
    IC void set_xform_project(const Fmatrix& M) { xforms.set_P(M); }
    IC void set_xform_world_old(const Fmatrix& M_) { xforms.set_W_old(M_); }
    IC void set_xform_view_old(const Fmatrix& M_) { xforms.set_V_old(M_); }
    IC void set_xform_project_old(const Fmatrix& M_) { xforms.set_P_old(M_); }

    u32 curr_rt_width{};
    u32 curr_rt_height{};

    IC void set_pass_targets(const ref_rt& mrt0, const ref_rt& mrt1, const ref_rt& mrt2, const ref_rt& mrt3, const ref_rt& zb);

    IC void set_RT(ID3DRenderTargetView* RT, u32 ID = 0);
    IC void set_ZB(ID3DDepthStencilView* ZB);
    IC ID3DRenderTargetView* get_RT(u32 ID = 0) const;
    IC ID3DDepthStencilView* get_ZB() const;

    IC  void ClearRT(ID3DRenderTargetView* rt, const Fcolor& color) const;
    ICF void ClearRT(ref_rt& rt, const Fcolor& color) { ClearRT(rt->pRT, color); }

    IC void ClearZB(ID3DDepthStencilView* zb, float depth);
    IC void ClearZB(ID3DDepthStencilView* zb, float depth, u8 stencil);

    ICF void ClearZB(ref_rt& zb, float depth) { ClearZB(zb->pZRT[context_id], depth); }
    ICF void ClearZB(ref_rt& zb, float depth, u8 stencil) { ClearZB(zb->pZRT[context_id], depth, stencil); }

    IC void set_Constants(R_constant_table* C);
    void set_Constants(ref_ctable& C) { set_Constants(&*C); }

    void set_Textures(STextureList* T);
    void set_Textures(ref_texture_list& T) { set_Textures(&*T); }

    IC void set_Element(ShaderElement* S, u32 pass = 0);
    void set_Element(ref_selement& S, u32 pass = 0) { set_Element(&*S, pass); }

    IC void set_Shader(Shader* S, u32 pass = 0);
    void set_Shader(ref_shader& S, u32 pass = 0) { set_Shader(&*S, pass); }

    ICF void set_States(ID3DState* _state);
    ICF void set_States(ref_state& _state) { set_States(_state->state); }

    IC void set_Pass(SPass* P);
    void set_Pass(const ref_pass& P) { set_Pass(&*P); }

    ICF void set_PS(const ref_ps& _ps) { set_PS(_ps->sh, _ps->cName.c_str()); }
    ICF void set_GS(const ref_gs& _gs) { set_GS(_gs->sh, _gs->cName.c_str()); }
    ICF void set_HS(const ref_hs& _hs) { set_HS(_hs->sh, _hs->cName.c_str()); }
    ICF void set_DS(const ref_ds& _ds) { set_DS(_ds->sh, _ds->cName.c_str()); }
    ICF void set_CS(const ref_cs& _cs) { set_CS(_cs->sh, _cs->cName.c_str()); }
    ICF void set_VS(const ref_vs& _vs);

    ICF bool is_TessEnabled() const;

    void Invalidate();

    // tmp
    ICF void set_Format(SDeclaration* _decl);

    ID3DVertexBuffer* GetFloraVbuff(u32& size);

protected:
    //	In DX10 we need input shader signature which is stored in ref_vs

    ICF void set_VS(ID3DVertexShader* _vs, LPCSTR _n = nullptr);
    ICF void set_GS(ID3DGeometryShader* _gs, LPCSTR _n = nullptr);
    ICF void set_HS(ID3D11HullShader* _hs, LPCSTR _n = nullptr);
    ICF void set_DS(ID3D11DomainShader* _ds, LPCSTR _n = nullptr);
    ICF void set_CS(ID3D11ComputeShader* _cs, LPCSTR _n = nullptr);

public:

    ICF void set_PS(ID3DPixelShader* _ps, LPCSTR _n = nullptr);

    ICF void set_Vertices(ID3DVertexBuffer* _vb, u32 _vb_stride);
    ICF void set_Vertices_Forced(const u32 count, ID3DVertexBuffer* const* _vb, const u32* _vb_stride, const u32* iOffset);
    ICF void set_Indices(ID3DIndexBuffer* _ib);
    ICF void set_Geometry(SGeometry* _geom);
    ICF void set_Geometry(ref_geom& _geom) { set_Geometry(&*_geom); }

    IC void set_Stencil(u32 _enable, u32 _func = D3DCMP_ALWAYS, u32 _ref = 0x00, u32 _mask = 0x00, u32 _writemask = 0x00, u32 _fail = D3DSTENCILOP_KEEP, u32 _pass = D3DSTENCILOP_KEEP, u32 _zfail = D3DSTENCILOP_KEEP);

    IC void set_Z(u32 _enable);
    IC void set_ZFunc(u32 _func);
    IC void set_AlphaRef(u32 _value);
    IC void set_ColorWriteEnable(u32 _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

    IC void set_CullMode(u32 _mode);
    IC void set_FillMode(u32 _mode);

    IC void set_Scissor(Irect* rect = nullptr);

    IC void SetViewport(const D3D_VIEWPORT& viewport) const;

    float o_hemi;
    float o_hemi_cube[6/*CROS_impl::NUM_FACES*/];
    float o_sun;

    void apply_lmaterial();

    // constants
    ICF ref_constant get_c(LPCSTR n)
    {
        if (ctable)
            return ctable->get(n);
        return nullptr;
    }
    ICF ref_constant get_c(const shared_str& n)
    {
        if (ctable)
            return ctable->get(n);
        return nullptr;
    }

    // constants - direct (fast)
    ICF void set_c(R_constant* C, const Fmatrix& A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, const Fvector2& A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, const Fvector3& A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, const Fvector4& A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, float x, float y)
    {
        if (C)
            constants.set(C, x, y);
    }
    ICF void set_c(R_constant* C, float x, float y, float z)
    {
        if (C)
            constants.set(C, x, y, z);
    }
    ICF void set_c(R_constant* C, float x, float y, float z, float w)
    {
        if (C)
            constants.set(C, x, y, z, w);
    }
    ICF void set_ca(R_constant* C, u32 e, const Fmatrix& A)
    {
        if (C)
            constants.seta(C, e, A);
    }
    ICF void set_ca(R_constant* C, u32 e, const Fvector2& A)
    {
        if (C)
            constants.seta(C, e, A);
    }
    ICF void set_ca(R_constant* C, u32 e, const Fvector3& A)
    {
        if (C)
            constants.seta(C, e, A);
    }
    ICF void set_ca(R_constant* C, u32 e, const Fvector4& A)
    {
        if (C)
            constants.seta(C, e, A);
    }
    ICF void set_ca(R_constant* C, u32 e, float x, float y)
    {
        if (C)
            constants.seta(C, e, x, y);
    }
    ICF void set_ca(R_constant* C, u32 e, float x, float y, float z)
    {
        if (C)
            constants.seta(C, e, x, y, z);
    }
    ICF void set_ca(R_constant* C, u32 e, float x, float y, float z, float w)
    {
        if (C)
            constants.seta(C, e, x, y, z, w);
    }
    ICF void set_c(R_constant* C, float A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, bool A)
    {
        if (C)
            constants.set(C, A);
    }
    ICF void set_c(R_constant* C, int A)
    {
        if (C)
            constants.set(C, A);
    }

    // constants - LPCSTR (slow)
    ICF void set_c(LPCSTR n, const Fmatrix& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, const Fvector2& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, const Fvector3& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, const Fvector4& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, float x, float y)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y);
    }
    ICF void set_c(LPCSTR n, float x, float y, float z)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y, z);
    }
    ICF void set_c(LPCSTR n, float x, float y, float z, float w)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y, z, w);
    }
    ICF void set_ca(LPCSTR n, u32 e, const Fmatrix& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(LPCSTR n, u32 e, const Fvector2& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(LPCSTR n, u32 e, const Fvector3& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(LPCSTR n, u32 e, const Fvector4& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(LPCSTR n, u32 e, float x, float y)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y);
    }
    ICF void set_ca(LPCSTR n, u32 e, float x, float y, float z)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y, z);
    }
    ICF void set_ca(LPCSTR n, u32 e, float x, float y, float z, float w)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y, z, w);
    }
    ICF void set_c(LPCSTR n, bool A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, float A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(LPCSTR n, int A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }

    // constants - shared_str (average)
    ICF void set_c(shared_str& n, const Fmatrix& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, const Fvector2& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, const Fvector3& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, const Fvector4& A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, float x, float y)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y);
    }
    ICF void set_c(shared_str& n, float x, float y, float z)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y, z);
    }
    ICF void set_c(shared_str& n, float x, float y, float z, float w)
    {
        if (ctable)
            set_c(&*ctable->get(n), x, y, z, w);
    }
    ICF void set_ca(shared_str& n, u32 e, const Fmatrix& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(shared_str& n, u32 e, const Fvector2& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(shared_str& n, u32 e, const Fvector3& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(shared_str& n, u32 e, const Fvector4& A)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, A);
    }
    ICF void set_ca(shared_str& n, u32 e, float x, float y)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y);
    }
    ICF void set_ca(shared_str& n, u32 e, float x, float y, float z)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y, z);
    }
    ICF void set_ca(shared_str& n, u32 e, float x, float y, float z, float w)
    {
        if (ctable)
            set_ca(&*ctable->get(n), e, x, y, z, w);
    }
    ICF void set_c(shared_str& n, bool A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, float A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }
    ICF void set_c(shared_str& n, int A)
    {
        if (ctable)
            set_c(&*ctable->get(n), A);
    }

    ICF void Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC, u32 inst_cnt = 0);
    ICF void Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC);
    ICF void Render(const u32 Vcount);

    ICF void submit()
    {
        R_ASSERT(context_id != CHW::IMM_CTX_ID);
        ID3D11CommandList* pCommandList{};

        const auto hr = HW.get_context(context_id)->FinishCommandList(false, &pCommandList);
        if (!FAILED(hr))
        {
            HW.get_context(CHW::IMM_CTX_ID)->ExecuteCommandList(pCommandList, false);
        }
        else
        {
            const auto hr2 = HW.pDevice->GetDeviceRemovedReason();
            FATAL("!!FAILED HW.get_context(%u)->FinishCommandList! Error: [%s]; GetDeviceRemovedReason returns: [%s]", context_id, _com_error{hr}.ErrorMessage(),
                FAILED(hr2) ? _com_error{hr2}.ErrorMessage() : "No device removal error detected");
        }

        _RELEASE(pCommandList);
    }

    //ICF void Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);

    void gpu_mark_begin(const wchar_t* name);
    void gpu_mark_end();

    // Device create / destroy / frame signaling
    void OnFrameBegin();
    void OnFrameEnd();
    void OnDeviceCreate();
    void OnDeviceDestroy();

    // Debug render
    void dbg_DP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 vBase, u32 pc);
    void dbg_DIP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);

    void dbg_SetRS(D3DRENDERSTATETYPE p1, u32 p2) { VERIFY(!"Not implemented"); }
    void dbg_SetSS(u32 sampler, D3DSAMPLERSTATETYPE type, u32 value) { VERIFY(!"Not implemented"); }

    void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, u16* pIdx, int pcnt);
    void dbg_Draw_Near(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, u16* pIdx, int pcnt);
    void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);

    void dbg_DrawAABB(Fvector& T, float sx, float sy, float sz, u32 C)
    {
        Fvector half_dim;
        half_dim.set(sx, sy, sz);
        Fmatrix TM;
        TM.translate(T);
        dbg_DrawOBB(TM, half_dim, C);
    }
    void dbg_DrawOBB(Fmatrix& T, Fvector& half_dim, u32 C);
    void dbg_DrawTRI(Fmatrix& T, Fvector* p, u32 C) { dbg_DrawTRI(T, p[0], p[1], p[2], C); }
    void dbg_DrawTRI(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);
    void dbg_DrawLINE(Fmatrix& T, Fvector& p1, Fvector& p2, u32 C);
    void dbg_DrawEllipse(Fmatrix& T, u32 C);

    CBackend();

    // debug

    //BOOL m_bNearer{};

    //void SetNearer(BOOL enabled)
    //{
    //    Fmatrix mProject = Device.mProject;
    //    if (enabled && !m_bNearer)
    //    {
    //        m_bNearer = TRUE;
    //        mProject._43 -= EPS_L;
    //    }
    //    else if (!enabled && m_bNearer)
    //    {
    //        m_bNearer = FALSE;
    //        mProject._43 += EPS_L;
    //    }
    //    set_xform_project(mProject);
    //}
    
private:
    // Debug Draw
    void InitializeDebugDraw();
    void DestroyDebugDraw();

    ref_geom vs_L, vs_TL;

    //	DirectX 10 internal functionality
    void ApplyVertexLayout();
    void ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology);
    bool CBuffersNeedUpdate(ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax);

private:
    ID3DBlob* m_pInputSignature;
    ID3DUserDefinedAnnotation* pAnnotation{};

    bool m_bChangedRTorZB{}, need_reset_vertbuf{};

public:
    void ApplyRTandZB();

    dx10StateManager StateManager;
    dx10ShaderResourceStateCache SRVSManager;

    SMAP_Allocator LP_smap_pool;
};
#pragma warning(pop)

#include "D3DUtils.h"

#define RCache RImplementation.get_imm_context().cmd_list
