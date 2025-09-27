// TextureManager.cpp: implementation of the CResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ResourceManager.h"
#include "tss.h"
#include "blenders/blender.h"
#include "blenders/blender_recorder.h"

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

IBlender* CResourceManager::_GetBlender(LPCSTR Name)
{
    R_ASSERT(Name && Name[0]);

    auto I = m_blenders.find(Name);

    if (I == m_blenders.end())
    {
        FATAL("!![%s] DX10: Shader [%s] not found in library.", __FUNCTION__, Name);
        return nullptr;
    }

    return I->second;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CResourceManager::_ParseList(sh_list& dest, LPCSTR names)
{
    if (nullptr == names || 0 == names[0])
        names = "$null";

    dest.clear();
    const char* P = (char*)names;
    svector<char, 128> N;

    while (*P)
    {
        if (*P == ',')
        {
            // flush
            N.push_back(0);
            strlwr(N.begin());

            fix_texture_name(N.begin());

            dest.emplace_back(N.begin());
            N.clear();
        }
        else
        {
            N.push_back(*P);
        }
        P++;
    }

    if (!N.empty())
    {
        // flush
        N.push_back(0);
        strlwr(N.begin());

        fix_texture_name(N.begin());

        dest.emplace_back(N.begin());
    }
}

ShaderElement* CResourceManager::_CreateElement(ShaderElement&& S)
{
    if (S.passes.empty())
        return nullptr;

    // Search equal in shaders array
    for (const auto& v_element : v_elements)
        if (S.equal(*v_element))
            return v_element;

    // Create _new_ entry
    ShaderElement* N = v_elements.emplace_back(xr_new<ShaderElement>(std::move(S)));
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    return N;
}

void CResourceManager::_DeleteElement(const ShaderElement* S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_elements, S))
        return;
    Msg("! ERROR: Failed to find compiled 'shader-element'");
}

Shader* CResourceManager::Create(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    return _cpp_Create(B, s_shader, s_textures, s_constants, s_matrices);
}

Shader* CResourceManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    //	TODO: DX10: When all shaders are ready switch to common path
    if (_lua_HasShader(s_shader))
        return _lua_Create(s_shader, s_textures);

    if (Shader* pShader = _cpp_Create(s_shader, s_textures, s_constants, s_matrices))
        return pShader;

    if (_lua_HasShader("stub_default"))
        return _lua_Create("stub_default", s_textures);

    FATAL("Can't find stub_default.s");
    return nullptr;
}

Shader* CResourceManager::_cpp_Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    //	TODO: DX10: When all shaders are ready switch to common path
    IBlender* pBlender = _GetBlender(s_shader ? s_shader : "null");
    if (!pBlender)
        return nullptr;
    return _cpp_Create(pBlender, s_shader, s_textures, s_constants, s_matrices);
}

Shader* CResourceManager::_cpp_Create(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    CBlender_Compile C;
    Shader S;

    // Access to template
    C.BT = B;
    C.bEditor = FALSE;
    C.bDetail = FALSE;
    C.HudElement = RImplementation.hud_loading;

    // Parse names
    _ParseList(C.L_textures, s_textures);
    _ParseList(C.L_constants, s_constants);
    _ParseList(C.L_matrices, s_matrices);

    {
        // need lock ???
        std::scoped_lock scope(ResourceEngineLock);

        // Compile element	(LOD0 - HQ)
        {
            C.iElement = 0;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture);
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[0] = _CreateElement(std::move(E));
        }

        // Compile element	(LOD1)
        {
            C.iElement = 1;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture);
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[1] = _CreateElement(std::move(E));
        }

        // Compile element
        {
            C.iElement = 2;
            C.bDetail = FALSE;
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[2] = _CreateElement(std::move(E));
        }

        // Compile element
        {
            C.iElement = 3;
            C.bDetail = FALSE;
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[3] = _CreateElement(std::move(E));
        }

        // Compile element
        {
            C.iElement = 4;
            C.bDetail = TRUE; //.$$$ HACK :)
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[4] = _CreateElement(std::move(E));
        }

        // Compile element
        {
            C.iElement = 5;
            C.bDetail = FALSE;
            ShaderElement E;
            C._cpp_Compile(&E);
            S.E[5] = _CreateElement(std::move(E));
        }
    }

    std::scoped_lock scope(v_shaders_lock);

    // Search equal in shaders array
    for (auto* v_shader : v_shaders)
        if (S.equal(v_shader))
            return v_shader;

    // Create _new_ entry
    Shader* N = xr_new<Shader>(S);
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    N->dbg_shader_name = s_shader;
    N->dbg_texture_name = s_textures;

    v_shaders.push_back(N);

    return N;
}


