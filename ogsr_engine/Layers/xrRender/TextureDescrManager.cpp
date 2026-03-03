#include "stdafx.h"

#include "TextureDescrManager.h"
#include "ETextureParams.h"

// eye-params
float r__dtex_range = 50;

void fix_texture_thm_name(LPSTR fn)
{
    const LPSTR _ext = strext(fn);
    if (_ext && (0 == _stricmp(_ext, ".thm")))
        *_ext = 0;
}

void CTextureDescrMngr::LoadLTX()
{
    FS_FileSet flist;
    FS.file_list(flist, fsgame::game_textures, FS_ListFiles | FS_RootOnly, "*textures*.ltx");
    Msg("[%s] count of *textures*.ltx files: [%u]", __FUNCTION__, flist.size());

    for (const auto& file : flist)
    {
        string_path fn;
        FS.update_path(fn, fsgame::game_textures, file.name.c_str());
        CInifile ini(fn);

        if (ini.section_exist("association"))
        {
            auto& Sect = ini.r_section("association");

            m_texture_details.reserve(m_texture_details.size() + Sect.Data.size());
            m_detail_scalers.reserve(m_detail_scalers.size() + Sect.Data.size());

            for (const auto& [key, value] : Sect.Data)
            {
                texture_desc& desc = m_texture_details[key];
                if (desc.m_assoc)
                    xr_delete(desc.m_assoc);
                desc.m_assoc = xr_new<texture_assoc>();

                string_path T;
                float s;
                const int res = sscanf(value.c_str(), "%[^,],%f", T, &s);
                R_ASSERT(res == 2);
                desc.m_assoc->detail_name = T;

                m_detail_scalers[key] = s;

                desc.m_assoc->usage = 0;
                if (strstr(value.c_str(), "usage[diffuse_or_bump]"))
                    desc.m_assoc->usage = (1 << 0) | (1 << 1);
                else if (strstr(value.c_str(), "usage[bump]"))
                    desc.m_assoc->usage = (1 << 1);
                else if (strstr(value.c_str(), "usage[diffuse]"))
                    desc.m_assoc->usage = (1 << 0);
            }
        }

        if (ini.section_exist("specification"))
        {
            auto& Sect = ini.r_section("specification");

            m_texture_details.reserve(m_texture_details.size() + Sect.Data.size());

            for (const auto& [key, value] : Sect.Data)
            {
                texture_desc& desc = m_texture_details[key];
                if (desc.m_spec)
                    xr_delete(desc.m_spec);
                desc.m_spec = xr_new<texture_spec>();

                string_path bmode{}, bparallax{};
                const int res = sscanf(value.c_str(), "bump_mode[%[^]]], material[%f], parallax[%[^]]", bmode, &desc.m_spec->m_material, bparallax);
                ASSERT_FMT(res >= 2, "Something strange in textures.ltx. key:[%s], val:[%s]", key.c_str(), value.c_str());

                if ((bmode[0] == 'u') && (bmode[1] == 's') && (bmode[2] == 'e') && (bmode[3] == ':')) // bump-map specified
                    desc.m_spec->m_bump_name = bmode + 4;

                desc.m_spec->m_use_steep_parallax = (bparallax[0] == 'y') && (bparallax[1] == 'e') && (bparallax[2] == 's');
            }
        }
    }
}

void CTextureDescrMngr::LoadTHM(LPCSTR initial_path)
{
    FS_FileSet flist;
    FS.file_list(flist, initial_path, FS_ListFiles, "*.thm");

#ifdef DEBUG
    Msg("count of .thm files=%d", flist.size());
#endif // #ifdef DEBUG

    FS_FileSetIt It = flist.begin();
    const FS_FileSetIt It_e = flist.end();

    string_path full_name;

    m_texture_details.reserve(m_texture_details.size() + flist.size());
    m_detail_scalers.reserve(m_detail_scalers.size() + flist.size());

    for (; It != It_e; ++It)
    {
        LPCSTR file_nm = It->name.c_str();

        FS.update_path(full_name, initial_path, file_nm);
        IReader* F = FS.r_open(full_name);

        LoadTHMFile(F, file_nm);
    }
}

