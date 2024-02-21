// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable : 4995)
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default : 4995)

#include "FS_internal.h"
#include "stream_reader.h"
#include "file_stream_reader.h"
#include "trivial_encryptor.h"

#include <filesystem>

constexpr u32 BIG_FILE_READER_WINDOW_SIZE = 1024 * 1024;

std::unique_ptr<CLocatorAPI> xr_FS;

struct _open_file
{
    union
    {
        IReader* _reader;
        CStreamReader* _stream_reader;
    };
    shared_str _fn;
    u32 _used;
};

template <typename T>
struct eq_pointer;

template <>
struct eq_pointer<IReader>
{
    IReader* _val;
    eq_pointer(IReader* p) : _val(p) {}
    bool operator()(_open_file& itm) { return (_val == itm._reader); }
};
template <>
struct eq_pointer<CStreamReader>
{
    CStreamReader* _val;
    eq_pointer(CStreamReader* p) : _val(p) {}
    bool operator()(_open_file& itm) { return (_val == itm._stream_reader); }
};
struct eq_fname_free
{
    shared_str _val;
    eq_fname_free(shared_str s) { _val = s; }
    bool operator()(_open_file& itm) { return (_val == itm._fn && itm._reader == NULL); }
};
struct eq_fname_check
{
    shared_str _val;
    eq_fname_check(shared_str s) { _val = s; }
    bool operator()(_open_file& itm) { return (_val == itm._fn && itm._reader != NULL); }
};

XRCORE_API xr_vector<_open_file> g_open_files;

void _check_open_file(const shared_str& _fname)
{
    xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_check(_fname));
    if (it != g_open_files.end())
        Msg("file opened at least twice: [%s]", _fname.c_str());
}

_open_file& find_free_item(const shared_str& _fname)
{
    xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_free(_fname));
    if (it == g_open_files.end())
    {
        g_open_files.resize(g_open_files.size() + 1);
        _open_file& _of = g_open_files.back();
        _of._fn = _fname;
        _of._used = 0;
        return _of;
    }
    return *it;
}

void setup_reader(CStreamReader* _r, _open_file& _of) { _of._stream_reader = _r; }

void setup_reader(IReader* _r, _open_file& _of) { _of._reader = _r; }

template <typename T>
void _register_open_file(T* _r, LPCSTR _fname)
{
    xrCriticalSection _lock;
    _lock.Enter();

    shared_str f = _fname;
    _check_open_file(f);

    _open_file& _of = find_free_item(_fname);
    setup_reader(_r, _of);
    _of._used += 1;

    _lock.Leave();
}

template <typename T>
void _unregister_open_file(T* _r)
{
    xrCriticalSection _lock;
    _lock.Enter();

    xr_vector<_open_file>::iterator it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_pointer<T>(_r));
    VERIFY(it != g_open_files.end());
    _open_file& _of = *it;
    _of._reader = NULL;
    _lock.Leave();
}

XRCORE_API void _dump_open_files(int mode)
{
    xr_vector<_open_file>::iterator it = g_open_files.begin();
    xr_vector<_open_file>::iterator it_e = g_open_files.end();

    bool bShow = false;
    if (mode == 1)
    {
        for (; it != it_e; ++it)
        {
            _open_file& _of = *it;
            if (_of._reader != NULL)
            {
                if (!bShow)
                    Log("----opened files");

                bShow = true;
                Msg("[%d] fname:%s", _of._used, _of._fn.c_str());
            }
        }
    }
    else
    {
        Log("----un-used");
        for (it = g_open_files.begin(); it != it_e; ++it)
        {
            _open_file& _of = *it;
            if (_of._reader == NULL)
                Msg("[%d] fname:%s", _of._used, _of._fn.c_str());
        }
    }
    if (bShow)
        Msg("----total count = [%u]", g_open_files.size());
}

CLocatorAPI::CLocatorAPI()
#ifdef PROFILE_CRITICAL_SECTIONS
    : m_auth_lock(MUTEX_PROFILE_ID(CLocatorAPI::m_auth_lock))
#endif // PROFILE_CRITICAL_SECTIONS
{
    m_Flags.zero();
    // get page size
    SYSTEM_INFO sys_inf;
    GetSystemInfo(&sys_inf);
    dwAllocGranularity = sys_inf.dwAllocationGranularity;
    m_iLockRescan = 0;
    dwOpenCounter = 0;
}

CLocatorAPI::~CLocatorAPI()
{
    VERIFY(0 == m_iLockRescan);
    _dump_open_files(1);
}