void CResourceManager::Delete(const Shader* S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    std::scoped_lock scope(v_shaders_lock);

    if (reclaim(v_shaders, S))
        return;
    Msg("! ERROR: Failed to find complete shader");
}

void CResourceManager::DeferredUpload()
{
    if (!Device.b_is_Ready)
        return;

    const int number_of_threads = TTAPI->get_num_threads();

    Msg("CResourceManager::DeferredUpload [%s] -> START, texture count = [%u], number_of_threads = %d", ps_r2_ls_flags_ext.test(R2FLAGEXT_MT_TEXLOAD) ? "MT" : "NO MT",
        m_textures.size(), number_of_threads);

    Msg("CResourceManager::DeferredUpload VRAM usage before:");
    
    u32 m_base = 0;
    u32 c_base = 0;
    u32 m_lmaps = 0;
    u32 c_lmaps = 0;

    _GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

    HW.DumpVideoMemoryUsage();

    // Теперь многопоточная загрузка текстур даёт очень существенный прирост скорости, проверено.
    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_MT_TEXLOAD))
    {
        for (const auto& pair : m_textures)
        {
            TTAPI->submit_detach([](CTexture* tex) { tex->Load(); }, pair.second);
        }

        TTAPI->wait_for_tasks();
    }
    else
        for (const auto& pair : m_textures)
            pair.second->Load();

    Msg("CResourceManager::DeferredUpload VRAM usage after:");

    _GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

    HW.DumpVideoMemoryUsage();

    Msg("CResourceManager::DeferredUpload -> END");
}

void CResourceManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
    m_base = c_base = m_lmaps = c_lmaps = 0;

    map_Texture::iterator I = m_textures.begin();
    const map_Texture::iterator E = m_textures.end();
    for (; I != E; ++I)
    {
        const u32 m = I->second->flags.memUsage;
        if (strstr(I->first, "lmap"))
        {
            c_lmaps++;
            m_lmaps += m;
        }
        else
        {
            c_base++;
            m_base += m;
        }
    }
}
void CResourceManager::_DumpMemoryUsage()
{
    xr_multimap<u32, std::pair<u32, shared_str>> mtex;

    // sort
    {
        map_Texture::iterator I = m_textures.begin();
        const map_Texture::iterator E = m_textures.end();
        for (; I != E; ++I)
        {
            u32 m = I->second->flags.memUsage;
            shared_str n = I->second->cName;
            mtex.insert(mk_pair(m, mk_pair(I->second->ref_count.load(), n)));
        }
    }

    // dump
    {
        xr_multimap<u32, std::pair<u32, shared_str>>::iterator I = mtex.begin();
        const xr_multimap<u32, std::pair<u32, shared_str>>::iterator E = mtex.end();
        for (; I != E; ++I)
            Msg("* %4.1f : [%4d] %s", float(I->first) / 1024.f, I->second.first, I->second.second.c_str());
    }
}

xr_vector<ITexture*> CResourceManager::FindTexture(const char* Name) const
{
    R_ASSERT(Name && strlen(Name));

    string_path filename;
    strcpy_s(filename, Name);
    fix_texture_name(filename);

    char* ch = strchr(filename, '*');

    xr_vector<ITexture*> res;

    if (!ch) // no wildcard?
    {
        const auto I = m_textures.find(filename);
        if (I != m_textures.end())
            res.emplace_back(smart_cast<ITexture*>(I->second));
    }
    else
    {
        // alpet: test for wildcard matching
        ch[0] = 0; // remove *

        for (const auto& [name, tex] : m_textures)
            if (strstr(name, filename))
                res.emplace_back(smart_cast<ITexture*>(tex));
    }

    return res;
}
