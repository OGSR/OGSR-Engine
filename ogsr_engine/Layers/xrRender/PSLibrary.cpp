//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"


#include "PSLibrary.h"
#include "ParticleEffect.h"
#include "ParticleGroup.h"


#define _game_data_ "$game_data$"

bool ped_sort_pred(const PS::CPEDef* a, const PS::CPEDef* b) { return xr_strcmp(a->Name(), b->Name()) < 0; }
bool pgd_sort_pred(const PS::CPGDef* a, const PS::CPGDef* b) { return xr_strcmp(a->m_Name, b->m_Name) < 0; }

bool ped_find_pred(const PS::CPEDef* a, LPCSTR b) { return xr_strcmp(a->Name(), b) < 0; }
bool pgd_find_pred(const PS::CPGDef* a, LPCSTR b) { return xr_strcmp(a->m_Name, b) < 0; }
//----------------------------------------------------
void CPSLibrary::OnCreate()
{
    {
        LoadAll();
    }
}

void CPSLibrary::LoadAll() 
{
    Load2(); // load ltx pg and pe

    FS_FileSet flist;
    FS.file_list(flist, _game_data_, FS_ListFiles | FS_RootOnly, "*particles*.xr");
    Msg("[%s] count of *particles*.xr files: [%u]", __FUNCTION__, flist.size());

    //for (const auto& file : flist)
    //{
    //    string_path fn;
    //    FS.update_path(fn, _game_data_, file.name.c_str());

    //    if (!FS.exist(fn))
    //    {
    //        Msg("Can't find file: '%s'", fn);
    //    }

    //    if (!Load(fn))
    //    {
    //        Msg("CPSLibrary: Cannot load file: '%s'", fn);
    //    }
    //}

    string_path fn;

    FS.update_path(fn, _game_data_, "particles_cop.xr");
    if (FS.exist(fn))
    {
        Msg("Load [%s]", fn);

        Load(fn);
    }

    FS.update_path(fn, _game_data_, "particles.xr");
    if (FS.exist(fn))
    {
        Msg("Load [%s]", fn);

        Load(fn);
    }

    std::sort(m_PEDs.begin(), m_PEDs.end(), ped_sort_pred);
    std::sort(m_PGDs.begin(), m_PGDs.end(), pgd_sort_pred);

    for (auto& m_PED : m_PEDs)
        m_PED->CreateShader();
}

void CPSLibrary::ExportAllAsNew()
{
    string_path fn;
    FS.update_path(fn, _game_data_, "particles_cop.new_xr"); // dummy file name
    Msg("~ Exported all ltx shaders to [%s] in COP format. Rename file to use it.", fn);
    Save(fn);
}

void CPSLibrary::OnDestroy()
{
    for (auto& m_PED : m_PEDs)
    {
        m_PED->DestroyShader();
        xr_delete(m_PED);
    }
    m_PEDs.clear();

    for (auto& m_PGD : m_PGDs)
        xr_delete(m_PGD);
    m_PGDs.clear();
}
//----------------------------------------------------
PS::PEDIt CPSLibrary::FindPEDIt(LPCSTR Name)
{
    if (!Name)
        return m_PEDs.end();

    PS::PEDIt I = std::lower_bound(m_PEDs.begin(), m_PEDs.end(), Name, ped_find_pred);
    if (I == m_PEDs.end() || (0 != xr_strcmp((*I)->m_Name, Name)))
        return m_PEDs.end();
    else
        return I;
}

PS::CPEDef* CPSLibrary::FindPED(LPCSTR Name)
{
    PS::PEDIt it = FindPEDIt(Name);
    return (it == m_PEDs.end()) ? 0 : *it;
}

PS::PGDIt CPSLibrary::FindPGDIt(LPCSTR Name)
{
    if (!Name)
        return m_PGDs.end();

    PS::PGDIt I = std::lower_bound(m_PGDs.begin(), m_PGDs.end(), Name, pgd_find_pred);
    if (I == m_PGDs.end() || (0 != xr_strcmp((*I)->m_Name, Name)))
        return m_PGDs.end();
    else
        return I;
}