void CLocatorAPI::Register(LPCSTR name, u32 vfs, u32 crc, u32 ptr, u32 size_real, u32 size_compressed, u32 modif)
{
    string256 temp_file_name;
    strcpy_s(temp_file_name, name);
    xr_strlwr(temp_file_name);

    // Register file
    file desc;
    //	desc.name			= xr_strlwr(xr_strdup(name));
    desc.name = temp_file_name;
    desc.vfs = vfs;
    desc.crc = crc;
    desc.ptr = ptr;
    desc.size_real = size_real;
    desc.size_compressed = size_compressed;
    desc.modif = modif & (~u32(0x3));
    //	Msg("registering file %s - %d", name, size_real);
    //	if file already exist - update info
    files_it I = files.find(desc);
    if (I != files.end())
    {
        desc.name = I->name;

        // sad but true, performance option
        // correct way is to erase and then insert new record:
        const_cast<file&>(*I) = desc;
        return;
    }
    else
    {
        desc.name = xr_strdup(desc.name);
    }

    // otherwise insert file
    files.insert(desc);

    // Try to register folder(s)
    string_path temp;
    strcpy_s(temp, desc.name);
    string_path path;
    string_path folder;
    while (temp[0])
    {
        _splitpath(temp, path, folder, 0, 0);
        if (!folder[0])
            break;
        strcat_s(path, folder);
        if (!exist(path))
        {
            desc.name = xr_strdup(path);
            desc.vfs = 0xffffffff;
            desc.ptr = 0;
            desc.size_real = 0;
            desc.size_compressed = 0;
            desc.modif = u32(-1);
            desc.folder = true;

            std::pair<files_it, bool> I = files.insert(desc);

            R_ASSERT(I.second);
        }
        strcpy_s(temp, path); // strcpy_s(temp, folder);
        if (xr_strlen(temp))
            temp[xr_strlen(temp) - 1] = 0;
    }
}

static IReader* open_chunk(void* ptr, const u32 ID, const char* archiveName, const u32 archiveSize, const bool shouldDecrypt)
{
    BOOL res;
    u32 dwType, dwSize;
    DWORD read_byte;
    DWORD dwPtr = SetFilePointer(ptr, 0, 0, FILE_BEGIN);
    R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
    while (true)
    {
        res = ReadFile(ptr, &dwType, 4, &read_byte, 0);
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));
        res = ReadFile(ptr, &dwSize, 4, &read_byte, 0);
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));
        if ((dwType & (~CFS_CompressMark)) == ID)
        {
            u8* src_data = xr_alloc<u8>(dwSize);
            res = ReadFile(ptr, src_data, dwSize, &read_byte, 0);
            R_ASSERT3(res && read_byte == dwSize, archiveName, Debug.error2string(GetLastError()));
            if (dwType & CFS_CompressMark)
            {
                BYTE* dest{};
                unsigned dest_sz{};

                if (shouldDecrypt)
                    g_trivial_encryptor.decode(src_data, dwSize, src_data); // Try WW key first

                bool result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);

                if (!result && shouldDecrypt) // Let's try to decode with Rus key
                {
                    MsgDbg("[%s]: decoding of %s with WW key failed, trying RU key...", __FUNCTION__, archiveName);
                    g_trivial_encryptor.encode(src_data, dwSize, src_data); // rollback
                    g_trivial_encryptor.decode(src_data, dwSize, src_data, trivial_encryptor::key_flag::russian);
                    result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);
                }

                CHECK_OR_EXIT(result, make_string("[%s] Can't decompress archive [%s]", __FUNCTION__, archiveName));

                xr_free(src_data);
                return xr_new<CTempReader>(dest, dest_sz, 0);
            }
            else
            {
                return xr_new<CTempReader>(src_data, dwSize, 0);
            }
        }
        else
        {
            dwPtr = SetFilePointer(ptr, dwSize, 0, FILE_CURRENT);
            R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
        }
    }
    return 0;
};

