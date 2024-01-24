#include "stdafx.h"

#include "ResourceManager.h"
#include "blenders\blender.h"

BOOL bShadersXrExport{};

void CResourceManager::OnDeviceDestroy(BOOL)
{
    if (RDEVICE.b_is_Ready)
        return;

    m_textures_description.UnLoad();

    // Release blenders
    for (map_BlenderIt b = m_blenders.begin(); b != m_blenders.end(); b++)
    {
        xr_free((char*&)b->first);
        IBlenderXr::Destroy(b->second);
    }
    m_blenders.clear();

    // scripting
    LS_Unload();
}

void CResourceManager::OnDeviceCreate()
{
    if (!RDEVICE.b_is_Ready)
        return;

    // scripting
    LS_Load();

    FS_FileSet flist;
    FS.file_list(flist, _game_data_, FS_ListFiles | FS_RootOnly, "*shaders*.xr");

    if (!flist.empty()) // legacy mode with convert
    {
        Msg("[%s] count of *shaders*.xr files: [%u]", __FUNCTION__, flist.size());

        for (const auto& file : flist)
        {
            string_path fname;
            FS.update_path(fname, _game_data_, file.name.c_str());

            // Msg("Loading shader file: [%s]", fname);

            LoadShaderFile(fname);
        }
    }
    else
    {
        FS_FileSet flist2;
        FS.file_list(flist2, _game_data_, FS_ListFiles | FS_RootOnly, "*shaders*.ltx");

        Msg("[%s] count of *shaders*.ltx files: [%u]", __FUNCTION__, flist2.size());

        for (const auto& file : flist2)
        {
            string_path fname;
            FS.update_path(fname, _game_data_, file.name.c_str());

            // Msg("Loading shader file: [%s]", fname);

            LoadShaderLtxFile(fname);
        }
    }

    m_textures_description.Load();
}

void CResourceManager::LoadShaderFile(LPCSTR fname)
{
    // Check if file is compressed already
    string32 ID = "shENGINE";

    string32 id;
    IReader* F = FS.r_open(fname);
    R_ASSERT2(F, fname);
    F->r(&id, 8);

    if (0 == strncmp(id, ID, 8))
    {
        FATAL("Unsupported blender library. Compressed?");
    }    

    string_path ini_path;
    strcpy_s(ini_path, fname);
    if (strext(ini_path))
        *strext(ini_path) = 0;
    strcat_s(ini_path, ".ltx");

    CInifile ini(ini_path, 0, 1, 1);

    // Load blenders
    if (IReader* fs = F->open_chunk(2))
    {
        IReader* chunk;
        int chunk_id = 0;

        while ((chunk = fs->open_chunk(chunk_id)) != nullptr)
        {
            CBlender_DESC desc;
            chunk->r(&desc, sizeof(desc));

            IBlenderXr* B = IBlenderXr::Create(desc.CLS);
            if (!B)
            {
                Msg("! Renderer doesn't support blender '%s'", desc.cName);
            }
            else
            {
                if (B->getDescription().version < desc.version)
                {
                    Msg("! Version conflict in shader '%s'", desc.cName);
                }

                chunk->seek(0);

                B->Load(*chunk, desc.version);

                // для конвертации в ltx
                if (bShadersXrExport && !ini.section_exist(desc.cName))
                {
                    B->SaveIni(&ini, desc.cName);
                }

                //Msg("Loading shader: [%s]", desc.cName);

                std::pair<map_BlenderIt, bool> I = m_blenders.insert_or_assign(xr_strdup(desc.cName), B);
                ASSERT_FMT(I.second, "CResourceManager::LoadSharedFile - found shader name [%s]", desc.cName);
            }

            chunk->close();
            chunk_id += 1;
        }
        fs->close();
    }

    FS.r_close(F);
}

void CResourceManager::LoadShaderLtxFile(LPCSTR fname)
{
    string_path ini_path;
    strcpy_s(ini_path, fname);

    CInifile ini(ini_path, 0, 1, 0);

    for (const auto& it : ini.sections_ordered())
    {
        auto& name = it.first;

        const CLASS_ID cls = ini.r_clsid(name, "class");
        const u16 version = ini.r_u16(name, "version");

        IBlenderXr* B = IBlenderXr::Create(cls);
        if (!B)
        {
            Msg("! Renderer doesn't support blender '%s'", name.c_str());
        }
        else
        {
            if (B->getDescription().version < version)
            {
                Msg("! Version conflict in shader '%s'", name.c_str());
            }

            B->LoadIni(&ini, name.c_str());

            // Msg("Loading shader: [%s]", desc.cName);

            std::pair<map_BlenderIt, bool> I = m_blenders.insert_or_assign(xr_strdup(name.c_str()), B);
            ASSERT_FMT(I.second, "CResourceManager::LoadSharedFile - found shader name [%s]", name.c_str());
        }
    }
}