void CTextureDescrMngr::LoadTHMFile(IReader* F, LPCSTR file_nm)
{
    string_path key;
    xr_strcpy(key, file_nm);
    fix_texture_thm_name(key);

    R_ASSERT(F->find_chunk_thm(THM_CHUNK_TYPE, key));
    F->r_u32();

    STextureParams tp{};
    tp.Load(*F, key);
    FS.r_close(F);

        if (
#ifdef USE_SHOC_THM_FORMAT
            STextureParams::ttImage == tp.fmt || STextureParams::ttTerrain == tp.fmt || STextureParams::ttNormalMap == tp.fmt
#else
            STextureParams::ttImage == tp.type || STextureParams::ttTerrain == tp.type || STextureParams::ttNormalMap == tp.type
#endif
        )
        {
            texture_desc& desc = m_texture_details[key];

            if (tp.detail_name.size() && tp.flags.is_any(STextureParams::flDiffuseDetail | STextureParams::flBumpDetail))
            {
                if (desc.m_assoc)
                    xr_delete(desc.m_assoc);

                desc.m_assoc = xr_new<texture_assoc>();
                desc.m_assoc->detail_name = tp.detail_name;

                m_detail_scalers[key] = tp.detail_scale;

                desc.m_assoc->usage = 0;

                if (tp.flags.is(STextureParams::flDiffuseDetail))
                    desc.m_assoc->usage |= (1 << 0);

                if (tp.flags.is(STextureParams::flBumpDetail))
                    desc.m_assoc->usage |= (1 << 1);
            }
            if (desc.m_spec)
                xr_delete(desc.m_spec);

            desc.m_spec = xr_new<texture_spec>();
            desc.m_spec->m_material = static_cast<float>(tp.material) + tp.material_weight;
            desc.m_spec->m_use_steep_parallax = false;

            if (tp.bump_mode == STextureParams::tbmUse)
            {
                desc.m_spec->m_bump_name = tp.bump_name;
            }
            else if (tp.bump_mode == STextureParams::tbmUseParallax)
            {
                desc.m_spec->m_bump_name = tp.bump_name;
                desc.m_spec->m_use_steep_parallax = true;
            }
        }
}


void CTextureDescrMngr::EnsureTHMLoaded(const shared_str& tex_name)
{
    if (tex_name.empty())
        return;

    static bool dev_reference_copy = strstr(Core.Params, "-dev_reference_copy");
    if (!dev_reference_copy)
        return;

    if (m_texture_details.contains(tex_name))
        return;

    static xr_set<xr_string> checked;

    if (checked.contains(tex_name.c_str()))
        return;

    checked.emplace(tex_name.c_str());

    string_path file_nm;
    string_path full_name;

    xr_strcpy(file_nm, tex_name.c_str());
    xr_strcat(file_nm, ".thm");

    FS.update_path(full_name, fsgame::game_textures, file_nm);
    if (IReader* F = FS.r_open(full_name))
    {
        LoadTHMFile(F, file_nm);
    }
    else
    {
        FS.update_path(full_name, fsgame::level, file_nm);
        if (IReader* F2 = FS.r_open(full_name))
        {
            LoadTHMFile(F2, file_nm);
        }
    }
}

void CTextureDescrMngr::Load()
{
#ifdef DEBUG
    CTimer TT;
    TT.Start();
#endif // #ifdef DEBUG

#ifdef USE_TEXTURES_LTX
    LoadLTX();
#endif
    LoadTHM(fsgame::game_textures);
    LoadTHM(fsgame::level);

#ifdef DEBUG
    Msg("load time=%d ms", TT.GetElapsed_ms());
#endif // #ifdef DEBUG
}

void CTextureDescrMngr::UnLoad()
{
    auto I = m_texture_details.begin();
    const auto E = m_texture_details.end();
    for (; I != E; ++I)
    {
        xr_delete(I->second.m_assoc);
        xr_delete(I->second.m_spec);
    }
    m_texture_details.clear();
}

CTextureDescrMngr::~CTextureDescrMngr()
{
    m_detail_scalers.clear();
}

shared_str CTextureDescrMngr::GetBumpName(const shared_str& tex_name)
{
    EnsureTHMLoaded(tex_name);

    const auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_bump_name;
        }
    }
    return "";
}

BOOL CTextureDescrMngr::UseSteepParallax(const shared_str& tex_name)
{
    EnsureTHMLoaded(tex_name);

    const auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_use_steep_parallax;
        }
    }
    return FALSE;
}

float CTextureDescrMngr::GetMaterial(const shared_str& tex_name)
{
    EnsureTHMLoaded(tex_name);

    const auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_material;
        }
    }
    return 1.0f;
}

float CTextureDescrMngr::GetScale(const shared_str& tex_name)
{
    EnsureTHMLoaded(tex_name);

    const auto It2 = m_detail_scalers.find(tex_name);
    return It2 == m_detail_scalers.end() ? 1.f : It2->second; // TA->cs;
}

void CTextureDescrMngr::GetTextureUsage(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump)
{
    EnsureTHMLoaded(tex_name);

    const auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_assoc)
        {
            const u8 usage = I->second.m_assoc->usage;
            bDiffuse = !!(usage & (1 << 0));
            bBump = !!(usage & (1 << 1));
        }
    }
}

BOOL CTextureDescrMngr::GetDetailTexture(const shared_str& tex_name, LPCSTR& res)
{
    EnsureTHMLoaded(tex_name);

    const auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_assoc)
        {
            const texture_assoc* TA = I->second.m_assoc;
            res = TA->detail_name.c_str();
            return TRUE;
        }
    }
    return FALSE;
}