void CLocatorAPI::ProcessArchive(LPCSTR _path, LPCSTR base_path)
{
    // find existing archive
    shared_str path = _path;

    for (archives_it it = archives.begin(); it != archives.end(); ++it)
        if (it->path == path)
            return;

    const bool shouldDecrypt = !strstr(_path, ".xdb");

    // open archive
    auto& A = archives.emplace_back();
    A.path = path;
    // Open the file
    A.hSrcFile = CreateFile(*path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    R_ASSERT(A.hSrcFile != INVALID_HANDLE_VALUE);
    A.hSrcMap = CreateFileMapping(A.hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
    R_ASSERT(A.hSrcMap != INVALID_HANDLE_VALUE);
    A.size = GetFileSize(A.hSrcFile, 0);
    R_ASSERT(A.size > 0);

    // Create base path
    string_path base;
    if (!base_path)
    {
        strcpy_s(base, *path);
        if (strext(base))
            *strext(base) = 0;
    }
    else
    {
        strcpy_s(base, base_path);
    }
    strcat_s(base, "\\");

    // Read headers
    IReader* hdr = open_chunk(A.hSrcFile, 1, A.path.c_str(), A.size, shouldDecrypt);
    R_ASSERT(hdr);
    RStringVec fv;
    while (!hdr->eof())
    {
        string_path name, full;

        string1024 buffer_start;
        u16 buffer_size = hdr->r_u16();
        VERIFY(buffer_size < sizeof(name) + 4 * sizeof(u32));
        VERIFY(buffer_size < sizeof(buffer_start));
        u8* buffer = (u8*)&*buffer_start;
        hdr->r(buffer, buffer_size);

        u32 size_real = *(u32*)buffer;
        buffer += sizeof(size_real);

        u32 size_compr = *(u32*)buffer;
        buffer += sizeof(size_compr);

        u32 crc = *(u32*)buffer;
        buffer += sizeof(crc);

        u32 name_length = buffer_size - 4 * sizeof(u32);
        Memory.mem_copy(name, buffer, name_length);
        name[name_length] = 0;
        buffer += buffer_size - 4 * sizeof(u32);

        u32 ptr = *(u32*)buffer;
        buffer += sizeof(ptr);

        strconcat(sizeof(full), full, base, name);
        size_t vfs = archives.size() - 1;

        Register(full, (u32)vfs, crc, ptr, size_real, size_compr, 0);
    }
    hdr->close();
}

void CLocatorAPI::ProcessOne(LPCSTR path, const _finddata_t& F, bool bNoRecurse)
{
    string_path N;
    strcpy_s(N, path);
    strcat_s(N, F.name);
    xr_strlwr(N);

    if (F.attrib & _A_HIDDEN)
    {
        return;
    }

    if (F.attrib & _A_SUBDIR)
    {
        if (bNoRecurse)
            return;

        if (0 == xr_strcmp(F.name, "."))
            return;
        if (0 == xr_strcmp(F.name, ".."))
            return;

        strcat_s(N, "\\");

        RecurseScanPhysicalPath(N, false, bNoRecurse);
    }
    else
    {
        if (!m_Flags.is(flTargetFolderOnly) && strext(N) && 0 == strncmp(strext(N), ".db", 3))
        {
            Msg("--Found base arch: [%s], size: [%u]", N, F.size);
            ProcessArchive(N);
        }
        else
        {
            Register(N, 0xffffffff, 0, 0, F.size, F.size, (u32)F.time_write);
        }
    }
}

IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y) { return xr_strcmp(x.name, y.name) < 0; }

bool ignore_name(const char* _name)
{
    // ignore windows hidden Thumbs.db
    if (!strcmp(_name, "Thumbs.db"))
        return true;

    // ignore processing ".svn" folders
    if (!strcmp(_name, ".svn"))
        return true;
    
    return false;
}

// we need to check for file existance
// because Unicode file names can
// be interpolated by FindNextFile()

bool ignore_path(const char* _path)
{
    return !std::filesystem::exists(_path);
}

bool CLocatorAPI::RecurseScanPhysicalPath(const char* path, const bool log_if_found, bool bNoRecurse)
{
    _finddata_t sFile;
    intptr_t hFile;

    string_path N;
    strcpy_s(N, path);
    strcat_s(N, "*.*");

    using FFVec = xr_vector<_finddata_t>;
    FFVec rec_files;
    rec_files.reserve(1224);

    // find all files
    if (-1 == (hFile = _findfirst(N, &sFile)))
    {
        // Log		("! Wrong path: ",path);
        return false;
    }

    if (log_if_found)
        Msg("--Found FS dir: [%s]", path);

    string1024 full_path;
    if (m_Flags.test(flNeedCheck))
    {
        strcpy_s(full_path, path);
        strcat_s(full_path, sFile.name);

        // загоняем в вектор для того *.db* приходили в сортированном порядке
        if (!ignore_name(sFile.name) && !ignore_path(full_path))
            rec_files.push_back(sFile);

        while (_findnext(hFile, &sFile) == 0)
        {
            strcpy_s(full_path, path);
            strcat_s(full_path, sFile.name);
            if (!ignore_name(sFile.name) && !ignore_path(full_path))
                rec_files.push_back(sFile);
        }
    }
    else
    {
        // загоняем в вектор для того *.db* приходили в сортированном порядке
        if (!ignore_name(sFile.name))
            rec_files.push_back(sFile);

        while (_findnext(hFile, &sFile) == 0)
        {
            if (!ignore_name(sFile.name))
                rec_files.push_back(sFile);
        }
    }

    _findclose(hFile);

    if (log_if_found)
        Msg("  files: [%u]", rec_files.size());

    std::sort(rec_files.begin(), rec_files.end(), pred_str_ff);

    for (const auto& el : rec_files)
    {
        ProcessOne(path, el, bNoRecurse);
    }

    // insert self
    if (path && path[0])
    {
        Register(path, 0xffffffff, 0, 0, 0, 0, 0);
    }

    return true;
}

void CLocatorAPI::_initialize(u32 flags, LPCSTR target_folder, LPCSTR fs_name)
{
    char _delimiter = '|'; //','
    if (m_Flags.is(flReady))
        return;
    CTimer t;
    t.Start();
    Log("Initializing File System...");
    u32 M1 = Memory.mem_usage();

    m_Flags.set(flags, TRUE);

    // scan root directory
    string4096 buf;
    IReader* pFSltx = 0;

    // append working folder
    LPCSTR fs_ltx = NULL;

    // append application path
    if (m_Flags.is(flScanAppRoot))
    {
        append_path("$app_root$", Core.ApplicationPath, 0, FALSE);
    }

    if (m_Flags.is(flTargetFolderOnly))
    {
        append_path("$fs_root$", "", 0, FALSE);
    }
    else
    {
        // find nearest fs.ltx and set fs_root correctly
        fs_ltx = (fs_name && fs_name[0]) ? fs_name : FSLTX;
        pFSltx = r_open(fs_ltx);

        if (!pFSltx && m_Flags.is(flScanAppRoot))
            pFSltx = r_open("$app_root$", fs_ltx);

        if (!pFSltx)
        {
            if (strstr(Core.Params, "-use-work-dir"))
            {
                string_path currentDir;
                GetCurrentDirectory(std::size(currentDir) - 1, currentDir);
                currentDir[std::size(currentDir) - 1] = '\0';
                append_path("$fs_root$", currentDir, 0, FALSE);
            }
            else
            {
                string_path tmpAppPath{};
                strcpy_s(tmpAppPath, Core.ApplicationPath);
                if (xr_strlen(tmpAppPath))
                {
                    tmpAppPath[xr_strlen(tmpAppPath) - 1] = 0;
                    if (strrchr(tmpAppPath, '\\'))
                        *(strrchr(tmpAppPath, '\\') + 1) = 0;

                    append_path("$fs_root$", tmpAppPath, 0, FALSE);
                }
                else
                    append_path("$fs_root$", "", 0, FALSE);
            }

            pFSltx = r_open("$fs_root$", fs_ltx);
        }
        else
            append_path("$fs_root$", "", 0, FALSE);

        Msg("using fs-ltx: [%s]", fs_ltx);
    }

    //-----------------------------------------------------------
    // append application data path
    // target folder
    if (m_Flags.is(flTargetFolderOnly))
    {
        append_path("$target_folder$", target_folder, 0, TRUE);
    }
    else
    {
        CHECK_OR_EXIT(pFSltx, make_string("Cannot open file \"%s\".\nCheck your working folder.", fs_ltx));
        // append all pathes
        string_path id, root, add, def, capt;
        LPCSTR lp_add, lp_def, lp_capt;
        string16 b_v;
        string4096 temp;

        while (!pFSltx->eof())
        {
            pFSltx->r_string(buf, sizeof(buf));
            if (buf[0] == ';')
                continue;

            _GetItem(buf, 0, id, '=');

            if (!m_Flags.is(flBuildCopy) && (0 == xr_strcmp(id, "$build_copy$")))
                continue;

            _GetItem(buf, 1, temp, '=');
            int cnt = _GetItemCount(temp, _delimiter);
            R_ASSERT2(cnt >= 3, temp);
            u32 fl = 0;
            _GetItem(temp, 0, b_v, _delimiter);

            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flRecurse;

            _GetItem(temp, 1, b_v, _delimiter);
            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flNotif;

            _GetItem(temp, 2, root, _delimiter);
            _GetItem(temp, 3, add, _delimiter);
            _GetItem(temp, 4, def, _delimiter);
            _GetItem(temp, 5, capt, _delimiter);
            xr_strlwr(id);

            xr_strlwr(root);
            lp_add = (cnt >= 4) ? xr_strlwr(add) : 0;
            lp_def = (cnt >= 5) ? def : 0;
            lp_capt = (cnt >= 6) ? capt : 0;

            PathPairIt p_it = pathes.find(root);

            std::pair<PathPairIt, bool> I;
            FS_Path* P = xr_new<FS_Path>((p_it != pathes.end()) ? p_it->second->m_Path : root, lp_add, lp_def, lp_capt, fl);
            bool bNoRecurse = !(fl & FS_Path::flRecurse);
#ifdef RESTRICT_GAMEDATA
            if (!strcmp(id, "$app_data_root$") || !strcmp(id, "$game_saves$") || !strcmp(id, "$logs$") || !strcmp(id, "$screenshots$"))
                RecurseScanPhysicalPath(P->m_Path);
#else
            RecurseScanPhysicalPath(P->m_Path, true, bNoRecurse);
#endif
            I = pathes.emplace(xr_strdup(id), P);
#ifndef DEBUG
            m_Flags.set(flCacheFiles, FALSE);
#endif // DEBUG

            CHECK_OR_EXIT(I.second, "The file 'fsgame.ltx' is corrupted (it contains duplicated lines).\nPlease reinstall the game or fix the problem manually.");
        }
        r_close(pFSltx);
        R_ASSERT(path_exist("$app_data_root$"));
    };

#ifndef RESTRICT_GAMEDATA
    if (!m_Flags.is(flTargetFolderOnly))
        ProcessExternalArch();
#endif

    u32 M2 = Memory.mem_usage();
    Msg("FS: %d files cached, %dKb memory used.", files.size(), (M2 - M1) / 1024);

    m_Flags.set(flReady, TRUE);

    Msg("Init FileSystem %f sec", t.GetElapsed_sec());
    //-----------------------------------------------------------
    if (strstr(Core.Params, "-overlaypath"))
    {
        string1024 c_newAppPathRoot;
        sscanf(strstr(Core.Params, "-overlaypath ") + 13, "%[^ ] ", c_newAppPathRoot);
        FS_Path* pLogsPath = FS.get_path("$logs$");
        FS_Path* pAppdataPath = FS.get_path("$app_data_root$");

        if (pLogsPath)
            pLogsPath->_set_root(c_newAppPathRoot);
        if (pAppdataPath)
        {
            pAppdataPath->_set_root(c_newAppPathRoot);
            rescan_physical_path(pAppdataPath->m_Path, pAppdataPath->m_Flags.is(FS_Path::flRecurse));
        }
    }
    //-----------------------------------------------------------

    CreateLog(0 != strstr(Core.Params, "-nolog"));
}

void CLocatorAPI::_destroy()
{
    for (files_it I = files.begin(); I != files.end(); I++)
    {
        char* str = LPSTR(I->name);
        xr_free(str);
    }
    files.clear();
    for (PathPairIt p_it = pathes.begin(); p_it != pathes.end(); p_it++)
    {
        char* str = LPSTR(p_it->first);
        xr_free(str);
        xr_delete(p_it->second);
    }
    pathes.clear();
    for (archives_it a_it = archives.begin(); a_it != archives.end(); a_it++)
    {
        CloseHandle(a_it->hSrcMap);
        CloseHandle(a_it->hSrcFile);
    }
    archives.clear();
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* fn)
{
    files_it it = file_find_it(fn);
    return (it != files.end()) ? &(*it) : 0;
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* path, const char* name)
{
    string_path temp;
    update_path(temp, path, name);
    return exist(temp);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, LPCSTR path, LPCSTR name)
{
    update_path(fn, path, name);
    return exist(fn);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, LPCSTR path, LPCSTR name, LPCSTR ext)
{
    string_path nm;
    strconcat(sizeof(nm), nm, name, ext);
    update_path(fn, path, nm);
    return exist(fn);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* initial, const char* folder, u32 flags)
{
    string_path N;
    R_ASSERT(initial && initial[0]);
    update_path(N, initial, folder);
    return file_list_open(N, flags);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* _path, u32 flags)
{
    R_ASSERT(_path);
    VERIFY(flags);
    // проверить нужно ли пересканировать пути
    check_pathes();

    string_path N;

    if (path_exist(_path))
        update_path(N, _path, "");
    else
        strcpy_s(N, _path);

    file desc;
    desc.name = N;
    files_it I = files.find(desc);
    if (I == files.end())
        return 0;

    xr_vector<char*>* dest = xr_new<xr_vector<char*>>();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); I++)
    {
        const file& entry = *I;
        if (0 != strncmp(entry.name, N, base_len))
            break; // end of list
        const char* end_symbol = entry.name + xr_strlen(entry.name) - 1;
        if ((*end_symbol) != '\\')
        {
            // file
            if ((flags & FS_ListFiles) == 0)
                continue;

            const char* entry_begin = entry.name + base_len;
            if ((flags & FS_RootOnly) && strchr(entry_begin, '\\'))
                continue; // folder in folder
            dest->push_back(xr_strdup(entry_begin));
            LPSTR fname = dest->back();
            if (flags & FS_ClampExt)
                if (0 != strext(fname))
                    *strext(fname) = 0;
        }
        else
        {
            // folder
            if ((flags & FS_ListFolders) == 0)
                continue;
            const char* entry_begin = entry.name + base_len;

            if ((flags & FS_RootOnly) && (strchr(entry_begin, '\\') != end_symbol))
                continue; // folder in folder

            dest->push_back(xr_strdup(entry_begin));
        }
    }
    return dest;
}

