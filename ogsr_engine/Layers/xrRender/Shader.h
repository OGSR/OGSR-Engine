// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "r_constants.h"
#include "../../xrCore/xr_resource.h"

#include "sh_atomic.h"
#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"
#include "sh_rt.h"

typedef xr_vector<shared_str> sh_list;

class CBlender_Compile;
class IBlender;
class IBlenderXr;

#define SHADER_PASSES_MAX 2
#define SHADER_ELEMENTS_MAX 16

#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////////
struct ECORE_API STextureList : public xr_resource_flagged, public xr_vector<std::pair<u32, ref_texture>>
{
    typedef xr_vector<std::pair<u32, ref_texture>> inherited_vec;

    STextureList() = default;
    ~STextureList();

    IC BOOL equal(const STextureList& base) const
    {
        if (size() != base.size())
            return FALSE;
        for (u32 cmp = 0; cmp < size(); cmp++)
        {
            if ((*this)[cmp].first != base[cmp].first)
                return FALSE;
            if ((*this)[cmp].second != base[cmp].second)
                return FALSE;
        }
        return TRUE;
    }
    void clear();

    //	Avoid using this function.
    //	If possible use precompiled texture list.
    u32 find_texture_stage(const shared_str& TexName) const;
};
typedef resptr_core<STextureList, resptr_base<STextureList>> ref_texture_list;
//////////////////////////////////////////////////////////////////////////
struct ECORE_API SMatrixList : public xr_resource_flagged, public svector<ref_matrix, 4>
{
    ~SMatrixList();
};
typedef resptr_core<SMatrixList, resptr_base<SMatrixList>> ref_matrix_list;
//////////////////////////////////////////////////////////////////////////
struct ECORE_API SConstantList : public xr_resource_flagged, public svector<ref_constant_obsolette, 4>
{
    ~SConstantList();
};
typedef resptr_core<SConstantList, resptr_base<SConstantList>> ref_constant_list;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SGeometry : public xr_resource_flagged
{
    ref_declaration dcl;
    ID3DVertexBuffer* vb;
    ID3DIndexBuffer* ib;
    u32 vb_stride;
    SGeometry() = default;
    ~SGeometry();
};

struct ECORE_API resptrcode_geom : public resptr_base<SGeometry>
{
    void create(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    void create(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    void destroy() { _set(nullptr); }
    u32 stride() const { return _get()->vb_stride; }
};

typedef resptr_core<SGeometry, resptrcode_geom> ref_geom;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API SPass : public xr_resource_flagged
{
    ref_state state; // Generic state, like Z-Buffering, samplers, etc
    ref_ps ps; // may be NULL = FFP, in that case "state" must contain TSS setup
    ref_vs vs; // may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
    ref_gs gs; // may be NULL = don't use geometry shader at all
    ref_hs hs; // may be NULL = don't use hull shader at all
    ref_ds ds; // may be NULL = don't use domain shader at all
    ref_cs cs; // may be NULL = don't use compute shader at all
    ref_ctable constants; // may be NULL

    ref_texture_list T;
    ref_constant_list C;

    SPass() = default;
    ~SPass();

    BOOL equal(const SPass& other) const;
};
typedef resptr_core<SPass, resptr_base<SPass>> ref_pass;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API ShaderElement : public xr_resource_flagged
{
public:
    struct Sflags
    {
        u32 iPriority : 2;
        u32 bStrictB2F : 1;
        u32 bEmissive : 1;
        u32 bDistort : 1;
        u32 bWmark : 1;
        u32 iScopeLense : 1;
    };

public:
    Sflags flags;
    svector<ref_pass, SHADER_PASSES_MAX> passes;

    ShaderElement();
    ~ShaderElement();
    BOOL equal(ShaderElement& S);
    BOOL equal(ShaderElement* S);
};
typedef resptr_core<ShaderElement, resptr_base<ShaderElement>> ref_selement;

//////////////////////////////////////////////////////////////////////////
struct ECORE_API Shader : public xr_resource_flagged
{
public:
    ref_selement E[6]; // R2 - 0=deffer,			1=norm_lod1(normal),	2=psm,			3=ssm,		4=dsm

    shared_str dbg_shader_name;
    shared_str dbg_texture_name;

    Shader() = default;
    ~Shader();

    BOOL equal(Shader& S);
    BOOL equal(Shader* S);
};
struct ECORE_API resptrcode_shader : public resptr_base<Shader>
{
    void create(LPCSTR s_shader = nullptr, LPCSTR s_textures = nullptr, LPCSTR s_constants = nullptr, LPCSTR s_matrices = nullptr);
    void create(IBlender* B, LPCSTR s_shader = nullptr, LPCSTR s_textures = nullptr, LPCSTR s_constants = nullptr, LPCSTR s_matrices = nullptr);
    void destroy() { _set(nullptr); }
};
typedef resptr_core<Shader, resptrcode_shader> ref_shader;

enum SE_R1
{
    SE_R1_NORMAL_HQ = 0, // high quality/detail
    SE_R1_NORMAL_LQ = 1, // normal or low quality
    SE_R1_LPOINT = 2, // add: point light
    SE_R1_LSPOT = 3, // add: spot light
    SE_R1_LMODELS = 4, // lighting info for models or shadowing from models
};

#pragma pack(pop)
