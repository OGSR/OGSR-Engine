// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "Shader.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

string_unordered_map<std::string, ref_shader> g_ShadersCache;

ref_shader GetCachedShader(const char* sh, const char* tex)
{
    std::string key{sh};
    key += "_";
    key += tex ? tex : "";

    if (const auto it = g_ShadersCache.find(key); it != g_ShadersCache.end())
    {
        return it->second;
    }
    else
    {
        auto& shader = g_ShadersCache[key];
        shader.create(sh, tex);
        return shader;
    }
}

SGeometry::~SGeometry() { DEV->DeleteGeom(this); }

STextureList::~STextureList() { DEV->_DeleteTextureList(this); }

SMatrixList::~SMatrixList() { DEV->_DeleteMatrixList(this); }

SConstantList::~SConstantList() { DEV->_DeleteConstantList(this); }

SPass::~SPass() { DEV->_DeletePass(this); }

ShaderElement::~ShaderElement() { DEV->_DeleteElement(this); }

Shader::~Shader() { DEV->Delete(this); }

//////////////////////////////////////////////////////////////////////////
void resptrcode_shader::create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    _set(DEV->Create(s_shader, s_textures, s_constants, s_matrices));
}
void resptrcode_shader::create(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    _set(DEV->Create(B, s_shader, s_textures, s_constants, s_matrices));
}

//////////////////////////////////////////////////////////////////////////
void resptrcode_geom::create(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib) { _set(DEV->CreateGeom(FVF, vb, ib)); }
void resptrcode_geom::create(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib) { _set(DEV->CreateGeom(decl, vb, ib)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL SPass::equal(const SPass& other) const
{
    if (state != other.state)
        return FALSE;
    if (ps != other.ps)
        return FALSE;
    if (vs != other.vs)
        return FALSE;
    if (gs != other.gs)
        return FALSE;
    if (hs != other.hs)
        return FALSE;
    if (ds != other.ds)
        return FALSE;
    if (cs != other.cs)
        return FALSE;
    if (constants != other.constants)
        return FALSE; // is this nessesary??? (ps+vs already combines)
    if (T != other.T)
        return FALSE;
    if (C != other.C)
        return FALSE;
    return TRUE;
}

//
ShaderElement::ShaderElement()
{
    flags.iPriority = 1;
    flags.bStrictB2F = FALSE;
    flags.bEmissive = FALSE;
    flags.bDistort = FALSE;
    flags.bWmark = FALSE;
    flags.iScopeLense = FALSE;
}

BOOL ShaderElement::equal(ShaderElement& S)
{
    if (flags.iPriority != S.flags.iPriority)
        return FALSE;
    if (flags.bStrictB2F != S.flags.bStrictB2F)
        return FALSE;
    if (flags.bEmissive != S.flags.bEmissive)
        return FALSE;
    if (flags.bWmark != S.flags.bWmark)
        return FALSE;
    if (flags.iScopeLense != S.flags.iScopeLense)
        return FALSE;
    if (flags.bDistort != S.flags.bDistort)
        return FALSE;
    if (passes.size() != S.passes.size())
        return FALSE;
    for (u32 p = 0; p < passes.size(); p++)
        if (passes[p] != S.passes[p])
            return FALSE;
    return TRUE;
}

BOOL ShaderElement::equal(ShaderElement* S)
{
    if (nullptr == S && nullptr == this)
        return TRUE; //-V704
    if (nullptr == S || nullptr == this)
        return FALSE; //-V704
    return equal(*S);
}

//
BOOL Shader::equal(Shader& S) { return E[0]->equal(&*S.E[0]) && E[1]->equal(&*S.E[1]) && E[2]->equal(&*S.E[2]) && E[3]->equal(&*S.E[3]) && E[4]->equal(&*S.E[4]); }
BOOL Shader::equal(Shader* S) { return equal(*S); }

void STextureList::clear()
{
    for (auto& it : *this)
        it.second.destroy();

    inherited_vec::clear();
}

u32 STextureList::find_texture_stage(const shared_str& TexName) const
{
    u32 dwTextureStage = 0;

    STextureList::const_iterator _it = this->begin();
    const STextureList::const_iterator _end = this->end();
    for (; _it != _end; ++_it)
    {
        const std::pair<u32, ref_texture>& loader = *_it;

        //	Shadowmap texture always uses 0 texture unit
        if (loader.second->cName == TexName)
        {
            //	Assign correct texture
            dwTextureStage = loader.first;
            break;
        }
    }

    VERIFY(_it != _end);

    return dwTextureStage;
}