void CLocatorAPI::file_list_close(xr_vector<char*>*& lst)
{
    if (lst)
    {
        for (xr_vector<char*>::iterator I = lst->begin(); I != lst->end(); I++)
            xr_free(*I);
        xr_delete(lst);
    }
}

int CLocatorAPI::file_list(FS_FileSet& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
    R_ASSERT(path);
    VERIFY(flags);
    // проверить нужно ли пересканировать пути
    check_pathes();

    string_path N;
    if (path_exist(path))
        update_path(N, path, "");
    else
        strcpy_s(N, path);

    file desc;
    desc.name = N;
    files_it I = files.find(desc);
    if (I == files.end())
        return 0;

    SStringVec masks;
    _SequenceToList(masks, mask);
    BOOL b_mask = !masks.empty();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); I++)
    {
        const file& entry = *I;
        if (0 != strncmp(entry.name, N, base_len))
            break; // end of list

        LPCSTR end_symbol = entry.name + xr_strlen(entry.name) - 1;
        if ((*end_symbol) != '\\')
        {
            // file
            if ((flags & FS_ListFiles) == 0)
                continue;

            LPCSTR entry_begin = entry.name + base_len;
            if ((flags & FS_RootOnly) && strchr(entry_begin, '\\'))
                continue; // folder in folder

            // check extension
            if (b_mask)
            {
                bool bOK = false;
                for (SStringVecIt it = masks.begin(); it != masks.end(); it++)
                {
                    if (PatternMatch(entry_begin, it->c_str()))
                    {
                        bOK = true;
                        break;
                    }
                }
                if (!bOK)
                    continue;
            }


            xr_string fn = entry_begin;

            // insert file entry
            if (flags & FS_ClampExt)
            {
                LPSTR src_ext = strext(entry_begin);
                if (src_ext)
                {
                    size_t ext_pos = src_ext - entry_begin;
                    fn.replace(ext_pos, strlen(src_ext), "");
                }
            }

            u32 fl = (entry.vfs != 0xffffffff ? FS_File::flVFS : 0);
            dest.emplace(fn, entry.size_real, entry.modif, fl, !(flags & FS_NoLower));
        }
        else
        {
            // folder
            if ((flags & FS_ListFolders) == 0)
                continue;
            LPCSTR entry_begin = entry.name + base_len;

            if ((flags & FS_RootOnly) && (strchr(entry_begin, '\\') != end_symbol))
                continue; // folder in folder
            u32 fl = FS_File::flSubDir | (entry.vfs ? FS_File::flVFS : 0);
            dest.emplace(entry_begin, entry.size_real, entry.modif, fl, !(flags & FS_NoLower));
        }
    }
    return int(dest.size());
}

