#include "stdafx.h"

#include "../../xr_3da/render.h"

#include "../xrRender/ResourceManager.h"
#include "../xrRender/tss.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"

#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../xrRenderDX10/dx10ConstantBuffer.h"

#include "../xrRender/ShaderResourceTraits.h"

#include <Utilities\FlexibleVertexFormat.h>

SHS* CResourceManager::_CreateHS(LPCSTR Name) { return CreateShader<SHS>(Name); }

void CResourceManager::_DeleteHS(const SHS* HS) { DestroyShader(HS); }

SDS* CResourceManager::_CreateDS(LPCSTR Name) { return CreateShader<SDS>(Name); }

void CResourceManager::_DeleteDS(const SDS* DS) { DestroyShader(DS); }

SCS* CResourceManager::_CreateCS(LPCSTR Name) { return CreateShader<SCS>(Name); }

void CResourceManager::_DeleteCS(const SCS* CS) { DestroyShader(CS); }

SGS* CResourceManager::_CreateGS(LPCSTR Name) { return CreateShader<SGS>(Name); }

void CResourceManager::_DeleteGS(const SGS* GS) { DestroyShader(GS); }

template <class T>
BOOL reclaim(xr_vector<T*>& vec, const T* ptr)
{
    auto it = vec.begin();
    auto end = vec.end();
    for (; it != end; ++it)
        if (*it == ptr)
        {
            vec.erase(it);
            return TRUE;
        }
    return FALSE;
}

//--------------------------------------------------------------------------------------------------------------
SState* CResourceManager::_CreateState(SimulatorStates& state_code)
{
    // Search equal state-code
    for (const auto& C : v_states)
    {
        SimulatorStates& base = C->state_code;
        if (base.equal(state_code))
            return C;
    }

    // Create New
    v_states.push_back(xr_new<SState>());
    v_states.back()->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_states.back()->state = ID3DState::Create(state_code);
    v_states.back()->state_code = state_code;
    return v_states.back();
}
void CResourceManager::_DeleteState(const SState* state)
{
    if (0 == (state->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_states, state))
        return;
    Msg("! ERROR: Failed to find compiled stateblock");
}

//--------------------------------------------------------------------------------------------------------------
SPass* CResourceManager::_CreatePass(const SPass& proto)
{
    for (const auto& v_passe : v_passes)
        if (v_passe->equal(proto))
            return v_passe;

    SPass* P = xr_new<SPass>();
    P->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    P->state = proto.state;
    P->ps = proto.ps;
    P->vs = proto.vs;
    P->gs = proto.gs;
    P->hs = proto.hs;
    P->ds = proto.ds;
    P->cs = proto.cs;
    P->constants = proto.constants;
    P->T = proto.T;
    P->C = proto.C;

    v_passes.push_back(P);
    return v_passes.back();
}
void CResourceManager::_DeletePass(const SPass* P)
{
    if (0 == (P->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_passes, P))
        return;
    Msg("! ERROR: Failed to find compiled pass");
}

