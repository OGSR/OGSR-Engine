#include "stdafx.h"

#include "PSLibrary.h"
#include "ParticleEffect.h"
#include "ParticleGroup.h"

#define _game_data_ "$game_data$"

void CPSLibrary::OnCreate() { LoadAll(); }

void CPSLibrary::LoadAll()
{
    if (!Load2()) // load ltx pg and pe
    {
        string_path fn;
        if (FS.exist(fn, _game_data_, "particles.xr"))
        {
            Msg("Load [%s]", fn);
            Load(fn);
        }

        if (FS.exist(fn, _game_data_, "particles_cop.xr"))
        {
            Msg("Load [%s]", fn);
            Load(fn);
        }
    }

    for (auto& pair : m_PEDs)
        pair.second->CreateShader();
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
    for (auto& pair : m_PEDs)
        pair.second->DestroyShader();

    m_PEDs.clear();
    m_PGDs.clear();
}

PS::CPEDef* CPSLibrary::FindPED(const char* Name)
{
    //auto it = Name ? m_PEDs.find(Name) : m_PEDs.end(); //Сомневаюсь что сюда может попасть nullptr
    auto it = m_PEDs.find(Name);
    return it == m_PEDs.end() ? nullptr : it->second.get();
}

PS::CPGDef* CPSLibrary::FindPGD(const char* Name)
{
    //auto it = Name ? m_PGDs.find(Name) : m_PGDs.end(); //Сомневаюсь что сюда может попасть nullptr
    auto it = m_PGDs.find(Name);
    return it == m_PGDs.end() ? nullptr : it->second.get();
}

bool CPSLibrary::Load(const char* nm)
{
    IReader* F = FS.r_open(nm);

    if (F->length() == 0)
        return true;

    R_ASSERT(F->find_chunk(PS_CHUNK_VERSION));
    u16 ver = F->r_u16();
    if (ver != PS_VERSION)
        return false;

    bool bRes = true;
    size_t loaded_count{};
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
            auto def = std::make_unique<PS::CPEDef>();
            if (def->Load(*O))
            {
                def->m_copFormat = copFileFormat;

                // if (m_PEDs.contains(def->m_Name))
                //     Msg("~~CPSLibrary: Duplicate ParticleEffect: [%s]. Last declared will be used.", def->m_Name.c_str());

                m_PEDs[def->m_Name] = std::move(def);
                loaded_count++;
            }
            else
            {
                bRes = false;
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
            auto def = std::make_unique<PS::CPGDef>();
            if (def->Load(*O))
            {
                // if (m_PGDs.contains(def->m_Name))
                //    Msg("CPSLibrary: Duplicate ParticleGroup: [%s]. Last declared will be used.", def->m_Name.c_str());

                m_PGDs[def->m_Name] = std::move(def);
                loaded_count++;
            }
            else
            {
                bRes = false;
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

    Msg("Loaded xr_particle files: [%u]", loaded_count);

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
    for (auto it = m_PEDs.begin(); it != m_PEDs.end(); ++it, ++chunk_id)
    {
        F.open_chunk(chunk_id);
        it->second->Save(F);
        F.close_chunk();
    }
    F.close_chunk();

    F.open_chunk(PS_CHUNK_THIRDGEN);
    chunk_id = 0;
    for (auto g_it = m_PGDs.begin(); g_it != m_PGDs.end(); ++g_it, ++chunk_id)
    {
        F.open_chunk(chunk_id);
        g_it->second->Save(F);
        F.close_chunk();
    }
    F.close_chunk();

    return F.save_to(nm);
}

void CPSLibrary::Reload()
{
    OnDestroy();
    OnCreate();

    Msg("PS Library was successfully reloaded.");
}

bool CPSLibrary::Load2()
{
    if (!FS.path_exist("$game_particles$"))
    {
        return false;
    }

    bool something_loaded{};
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
        something_loaded = true;
        if (0 == stricmp(p_ext, ".pe"))
        {
            auto def = std::make_unique<PS::CPEDef>();
            def->m_copFormat = true; // always in cop mode
            def->m_Name = _path;
            if (def->Load2(ini))
                m_PEDs[def->m_Name] = std::move(def);
        }
        else if (0 == stricmp(p_ext, ".pg"))
        {
            auto def = std::make_unique<PS::CPGDef>();
            def->m_Name = _path;
            if (def->Load2(ini))
                m_PGDs[def->m_Name] = std::move(def);
        }
        else
        {
            R_ASSERT(0);
        }
    }    

    Msg("Loaded particle files: [%u]", files.size());

    return something_loaded;
}

bool CPSLibrary::Save2(bool override)
{
    if (!FS.path_exist("$game_particles$"))
    {
        Msg("! Path $game_particles$ is not configured! Cannot export particles to ltx");
    }

    // FS.dir_delete("$game_particles$", "", TRUE); ???

    string_path fn;

    for (auto& [m_Name, pe] : m_PEDs)
    {
        FS.update_path(fn, "$game_particles$", m_Name.c_str());
        strcat(fn, ".pe");

        if (FS.exist(fn) && !override)
            continue;

        CInifile ini(fn, FALSE, TRUE, TRUE);
        pe->Save2(ini);
    }

    for (auto& [m_Name, pg] : m_PGDs)
    {
        FS.update_path(fn, "$game_particles$", m_Name.c_str());
        strcat(fn, ".pg");

        if (FS.exist(fn) && !override)
            continue;

        CInifile ini(fn, FALSE, TRUE, TRUE);
        pg->Save2(ini);
    }

    return true;
}