void CLocatorAPI::file_from_cache_impl(IReader*& R, LPSTR fname, const file& desc) { R = xr_new<CVirtualFileReader>(fname); }

void CLocatorAPI::file_from_cache_impl(CStreamReader*& R, LPSTR fname, const file& desc)
{
    CFileStreamReader* r = xr_new<CFileStreamReader>();
    r->construct(fname, BIG_FILE_READER_WINDOW_SIZE);
    R = r;
}

template <typename T>
void CLocatorAPI::file_from_cache(T*& R, LPSTR fname, const file& desc, LPCSTR& source_name)
{
    file_from_cache_impl(R, fname, desc);
}

void CLocatorAPI::file_from_archive(IReader*& R, LPCSTR fname, const file& desc)
{
    // Archived one
    archive& A = archives[desc.vfs];
    u32 start = (desc.ptr / dwAllocGranularity) * dwAllocGranularity;
    u32 end = (desc.ptr + desc.size_compressed) / dwAllocGranularity;
    if ((desc.ptr + desc.size_compressed) % dwAllocGranularity)
        end += 1;
    end *= dwAllocGranularity;
    if (end > A.size)
        end = A.size;
    u32 sz = (end - start);
    u8* ptr = (u8*)MapViewOfFile(A.hSrcMap, FILE_MAP_READ, 0, start, sz);
    VERIFY3(ptr, "cannot create file mapping on file", fname);

    string512 temp;
    sprintf_s(temp, "%s:%s", *A.path, fname);
#ifdef DEBUG
    register_file_mapping(ptr, sz, temp);
#endif // DEBUG

    u32 ptr_offs = desc.ptr - start;
    if (desc.size_real == desc.size_compressed)
    {
        R = xr_new<CPackReader>(ptr, ptr + ptr_offs, desc.size_real);
        return;
    }

    // Compressed
    u8* dest = xr_alloc<u8>(desc.size_real);
    rtc_decompress(dest, desc.size_real, ptr + ptr_offs, desc.size_compressed);
    R = xr_new<CTempReader>(dest, desc.size_real, 0);
    UnmapViewOfFile(ptr);
#ifdef DEBUG
    unregister_file_mapping(ptr, sz);
#endif // DEBUG
}