//--------------------------------------------------------------------------------------------------------------
SVS* CResourceManager::_CreateVS(LPCSTR _name)
{
    string_path name;
    xr_strcpy(name, _name);
    if (0 == RImplementation.m_skinning)
        xr_strcat(name, "_0");
    if (1 == RImplementation.m_skinning)
        xr_strcat(name, "_1");
    if (2 == RImplementation.m_skinning)
        xr_strcat(name, "_2");
    if (3 == RImplementation.m_skinning)
        xr_strcat(name, "_3");
    if (4 == RImplementation.m_skinning)
        xr_strcat(name, "_4");
    const LPSTR N = LPSTR(name);
    const map_VS::iterator I = m_vs.find(N);
    if (I != m_vs.end())
        return I->second;
    else
    {
        SVS* _vs = xr_new<SVS>();
        _vs->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_vs.insert(mk_pair(_vs->set_name(name), _vs));
        //_vs->vs				= NULL;
        //_vs->signature		= NULL;
        if (0 == stricmp(_name, "null"))
        {
            return _vs;
        }

        string_path shName;
        {
            const char* pchr = strchr(_name, '(');
            const ptrdiff_t size = pchr ? pchr - _name : xr_strlen(_name);
            strncpy_s(shName, _name, size);
            shName[size] = 0;
        }

        string_path cname;
        strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), /*_name*/ shName, ".vs");
        FS.update_path(cname, fsgame::game_shaders, cname);

        IReader* file = FS.r_open(cname);
        R_ASSERT(file, cname);

        file->skip_bom(cname);

        const std::string_view strbuf{reinterpret_cast<const char*>(file->pointer()), static_cast<size_t>(file->elapsed())};

        // Select target
        LPCSTR c_target = "vs_5_0";
        LPCSTR c_entry = "main";

        // xrSimpodin: Для воды снизил версию до 4.1 потому что с ней фиксится баг с неподвижной водой. Не понятно почему так происходит и проблема решается таким странным
        // способом. Можно было бы сменить c_entry на main_vs_4_1 но там куча шейдеров для воды сделано через инклуды и они не позволяют так сделать.
        if (!strncmp(shName, "water", strlen("water")))
        {
            c_target = "vs_4_1";
        }
        else if (strbuf.find("main_vs_4_1") != decltype(strbuf)::npos)
        {
            c_target = "vs_4_1";
            c_entry = "main_vs_4_1";
        }

        DWORD Flags{D3DCOMPILE_PACK_MATRIX_ROW_MAJOR};
        if (ps_r2_ls_flags_ext.test(R2FLAGEXT_SHADER_DBG))
        {
            Flags |= D3DCOMPILE_DEBUG;
            Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
        }

        HRESULT const _hr = RImplementation.shader_compile(name, reinterpret_cast<DWORD const*>(strbuf.data()), static_cast<UINT>(strbuf.size()), c_entry, c_target, Flags, (void*&)_vs);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s] file [%s]", name, cname);

        return _vs;
    }
}
void CResourceManager::_DeleteVS(const SVS* vs)
{
    if (0 == (vs->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    const LPSTR N = LPSTR(*vs->cName);
    const map_VS::iterator I = m_vs.find(N);
    if (I != m_vs.end())
    {
        m_vs.erase(I);

        for (const auto& iDecl : v_declarations)
        {
            xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout = iDecl->vs_to_layout.find(vs->signature->signature);
            if (iLayout != iDecl->vs_to_layout.end())
            {
                //	Release vertex layout
                _RELEASE(iLayout->second);
                iDecl->vs_to_layout.erase(iLayout);
            }
        }
        return;
    }
    Msg("! ERROR: Failed to find compiled vertex-shader '%s'", *vs->cName);
}

//--------------------------------------------------------------------------------------------------------------
SPS* CResourceManager::_CreatePS(LPCSTR _name)
{
    string_path name;
    xr_strcpy(name, _name);
    const LPSTR N = LPSTR(name);
    const map_PS::iterator I = m_ps.find(N);
    if (I != m_ps.end())
        return I->second;
    else
    {
        SPS* _ps = xr_new<SPS>();
        _ps->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_ps.insert(mk_pair(_ps->set_name(name), _ps));
        if (0 == stricmp(_name, "null"))
        {
            _ps->sh = nullptr;
            return _ps;
        }

        string_path shName;
        const char* pchr = strchr(_name, '(');
        const ptrdiff_t strSize = pchr ? pchr - _name : xr_strlen(_name);
        strncpy_s(shName, _name, strSize);
        shName[strSize] = 0;

        // Open file
        string_path cname;
        strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), /*_name*/ shName, ".ps");
        FS.update_path(cname, fsgame::game_shaders, cname);

        IReader* file = FS.r_open(cname);
        R_ASSERT(file, cname);

        file->skip_bom(cname);

        const std::string_view strbuf{reinterpret_cast<const char*>(file->pointer()), static_cast<size_t>(file->elapsed())};

        // Select target
        LPCSTR c_target = "ps_5_0";
        LPCSTR c_entry = "main";

        // xrSimpodin: Для воды снизил версию до 4.1 потому что с ней фиксится баг с неподвижной водой. Не понятно почему так происходит и проблема решается таким странным
        // способом.
        // Можно было бы сменить c_entry на main_ps_4_1 но там куча шейдеров для воды сделано через инклуды и они не позволяют так сделать.
        if (!strncmp(shName, "water", strlen("water")))
        {
            c_target = "ps_4_1";
        }
        else if (strbuf.find("main_ps_4_1") != decltype(strbuf)::npos)
        {
            c_target = "ps_4_1";
            c_entry = "main_ps_4_1";
        }

        DWORD Flags{D3DCOMPILE_PACK_MATRIX_ROW_MAJOR};
        if (ps_r2_ls_flags_ext.test(R2FLAGEXT_SHADER_DBG))
        {
            Flags |= D3DCOMPILE_DEBUG;
            Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
        }

        HRESULT const _hr = RImplementation.shader_compile(name, reinterpret_cast<DWORD const*>(strbuf.data()), static_cast<UINT>(strbuf.size()), c_entry, c_target, Flags, (void*&)_ps);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s] file [%s]", name, cname);

        return _ps;
    }
}
void CResourceManager::_DeletePS(const SPS* ps)
{
    if (0 == (ps->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    const LPSTR N = LPSTR(*ps->cName);
    const map_PS::iterator I = m_ps.find(N);
    if (I != m_ps.end())
    {
        m_ps.erase(I);
        return;
    }
    Msg("! ERROR: Failed to find compiled pixel-shader '%s'", *ps->cName);
}

//--------------------------------------------------------------------------------------------------------------

static BOOL dcl_equal(const D3DVERTEXELEMENT9* a, const D3DVERTEXELEMENT9* b)
{
    // check sizes
    const u32 a_size = FVF::GetDeclLength(a);
    const u32 b_size = FVF::GetDeclLength(b);
    if (a_size != b_size)
        return FALSE;
    return 0 == memcmp(a, b, a_size * sizeof(D3DVERTEXELEMENT9));
}

SDeclaration* CResourceManager::_CreateDecl(const D3DVERTEXELEMENT9* dcl)
{
    // Search equal code
    for (const auto& D : v_declarations)
    {
        if (!D->dcl_code.empty() && dcl_equal(dcl, &D->dcl_code.front()))
            return D;
    }

    // Create _new
    SDeclaration* D = xr_new<SDeclaration>();
    const u32 dcl_size = FVF::GetDeclLength(dcl) + 1;

    D->dcl_code.assign(dcl, dcl + dcl_size);
    dx10BufferUtils::ConvertVertexDeclaration(D->dcl_code, D->dx10_dcl_code);
    D->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_declarations.push_back(D);
    return D;
}
void CResourceManager::_DeleteDecl(const SDeclaration* dcl)
{
    if (0 == (dcl->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_declarations, dcl))
        return;
    Msg("! ERROR: Failed to find compiled vertex-declarator");
}

//--------------------------------------------------------------------------------------------------------------
R_constant_table* CResourceManager::_CreateConstantTable(R_constant_table& C)
{
    if (C.empty())
        return nullptr;

    for (const auto& v_constant_table : v_constant_tables)
        if (v_constant_table->equal(C))
            return v_constant_table;
    v_constant_tables.push_back(xr_new<R_constant_table>(C));
    v_constant_tables.back()->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    return v_constant_tables.back();
}
void CResourceManager::_DeleteConstantTable(const R_constant_table* C)
{
    if (0 == (C->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_constant_tables, C))
        return;
    Msg("! ERROR: Failed to find compiled constant-table");
}

//--------------------------------------------------------------------------------------------------------------
CRT* CResourceManager::_CreateRT(LPCSTR Name, u32 w, u32 h, DXGI_FORMAT f, u32 SampleCount /* = 1 */, u32 slices_num /*=1*/, Flags32 flags /*= {}*/)
{
    R_ASSERT(Name && Name[0] && w && h);

    // ***** first pass - search already created RT
    const LPSTR N = LPSTR(Name);
    const map_RT::iterator I = m_rtargets.find(N);
    if (I != m_rtargets.end())
        return I->second;
    else
    {
        CRT* RT = xr_new<CRT>();
        RT->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_rtargets.emplace(RT->set_name(Name), RT);
        if (Device.b_is_Ready)
            RT->create(Name, w, h, f, SampleCount, slices_num, flags);
        return RT;
    }
}
void CResourceManager::_DeleteRT(const CRT* RT)
{
    if (0 == (RT->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    const LPSTR N = LPSTR(*RT->cName);
    const map_RT::iterator I = m_rtargets.find(N);
    if (I != m_rtargets.end())
    {
        m_rtargets.erase(I);
        return;
    }
    Msg("! ERROR: Failed to find render-target '%s'", *RT->cName);
}

//--------------------------------------------------------------------------------------------------------------
void CResourceManager::DBG_VerifyGeoms()
{
}

SGeometry* CResourceManager::CreateGeom(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
{
    R_ASSERT(decl && vb);

    SDeclaration* dcl = _CreateDecl(decl);
    const u32 vb_stride = FVF::ComputeVertexSize(decl, 0);

    // ***** first pass - search already loaded shader
    for (const auto& v_geom : v_geoms)
    {
        SGeometry& G = *v_geom;
        if ((G.dcl == dcl) && (G.vb == vb) && (G.ib == ib) && (G.vb_stride == vb_stride))
            return v_geom;
    }

    SGeometry* Geom = xr_new<SGeometry>();
    Geom->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    Geom->dcl = dcl;
    Geom->vb = vb;
    Geom->vb_stride = vb_stride;
    Geom->ib = ib;
    v_geoms.push_back(Geom);
    return Geom;
}
SGeometry* CResourceManager::CreateGeom(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
{
    auto dcl = xr_vector<D3DVERTEXELEMENT9>(MAXD3DDECLLENGTH + 1);
    CHK_DX(FVF::CreateDeclFromFVF(FVF, dcl));
    SGeometry* g = CreateGeom(dcl.data(), vb, ib);
    return g;
}
void CResourceManager::DeleteGeom(const SGeometry* Geom)
{
    if (0 == (Geom->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_geoms, Geom))
        return;
    Msg("! ERROR: Failed to find compiled geometry-declaration");
}

CTexture* CResourceManager::_CreateTexture(LPCSTR _Name)
{
    // DBG_VerifyTextures	();
    if (0 == xr_strcmp(_Name, "null"))
        return nullptr;

    R_ASSERT(_Name && _Name[0]);

    string_path Name;
    xr_strcpy(Name, _Name);
    fix_texture_name(Name);

    // ***** first pass - search already loaded texture
    const LPSTR N = LPSTR(Name);
    const map_TextureIt I = m_textures.find(N);
    if (I != m_textures.end())
    {
        return I->second;
    }

    CTexture* T = xr_new<CTexture>();
    T->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    m_textures.insert(mk_pair(T->set_name(Name), T));
    T->Preload();

    const bool prefetch = Device.dwPrecacheFrame > 0;

    if (!prefetch)
        MsgDbg("~ %s name=%s", __FUNCTION__, Name);

    if (Device.b_is_Ready && !bDeferredLoad)
    {
        T->Load();
    }

    return T;
}
void CResourceManager::_DeleteTexture(const CTexture* T)
{
    // DBG_VerifyTextures	();

    if (0 == (T->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    const LPSTR N = LPSTR(*T->cName);
    const map_Texture::iterator I = m_textures.find(N);
    if (I != m_textures.end())
    {
        m_textures.erase(I);

        const bool prefetch = Device.dwPrecacheFrame > 0;

        if (!prefetch)
            MsgDbg("~ %s name=%s", __FUNCTION__, N);

        return;
    }
    Msg("! ERROR: Failed to find texture surface '%s'", *T->cName);
}

#ifdef DEBUG
void CResourceManager::DBG_VerifyTextures()
{
    map_Texture::iterator I = m_textures.begin();
    map_Texture::iterator E = m_textures.end();
    for (; I != E; I++)
    {
        R_ASSERT(I->first);
        R_ASSERT(I->second);
        R_ASSERT(I->second->cName);
        R_ASSERT(0 == xr_strcmp(I->first, *I->second->cName));
    }
}
#endif

bool cmp_tl(const std::pair<u32, ref_texture>& _1, const std::pair<u32, ref_texture>& _2) { return _1.first < _2.first; }

STextureList* CResourceManager::_CreateTextureList(STextureList& L)
{
    std::sort(L.begin(), L.end(), cmp_tl);
    for (const auto& base : lst_textures)
    {
        if (L.equal(*base))
            return base;
    }
    STextureList* lst = xr_new<STextureList>(L);
    lst->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    lst_textures.push_back(lst);
    return lst;
}
void CResourceManager::_DeleteTextureList(const STextureList* L)
{
    if (0 == (L->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(lst_textures, L))
        return;
    Msg("! ERROR: Failed to find compiled list of textures");
}

SMatrixList* CResourceManager::_CreateMatrixList(SMatrixList& L)
{
    BOOL bEmpty = TRUE;
    for (const auto& i : L)
        if (i)
        {
            bEmpty = FALSE;
            break;
        }
    if (bEmpty)
        return nullptr;

    for (const auto& base : lst_matrices)
    {
        if (L.equal(*base))
            return base;
    }
    SMatrixList* lst = xr_new<SMatrixList>(L);
    lst->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    lst_matrices.push_back(lst);
    return lst;
}
void CResourceManager::_DeleteMatrixList(const SMatrixList* L)
{
    if (0 == (L->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(lst_matrices, L))
        return;
    Msg("! ERROR: Failed to find compiled list of xform-defs");
}

SConstantList* CResourceManager::_CreateConstantList(SConstantList& L)
{
    BOOL bEmpty = TRUE;
    for (const auto& i : L)
        if (i)
        {
            bEmpty = FALSE;
            break;
        }
    if (bEmpty)
        return nullptr;

    for (const auto& base : lst_constants)
    {
        if (L.equal(*base))
            return base;
    }
    SConstantList* lst = xr_new<SConstantList>(L);
    lst->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    lst_constants.push_back(lst);
    return lst;
}
void CResourceManager::_DeleteConstantList(const SConstantList* L)
{
    if (0 == (L->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(lst_constants, L))
        return;
    Msg("! ERROR: Failed to find compiled list of r1-constant-defs");
}

dx10ConstantBuffer* CResourceManager::_CreateConstantBuffer(u32 context_id, ID3DShaderReflectionConstantBuffer* pTable)
{
    VERIFY(pTable);
    dx10ConstantBuffer* pTempBuffer = xr_new<dx10ConstantBuffer>(pTable);

    for (dx10ConstantBuffer* buf : v_constant_buffer[context_id])
    {
        if (pTempBuffer->Similar(*buf))
        {
            xr_delete(pTempBuffer);
            return buf;
        }
    }

    pTempBuffer->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_constant_buffer[context_id].push_back(pTempBuffer);
    return pTempBuffer;
}
bool CResourceManager::_DeleteConstantBuffer(u32 context_id, const dx10ConstantBuffer* pBuffer)
{
    if (0 == (pBuffer->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return true;
    if (reclaim(v_constant_buffer[context_id], pBuffer))
        return true;
    return false;
}

SInputSignature* CResourceManager::_CreateInputSignature(ID3DBlob* pBlob)
{
    VERIFY(pBlob);

    for (const auto& sign : v_input_signature)
    {
        if ((pBlob->GetBufferSize() == sign->signature->GetBufferSize()) && (!(memcmp(pBlob->GetBufferPointer(), sign->signature->GetBufferPointer(), pBlob->GetBufferSize()))))
        {
            return sign;
        }
    }

    SInputSignature* pSign = xr_new<SInputSignature>(pBlob);

    pSign->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_input_signature.push_back(pSign);

    return pSign;
}
void CResourceManager::_DeleteInputSignature(const SInputSignature* pSignature)
{
    if (0 == (pSignature->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_input_signature, pSignature))
        return;
    Msg("! ERROR: Failed to find compiled SInputSignature");
}