PS::CPGDef* CPSLibrary::FindPGD(LPCSTR Name)
{
    PS::PGDIt it = FindPGDIt(Name);
    return (it == m_PGDs.end()) ? 0 : *it;
}

void CPSLibrary::RenamePED(PS::CPEDef* src, LPCSTR new_name)
{
    R_ASSERT(src && new_name && new_name[0]);
    src->SetName(new_name);
}

void CPSLibrary::RenamePGD(PS::CPGDef* src, LPCSTR new_name)
{
    R_ASSERT(src && new_name && new_name[0]);
    src->SetName(new_name);
}

void CPSLibrary::Remove(LPCSTR nm)
{
    PS::PEDIt it = FindPEDIt(nm);
    if (it != m_PEDs.end())
    {
        (*it)->DestroyShader();
        xr_delete(*it);
        m_PEDs.erase(it);
    }
    else
    {
        PS::PGDIt it = FindPGDIt(nm);
        if (it != m_PGDs.end())
        {
            xr_delete(*it);
            m_PGDs.erase(it);
        }
    }
}
//----------------------------------------------------
bool CPSLibrary::Load(LPCSTR nm)
{
    IReader* F = FS.r_open(nm);

    if (F->length() == 0)
        return true;

    Msg("Load [%s]", nm);

    R_ASSERT(F->find_chunk(PS_CHUNK_VERSION));
    u16 ver = F->r_u16();
    if (ver != PS_VERSION)
        return false;

    bool bRes = true;

    const bool copFileFormat = strstr(nm, "_cop");

    if (copFileFormat)
        Msg("cop format used for file [%s]", nm);

    // second generation
    IReader* OBJ;
    OBJ = F->open_chunk(PS_CHUNK_SECONDGEN);
    if (OBJ)
    {
        IReader* O = OBJ->open_chunk(0);
        for (int count = 1; O; count++)
        {
            PS::CPEDef* def = xr_new<PS::CPEDef>();
            if (def->Load(*O))
            {
                def->m_copFormat = copFileFormat;

                //PS::PEDIt it = FindPEDIt(def->Name());
                //if (it != m_PEDs.end())
                //{
                //    Msg("CPSLibrary: Duplicate ParticleEffect: %s. Last declared will be used.", def->Name());
                //    xr_delete(*it);
                //    m_PEDs.erase(it);
                //}

                bool found = false;

                for (PS::PEDIt it = m_PEDs.begin(); it != m_PEDs.end(); it++)
                {
                    if (0 == xr_strcmp((*it)->Name(), def->Name()))
                    {
                        found = true;
                        break;
                    }
                }
                
                if (found)
                {
                    xr_delete(def);
                }
                else
                {
                    m_PEDs.push_back(def);
                }
            }
            else
            {
                bRes = false;
                xr_delete(def);
            }
            O->close();
            if (!bRes)
                break;
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    // second generation
    OBJ = F->open_chunk(PS_CHUNK_THIRDGEN);
    if (OBJ)
    {
        IReader* O = OBJ->open_chunk(0);
        for (int count = 1; O; count++)
        {
            PS::CPGDef* def = xr_new<PS::CPGDef>();
            if (def->Load(*O))
            {
                //PS::PGDIt it = FindPGDIt(def->m_Name.c_str());
                //if (it != m_PGDs.end())
                //{
                //    Msg("CPSLibrary: Duplicate ParticleGroup: %s. Last declared will be used.", def->m_Name.c_str());
                //    xr_delete(*it);
                //    m_PGDs.erase(it);
                //}

                bool found = false;

                for (PS::PGDIt it = m_PGDs.begin(); it != m_PGDs.end(); it++)
                {
                    if (0 == xr_strcmp((*it)->m_Name, def->m_Name))
                    {
                        found = true;
                        break;
                    }
                }
                
                if (found)
                {
                    xr_delete(def);
                }
                else
                {
                    m_PGDs.push_back(def);
                }
            }
            else
            {
                bRes = false;
                xr_delete(def);
            }
            O->close();
            if (!bRes)
                break;
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }

    // final
    FS.r_close(F);

    return bRes;
}

bool CPSLibrary::Save(const char* nm)
{
    CMemoryWriter F;

    F.open_chunk(PS_CHUNK_VERSION);
    F.w_u16(PS_VERSION);
    F.close_chunk();

    F.open_chunk(PS_CHUNK_SECONDGEN);
    u32 chunk_id = 0;
    for (PS::PEDIt it = m_PEDs.begin(); it != m_PEDs.end(); ++it, ++chunk_id)
    {
        F.open_chunk(chunk_id);
        (*it)->Save(F);
        F.close_chunk();
    }
    F.close_chunk();

    F.open_chunk(PS_CHUNK_THIRDGEN);
    chunk_id = 0;
    for (PS::PGDIt g_it = m_PGDs.begin(); g_it != m_PGDs.end(); ++g_it, ++chunk_id)
    {
        F.open_chunk(chunk_id);
        (*g_it)->Save(F);
        F.close_chunk();
    }
    F.close_chunk();

    return F.save_to(nm);
}


//----------------------------------------------------
void CPSLibrary::Reload()
{
    OnDestroy();
    OnCreate();

    Msg("PS Library was successfully reloaded.");
}
//----------------------------------------------------

using PS::CPGDef;

CPGDef const* const* CPSLibrary::particles_group_begin() const { return m_PGDs.size() ? &m_PGDs.front() : nullptr; }

CPGDef const* const* CPSLibrary::particles_group_end() const { return m_PGDs.size() ? &m_PGDs.back() : nullptr; }

shared_str const& CPSLibrary::particles_group_id(CPGDef const& particles_group) const { return (particles_group.m_Name); }

bool CPSLibrary::Load2()
{
    if (!FS.path_exist("$game_particles$"))
    {
        return true;
    }

    Msg("Start load particle files...");

    FS_FileSet files;
    string_path _path;

    FS.update_path(_path, "$game_particles$", "");
    FS.file_list(files, _path, FS_ListFiles, "*.pe,*.pg");

    FS_FileSet::iterator it = files.begin();
    FS_FileSet::iterator it_e = files.end();

    string_path p_path, p_name, p_ext;
    for (; it != it_e; ++it)
    {
        const FS_File& f = (*it);
        _splitpath(f.name.c_str(), 0, p_path, p_name, p_ext);
        FS.update_path(_path, "$game_particles$", f.name.c_str());
        CInifile ini(_path, TRUE, TRUE, FALSE);

        xr_sprintf(_path, sizeof(_path), "%s%s", p_path, p_name);
        if (0 == stricmp(p_ext, ".pe"))
        {
            PS::CPEDef* def = xr_new<PS::CPEDef>();
            def->m_copFormat = true; // always in cop mode 
            def->m_Name = _path;
            if (def->Load2(ini))
                m_PEDs.push_back(def);
            else
                xr_delete(def);
        }
        else if (0 == stricmp(p_ext, ".pg"))
        {
            PS::CPGDef* def = xr_new<PS::CPGDef>();
            def->m_Name = _path;
            if (def->Load2(ini))
                m_PGDs.push_back(def);
            else
                xr_delete(def);
        }
        else
        {
            R_ASSERT(0);
        }
    }    

    Msg("Loaded particle files: %d", files.size());

    return true;
}

bool CPSLibrary::Save2(bool override)
{
    if (!FS.path_exist("$game_particles$"))
    {
        Msg("! Path $game_particles$ is not configured! Cannot export particles to ltx");
    }

    // FS.dir_delete("$game_particles$", "", TRUE); ???

    string_path fn;

    for (auto pe : m_PEDs)
    {
        FS.update_path(fn, "$game_particles$", pe->m_Name.c_str());
        strcat(fn, ".pe");

        if (FS.exist(fn) && !override)
            continue;

        CInifile ini(fn, FALSE, TRUE, TRUE);
        pe->Save2(ini);
    }

    for (auto pg : m_PGDs)
    {
        FS.update_path(fn, "$game_particles$", pg->m_Name.c_str());
        strcat(fn, ".pg");

        if (FS.exist(fn) && !override)
            continue;

        CInifile ini(fn, FALSE, TRUE, TRUE);
        pg->Save2(ini);
    }

    return true;
}