void CLocatorAPI::file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc)
{
    archive& A = archives[desc.vfs];
    R_ASSERT2(desc.size_compressed == desc.size_real, make_string("cannot use stream reading for compressed data %s, do not compress data to be streamed", fname));

    R = xr_new<CStreamReader>();
    R->construct(A.hSrcMap, desc.ptr, desc.size_compressed, A.size, BIG_FILE_READER_WINDOW_SIZE);
}

bool CLocatorAPI::check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname, const file*& desc)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    // correct path
    strcpy_s(fname, _fname);
    xr_strlwr(fname);
    if (path && path[0])
        update_path(fname, path, fname);

    // Search entry
    file desc_f;
    desc_f.name = fname;

    files_it I = files.find(desc_f);
    if (I == files.end())
        return (false);

    ++dwOpenCounter;
    desc = &*I;
    return (true);
}

template <typename T>
T* CLocatorAPI::r_open_impl(LPCSTR path, LPCSTR _fname)
{
    T* R = 0;
    string_path fname;
    const file* desc = 0;
    LPCSTR source_name = &fname[0];

    if (!check_for_file(path, _fname, fname, desc))
        return (0);

    // OK, analyse
    if (0xffffffff == desc->vfs)
        file_from_cache(R, fname, *desc, source_name);
    else
        file_from_archive(R, fname, *desc);

    if (m_Flags.test(flDumpFileActivity))
        _register_open_file(R, fname);

    return (R);
}

CStreamReader* CLocatorAPI::rs_open(LPCSTR path, LPCSTR _fname) { return (r_open_impl<CStreamReader>(path, _fname)); }

IReader* CLocatorAPI::r_open(LPCSTR path, LPCSTR _fname) { return (r_open_impl<IReader>(path, _fname)); }

void CLocatorAPI::r_close(IReader*& fs)
{
    if (m_Flags.test(flDumpFileActivity))
        _unregister_open_file(fs);

    xr_delete(fs);
}

void CLocatorAPI::r_close(CStreamReader*& fs)
{
    if (m_Flags.test(flDumpFileActivity))
        _unregister_open_file(fs);

    fs->close();
}

IWriter* CLocatorAPI::w_open(LPCSTR path, LPCSTR _fname)
{
    string_path fname;
    strcpy_s(fname, _fname);
    xr_strlwr(fname); //,".$");
    if (path && path[0])
        update_path(fname, path, fname);
    CFileWriter* W = xr_new<CFileWriter>(fname, false);
    return W;
}

IWriter* CLocatorAPI::w_open_ex(LPCSTR path, LPCSTR _fname)
{
    string_path fname;
    strcpy_s(fname, _fname);
    xr_strlwr(fname); //,".$");
    if (path && path[0])
        update_path(fname, path, fname);
    CFileWriter* W = xr_new<CFileWriter>(fname, true);
    return W;
}

void CLocatorAPI::w_close(IWriter*& S)
{
    if (S)
    {
        R_ASSERT(S->fName.size());
        string_path fname;
        strcpy_s(fname, *S->fName);
        bool bReg = S->valid();
        xr_delete(S);

        if (bReg)
        {
            struct _stat st;
            _stat(fname, &st);
            Register(fname, 0xffffffff, 0, 0, st.st_size, st.st_size, (u32)st.st_mtime);
        }
    }
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(LPCSTR fname)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    file desc_f;
    desc_f.name = fname;

    return files.find(desc_f);
}

void CLocatorAPI::file_delete(LPCSTR path, LPCSTR nm)
{
    string_path fname;
    if (path && path[0])
        update_path(fname, path, nm);
    else
        strcpy_s(fname, nm);

    const files_it I = file_find_it(fname);
    if (I != files.end())
    {
        // remove file
        unlink(I->name);
        char* str = LPSTR(I->name);
        xr_free(str);
        files.erase(I);
    }
}

void CLocatorAPI::file_copy(LPCSTR src, LPCSTR dest)
{
#if __has_include("..\build_config_overrides\trivial_encryptor_ovr.h")
    FATAL("Mamkin Hacker Detected");
#else
    if (exist(src))
    {
        IReader* S = r_open(src);
        if (S)
        {
            IWriter* D = w_open(dest);
            if (D)
            {
                D->w(S->pointer(), S->length());
                w_close(D);
            }
            r_close(S);
        }
    }
#endif
}

void CLocatorAPI::file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite)
{
    files_it S = file_find_it(src);
    if (S != files.end())
    {
        files_it D = file_find_it(dest);
        if (D != files.end())
        {
            if (!bOwerwrite)
                return;
            unlink(D->name);
            char* str = LPSTR(D->name);
            xr_free(str);
            files.erase(D);

            S = file_find_it(src); //Обновим снова, потому что после erase итератор может быть невалидным
            R_ASSERT(S != files.end()); //на всякий случай
        }

        file new_desc = *S;
        // remove existing item
        char* str = LPSTR(S->name);
        xr_free(str);
        files.erase(S);
        // insert updated item
        new_desc.name = xr_strlwr(xr_strdup(dest));
        files.insert(new_desc);

        // physically rename file
        VerifyPath(dest);
        rename(src, dest);
    }
}

int CLocatorAPI::file_length(LPCSTR src)
{
    auto I = file_find_it(src);
    return I != files.end() ? I->size_real : -1;
}

bool CLocatorAPI::path_exist(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    return (P != pathes.end());
}

FS_Path* CLocatorAPI::append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive)
{
    VERIFY(root /**&&root[0]/**/);
    VERIFY(false == path_exist(path_alias));
    FS_Path* P = xr_new<FS_Path>(root, add, LPCSTR(0), LPCSTR(0), 0);
    bool bNoRecurse = !recursive;
    RecurseScanPhysicalPath(P->m_Path, false, bNoRecurse);
    pathes.emplace(xr_strdup(path_alias), P);
    return P;
}

FS_Path* CLocatorAPI::get_path(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    R_ASSERT2(P != pathes.end(), path);
    return P->second;
}

LPCSTR CLocatorAPI::update_path(string_path& dest, LPCSTR initial, LPCSTR src) 
{
    return get_path(initial)->_update(dest, src); 
}

u32 CLocatorAPI::get_file_age(LPCSTR nm)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    files_it I = file_find_it(nm);
    return (I != files.end()) ? I->modif : u32(-1);
}

void CLocatorAPI::rescan_physical_path(LPCSTR full_path, BOOL bRecurse)
{
    auto I = file_find_it(full_path);
    if (I == files.end())
        return;

    Msg("[rescan_physical_path] files count before: [%d]", files.size());

    const size_t base_len = strlen(full_path);

    while (I != files.end())
    {
        const file& entry = *I;

        if (0 != strncmp(entry.name, full_path, base_len))
            break; // end of list

        const char* entry_begin = entry.name + base_len;

        if (entry.vfs != 0xFFFFFFFF || entry.folder || (!bRecurse && strchr(entry_begin, '\\')))
        {
            I++;
        }
        else
        {
            //Msg("[rescan_physical_path] erace file: [%s]", entry.name);
            // erase item
            char* str = LPSTR(entry.name);
            xr_free(str);

            I = files.erase(I);
        }
    }

    Msg("[rescan_physical_path] files count before2: [%u]", files.size());

    bool bNoRecurse = !bRecurse;
    RecurseScanPhysicalPath(full_path, false, bNoRecurse);

    Msg("[rescan_physical_path] files count after: [%d]", files.size());
}

void CLocatorAPI::rescan_physical_pathes()
{
    m_Flags.set(flNeedRescan, FALSE);
    for (PathPairIt p_it = pathes.begin(); p_it != pathes.end(); p_it++)
    {
        FS_Path* P = p_it->second;
        if (P->m_Flags.is(FS_Path::flNeedRescan))
        {
            std::string filepath{P->m_Path};
            bool filepathExists = std::filesystem::exists(filepath);

            // рескан нужно делать только для реальных каталогов на диске
            if (filepathExists)
            {
                rescan_physical_path(P->m_Path, P->m_Flags.is(FS_Path::flRecurse));
            }

            P->m_Flags.set(FS_Path::flNeedRescan, FALSE);
        }
    }
}

void CLocatorAPI::lock_rescan() { m_iLockRescan++; }

void CLocatorAPI::unlock_rescan()
{
    m_iLockRescan--;
    VERIFY(m_iLockRescan >= 0);
    if ((0 == m_iLockRescan) && m_Flags.is(flNeedRescan))
        rescan_physical_pathes();
}

void CLocatorAPI::check_pathes()
{
    if (m_Flags.is(flNeedRescan) && (0 == m_iLockRescan))
    {
        lock_rescan();
        rescan_physical_pathes();
        unlock_rescan();
    }
}

void CLocatorAPI::register_archieve(LPCSTR path) { ProcessArchive(path); }

void CLocatorAPI::ProcessExternalArch()
{
    FS_FileSet fset;
    file_list(fset, "$mod_dir$", FS_ListFiles, "*.xdb*");

    FS_FileSetIt it = fset.begin();
    FS_FileSetIt it_e = fset.end();

    string_path full_mod_name, _path;
    for (; it != it_e; ++it)
    {
        Msg("--Found external arch: [%s], size: [%u]", (*it).name.c_str(), (*it).size);
        update_path(full_mod_name, "$mod_dir$", (*it).name.c_str());

        FS_Path* pFSRoot = FS.get_path("$fs_root$");

        strconcat(sizeof(_path), _path, pFSRoot->m_Path, "gamedata");

        ProcessArchive(full_mod_name, _path);
    }
}
