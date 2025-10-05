// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable : 4995)
#include <direct.h>
#include <fcntl.h>
#include <sys/stat.h>
#pragma warning(default : 4995)

#include <filesystem>

namespace sqfs
{
#include <sqfs/super.h>
}

#include "FS_internal.h"
#include "stream_reader.h"
#include "file_stream_reader.h"

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
    bool operator()(_open_file& itm) { return (_val == itm._fn && itm._reader == nullptr); }
};
struct eq_fname_check
{
    shared_str _val;
    eq_fname_check(shared_str s) { _val = s; }
    bool operator()(_open_file& itm) { return (_val == itm._fn && itm._reader != nullptr); }
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
    _of._reader = nullptr;
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
            if (_of._reader != nullptr)
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
            if (_of._reader == nullptr)
                Msg("[%d] fname:%s", _of._used, _of._fn.c_str());
        }
    }
    if (bShow)
        Msg("----total count = [%u]", g_open_files.size());
}

CLocatorAPI::CLocatorAPI()
{
    m_Flags.zero();
    // get page size
    SYSTEM_INFO sys_inf;
    GetSystemInfo(&sys_inf);
    dwAllocGranularity = sys_inf.dwAllocationGranularity;
    dwOpenCounter = 0;
}

CLocatorAPI::~CLocatorAPI()
{
    _dump_open_files(1);
}

void CLocatorAPI::Register(LPCSTR name, size_t vfs, u32 ptr, u32 size_real, u32 size_compressed, u32 time_write, bool folder)
{
    string256 temp_file_name;
    strcpy_s(temp_file_name, name);
    xr_strlwr(temp_file_name);

    // Register file
    file desc;

    desc.vfs = vfs;
    desc.ptr = ptr;
    desc.size_real = size_real;
    desc.size_compressed = size_compressed;
    desc.time_write = time_write & (~u32(0x3));
    desc.folder = folder;

    // if file already exist - update info
    if (const files_it I = file_find_it(temp_file_name); I != files.end())
    {
        desc.name = I->name;

        // sad but true, performance option
        // correct way is to erase and then insert new record:
        const_cast<file&>(*I) = desc;
        return;
    }

    desc.name = xr_strdup(temp_file_name);

    // otherwise insert file
    files.insert(desc);

    // Try to register folder(s)
    RegisterFolderHierarchy(desc.name);
}

void CLocatorAPI::RegisterFolderHierarchy(LPCSTR folder_path)
{
    string_path temp;
    strcpy_s(temp, folder_path);

    string_path path;
    string_path folder;
    while (temp[0])
    {
        _splitpath(temp, path, folder, nullptr, nullptr);

        if (!folder[0])
            break;
        strcat_s(path, folder);
        if (!exist(path))
        {
            file desc;

            desc.name = xr_strdup(path);
            desc.vfs = VFS_STANDARD_FILE;
            desc.ptr = 0;
            desc.size_real = 0;
            desc.size_compressed = 0;
            desc.time_write = u32(-1);
            desc.folder = true;

            std::pair<files_it, bool> I = files.insert(desc);

            R_ASSERT(I.second);
        }
        strcpy_s(temp, path); // strcpy_s(temp, folder);
        if (xr_strlen(temp))
            temp[xr_strlen(temp) - 1] = 0;
    }
}

/* Archives */

void CLocatorAPI::archive::open()
{
    // Open the file
    if (hSrcFile && hSrcMap)
        return;

    hSrcFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    R_ASSERT(hSrcFile != INVALID_HANDLE_VALUE);

    LARGE_INTEGER sz;
    GetFileSizeEx(hSrcFile, &sz);
    size = sz.QuadPart;
    R_ASSERT(size > 0);

    u32 dwPtr = SetFilePointer(hSrcFile, 0, nullptr, FILE_BEGIN);
    R_ASSERT(dwPtr != INVALID_SET_FILE_POINTER, path.c_str(), Debug.error2string(GetLastError()));

    u32 magic = u32(-1);
    DWORD read_byte;
    bool res = ReadFile(hSrcFile, &magic, 4, &read_byte, nullptr);
    R_ASSERT(res && read_byte == sizeof(u32), path.c_str(), Debug.error2string(GetLastError()));

    if (magic == SQFS_MAGIC)
        open_sqfs();
    else
        open_db();
}

IC void CLocatorAPI::archive::index(CLocatorAPI& loc, const char* fs_entry_point)
{
    if (type == container::SQFS)
        index_sqfs(loc, fs_entry_point);
    else
        index_db(loc, fs_entry_point);
}

IC IReader* CLocatorAPI::archive::read(const char* fname, const struct file& desc, u32 gran)
{
    return type == container::SQFS ? read_sqfs(fname, desc, gran) : read_db(fname, desc, gran);
}

IC CStreamReader* CLocatorAPI::archive::stream(const char* fname, const struct file& desc) { return type == container::SQFS ? stream_sqfs(fname, desc) : stream_db(fname, desc); }

IC void CLocatorAPI::archive::cleanup()
{
    if (type == container::SQFS)
        cleanup_sqfs();
    else
        cleanup_db();
}

void CLocatorAPI::archive::close()
{
    if (type == container::SQFS)
        close_sqfs();
    else
        close_db();

    CloseHandle(hSrcFile);
    hSrcFile = nullptr;
}

void CLocatorAPI::LoadArchive(archive& A, LPCSTR base_path)
{
    // Create base path
    string_path fs_entry_point;
    if (base_path && base_path[0])
    {
        strcpy_s(fs_entry_point, base_path);
    }
    else
    {
        strcpy_s(fs_entry_point, A.path.c_str());
        if (strext(fs_entry_point))
            *strext(fs_entry_point) = 0;
    }
    strcat_s(fs_entry_point, "\\");

    // Read FileSystem
    A.open();
    A.index(*this, fs_entry_point);
}

void CLocatorAPI::ProcessArchive(LPCSTR _path, LPCSTR base_path)
{
    // find existing archive
    shared_str path = _path;

    for (auto& archive : archives)
        if (archive.path == path)
            return;

    // open archive
    auto& A = archives.emplace_back();
    A.vfs_idx = archives.size() - 1;
    A.path = path;

    A.open();

    LoadArchive(A, base_path);
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
        if (0 == xr_strcmp(F.name, "."))
            return;
        if (0 == xr_strcmp(F.name, ".."))
            return;

        strcat_s(N, "\\");

        if (bNoRecurse)
        {
            // register folder itself
            Register(N, VFS_STANDARD_FILE, 0, 0, 0, (u32)-1, true);

            return;
        }

        RecurseScanPhysicalPath(N, false, bNoRecurse);
    }
    else
    {
        if (strext(N) && (0 == strncmp(strext(N), ".db", 3) || 0 == strncmp(strext(N), ".sq", 3)))
        {
            string_path base_path{};
            if (FS.path_exist(fsgame::fs_root))
            {
                strconcat(sizeof(_path), base_path, FS.get_path(fsgame::fs_root)->m_Path, "gamedata");
            }

            Msg("--Found base arch: [%s], size: [%u]", N, F.size);
            ProcessArchive(N, base_path);
        }
        else
        {
            Register(N, VFS_STANDARD_FILE, 0, F.size, F.size, (u32)F.time_write);
        }
    }
}

IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y) { return xr_strcmp(x.name, y.name) < 0; }

bool is_file_exists(const char* _path)
{
    struct stat file_info;

    if (stat(_path, &file_info) == 0 && file_info.st_mode & S_IFREG)
    {
        return true;
    }

    return false;
}

bool is_dir_exists(const char* _path)
{
    struct stat file_info;

    if (stat(_path, &file_info) == 0 && file_info.st_mode & S_IFDIR)
    {
        return true;
    }

    return false;
}

bool ignore_name(const char* _name)
{
    if (!strcmp(_name, "."))
        return true;
    if (!strcmp(_name, ".."))
        return true;

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

bool ignore_path(const char* _path) // _path can be a file or a folder
{
    struct stat file_info;

    if (stat(_path, &file_info) == 0)
{
        return false;
    }

    return true;
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
    {
        int files_cnt = 0;

        for (const auto& el : rec_files)
        {
            if (el.attrib & _A_HIDDEN)
            {
                continue;
            }

            if (0 == xr_strcmp(el.name, "."))
                continue;
            if (0 == xr_strcmp(el.name, ".."))
                continue;

            files_cnt++;
        }

        Msg("  items: [%u]", files_cnt);
    }

    std::sort(rec_files.begin(), rec_files.end(), pred_str_ff);

    for (const auto& el : rec_files)
    {
        ProcessOne(path, el, bNoRecurse);
    }

    return true;
}

void CLocatorAPI::init_gamedata_unused()
{
    static bool dev_gamedata_unused = strstr(Core.Params, "-dev_gamedata_unused");

    if (dev_gamedata_unused)
    {
        string_path fname;
        FS.update_path(fname, fsgame::app_data_root, "gamedata_unused.ltx");

        CInifile* tmp = xr_new<CInifile>(fname, FALSE);

        if (FS.exist(fname))
        {
            // tmp->load_file();
        }
        else
        {
            FS_FileSet files_set;

            FS.file_list(files_set, fsgame::game_sounds, FS_ListFiles, "*.*");
            for (const auto& file : files_set)
            {
                tmp->w_string(fsgame::game_sounds, file.name.c_str(), nullptr);
            }

            files_set.clear();
            FS.file_list(files_set, fsgame::game_textures, FS_ListFiles, "*.*");
            for (const auto& file : files_set)
            {
                tmp->w_string(fsgame::game_textures, file.name.c_str(), nullptr);
            }

            files_set.clear();
            FS.file_list(files_set, fsgame::game_meshes, FS_ListFiles, "*.*");
            for (const auto& file : files_set)
            {
                tmp->w_string(fsgame::game_meshes, file.name.c_str(), nullptr);
            }

            files_set.clear();

            tmp->save_as();
        }

        gamedata_unused_references = tmp;
    }
}

void CLocatorAPI::_initialize(u32 flags, LPCSTR fs_name)
{
    if (m_Flags.is(flReady))
        return;
    CTimer t;
    t.Start();
    Log("Initializing File System...");
    u32 M1 = Memory.mem_usage();

    m_Flags.set(flags, TRUE);

    // scan root directory
    string4096 buf{};
    IReader* pFSltx{};
    // append working folder
    LPCSTR fs_ltx{};

    // append application path
    append_path(fsgame::app_root, Core.ApplicationPath, nullptr, FALSE);

    {
        // find nearest fs.ltx and set fs_root correctly
        fs_ltx = (fs_name && fs_name[0]) ? fs_name : FSLTX;
        pFSltx = r_open(fs_ltx);

        if (!pFSltx)
            pFSltx = r_open(fsgame::app_root, fs_ltx);

        // находим где ж лежит fsgame.ltx

        if (!pFSltx)
        {
            if (strstr(Core.Params, "-use-work-dir"))
            {
                string_path currentDir;
                GetCurrentDirectory(std::size(currentDir) - 1, currentDir);
                currentDir[std::size(currentDir) - 1] = '\0';
                append_path(fsgame::fs_root, currentDir, nullptr, FALSE);
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

                    append_path(fsgame::fs_root, tmpAppPath, nullptr, FALSE);
                }
                else
                    append_path(fsgame::fs_root, "", nullptr, FALSE);
            }

            pFSltx = r_open(fsgame::fs_root, fs_ltx);
        }
        else
        {
            append_path(fsgame::fs_root, "", nullptr, FALSE);
        }

        Msg("using fs-ltx: [%s]", fs_ltx);
    }

    {
        CHECK_OR_EXIT(pFSltx, make_string("Cannot open file \"%s\".\nCheck your working folder.", fs_ltx));
        // append all pathes
        string_path id, root, add, def;
        LPCSTR lp_add, lp_def;
        string16 b_v;
        string4096 temp;

        while (!pFSltx->eof())
        {
            constexpr char _delimiter = '|';

            pFSltx->r_string(buf, sizeof(buf));
            if (buf[0] == ';')
                continue;

            _GetItem(buf, 0, id, '=');

            _GetItem(buf, 1, temp, '=');

            int cnt = _GetItemCount(temp, _delimiter);
            R_ASSERT(cnt >= 3, temp);
            u32 fl = 0;
            _GetItem(temp, 0, b_v, _delimiter);

            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flRecurse;

            _GetItem(temp, 1, b_v, _delimiter);
            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flNotif; // unused ???

            _GetItem(temp, 2, root, _delimiter);
            _GetItem(temp, 3, add, _delimiter);
            _GetItem(temp, 4, def, _delimiter);

            xr_strlwr(id);
            xr_strlwr(root);

            lp_add = (cnt >= 4) ? xr_strlwr(add) : nullptr;
            lp_def = (cnt >= 5) ? def : nullptr;

            PathPairIt p_it = pathes.find(root);

            FS_Path* P = xr_new<FS_Path>((p_it != pathes.end()) ? p_it->second->m_Path : root, lp_add, lp_def, fl);
            bool bNoRecurse = !(fl & FS_Path::flRecurse);
#ifdef RESTRICT_GAMEDATA
            if (!strcmp(id, "$app_data_root$") || !strcmp(id, "$game_saves$") || !strcmp(id, "$logs$") || !strcmp(id, "$screenshots$"))
                RecurseScanPhysicalPath(P->m_Path);
#else
            RecurseScanPhysicalPath(P->m_Path, true, bNoRecurse);
#endif

            std::pair<PathPairIt, bool> I;
            I = pathes.emplace(xr_strdup(id), P);
            CHECK_OR_EXIT(I.second, "The file 'fsgame.ltx' is corrupted (it contains duplicated lines).\nPlease reinstall the game or fix the problem manually.");
        }
        r_close(pFSltx);
        R_ASSERT(path_exist(fsgame::app_data_root));
    }

#ifndef RESTRICT_GAMEDATA
    string_path base_path;
    strconcat(sizeof(_path), base_path, FS.get_path(fsgame::fs_root)->m_Path, "gamedata");

    ProcessExternalMods(base_path);
#endif

    u32 M2 = Memory.mem_usage();
    Msg("FS: %d files cached, %dKb memory used.", files.size(), (M2 - M1) / 1024);

    m_Flags.set(flReady, TRUE);

    Msg("Init FileSystem %f sec", t.GetElapsed_sec());

    CreateLog(nullptr != strstr(Core.Params, "-nolog"));
}

void CLocatorAPI::_destroy()
{
    for (const auto& file : files)
    {
        free_file(file);
    }
    files.clear();
    for (auto& pathe : pathes)
    {
        char* str = LPSTR(pathe.first);
        xr_free(str);
        xr_delete(pathe.second);
    }
    pathes.clear();
    for (auto& archive : archives)
    {
        archive.cleanup();
        archive.close();
    }
    archives.clear();

    if (gamedata_unused_references)
    {
        gamedata_unused_references->save_as();
        xr_delete(gamedata_unused_references);
    }
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

    string_path N;

    if (path_exist(_path))
        update_path(N, _path, "");
    else
        strcpy_s(N, _path);

    files_it I = file_find_it(N);
    if (I == files.end())
        return nullptr;

    auto dest = xr_new<xr_vector<char*>>();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); ++I)
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
            dest->push_back(xr_strdup(entry_begin));
            LPSTR fname = dest->back();
            if (flags & FS_ClampExt)
                if (nullptr != strext(fname))
                    *strext(fname) = 0;
        }
        else
        {
            // folder
            if ((flags & FS_ListFolders) == 0)
                continue;

            LPCSTR entry_begin = entry.name + base_len;
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
        for (auto& I : *lst)
            xr_free(I);
        xr_delete(lst);
    }
}

int CLocatorAPI::file_list(FS_FileSet& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
    R_ASSERT(path);
    VERIFY(flags);

    string_path N;
    if (path_exist(path))
        update_path(N, path, "");
    else
        strcpy_s(N, path);

    files_it I = file_find_it(N);
    if (I == files.end())
        return 0;

    SStringVec masks;
    _SequenceToList(masks, mask);
    BOOL b_mask = !masks.empty();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); ++I)
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
                for (auto& mask : masks)
                {
                    if (pattern_match(entry_begin, mask.c_str()))
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

            u32 fl = (entry.vfs != VFS_STANDARD_FILE ? FS_File::flVFS : 0);
            dest.emplace(fn, entry.size_real, entry.time_write, fl, !(flags & FS_NoLower));
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
            dest.emplace(entry_begin, entry.size_real, entry.time_write, fl, !(flags & FS_NoLower));
        }
    }
    return int(dest.size());
}

void CLocatorAPI::file_from_cache_impl(IReader*& R, LPCSTR fname, const file& desc)
{
    R = xr_new<CVirtualFileReader>(fname);
}

void CLocatorAPI::file_from_cache_impl(CStreamReader*& R, LPCSTR fname, const file& desc)
{
    CFileStreamReader* r = xr_new<CFileStreamReader>();
    r->construct(fname, BIG_FILE_READER_WINDOW_SIZE);
    R = r;
}

template <typename T>
void CLocatorAPI::file_from_cache(T*& R, LPCSTR fname, const file& desc)
{
    file_from_cache_impl(R, fname, desc);
}

void CLocatorAPI::file_from_archive(IReader*& R, LPCSTR fname, const file& desc)
{
    // Archived one
    R = archives[desc.vfs].read(fname, desc, dwAllocGranularity);
}

void CLocatorAPI::file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc)
{
    CStreamReader* reader = archives[desc.vfs].stream(fname, desc);
    R = reader;
}

bool CLocatorAPI::check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname_result, const file*& desc)
{
    // correct path
    strcpy_s(fname_result, _fname);
    xr_strlwr(fname_result);

    if (path && path[0])
        update_path(fname_result, path, _fname);

    // Search entry
    files_it I = file_find_it(fname_result);
    if (I == files.end())
        return (false);

    ++dwOpenCounter;
    desc = &*I;
    return (true);
}

template <typename T>
T* CLocatorAPI::r_open_impl(LPCSTR path, LPCSTR _fname)
{
    string_path fname;
    const file* desc = nullptr;

    if (!check_for_file(path, _fname, fname, desc))
    {
        if (is_file_exists(_fname))
        {
            T* R = nullptr;
            file_from_cache(R, fname, *desc);
            return (R);
        }

        return nullptr;
    }

    T* R = nullptr;

    // OK, analyse
    if (VFS_STANDARD_FILE == desc->vfs)
    {
        LPCSTR actual_name = desc->real_file_path.empty() ? fname : desc->real_file_path.c_str();
        file_from_cache(R, actual_name, *desc);
    }
    else
    {
        file_from_archive(R, fname, *desc);
    }

    if (m_Flags.test(flDumpFileActivity))
    {
        _register_open_file(R, fname);
    }

    return (R);
}

CStreamReader* CLocatorAPI::rs_open(LPCSTR path, LPCSTR _fname) { return (r_open_impl<CStreamReader>(path, _fname)); } // only for ".geom", ".geomx", ".ogm"

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
        strcpy_s(fname, S->fName.c_str());
        bool bReg = S->valid();
        xr_delete(S);

        if (bReg)
        {
            struct _stat st;
            _stat(fname, &st);
            Register(fname, VFS_STANDARD_FILE, 0, st.st_size, st.st_size, (u32)st.st_mtime);
        }
    }
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(LPCSTR fname)
{
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
        _unlink(I->name);

        free_file(*I);
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

bool CLocatorAPI::path_exist(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    return (P != pathes.end());
}

FS_Path* CLocatorAPI::append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive)
{
    VERIFY(root);
    VERIFY(false == path_exist(path_alias));
    FS_Path* P = xr_new<FS_Path>(root, add);
    bool bNoRecurse = !recursive;
    RecurseScanPhysicalPath(P->m_Path, false, bNoRecurse);
    pathes.emplace(xr_strdup(path_alias), P);
    return P;
}

FS_Path* CLocatorAPI::get_path(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    R_ASSERT(P != pathes.end(), path);
    return P->second;
}

static std::recursive_mutex file_copy_mutex;

LPCSTR CLocatorAPI::update_path(string_path& dest, LPCSTR initial, LPCSTR src)
{
    static bool dev_reference_copy = !!strstr(Core.Params, "-dev_reference_copy");

    string_path src_origin{};

    if (dev_reference_copy || gamedata_unused_references)
    {
        strcpy_s(src_origin, sizeof(src_origin), src);
    }

    LPCSTR r = get_path(initial)->_update(dest, src);

    if (dev_reference_copy)
    {
        std::scoped_lock lock(file_copy_mutex);

        string_path fn_ref;

        if (strcmp(initial, fsgame::game_sounds) == 0)
        {
            if (!exist(dest))
            {
                if (exist(fn_ref, fsgame::game_sounds_reference, src_origin))
                {
                    Msg("[NFTC]: Sound founded: [%s] in $game_sounds_reference$ folder. Copying begins...", src_origin);
                    file_copy(fn_ref, dest);
                }
            }
        }
        else if (strcmp(initial, fsgame::game_textures) == 0)
        {
            if (!exist(dest))
            {
                if (exist(fn_ref, fsgame::game_textures_reference, src_origin))
                {
                    Msg("[NFTC]: Texture founded: [%s] in $game_textures_reference$ folder. Copying begins...", src_origin);
                    file_copy(fn_ref, dest);
                }
            }
        }
        else if (strcmp(initial, fsgame::game_meshes) == 0)
        {
            if (!exist(dest))
            {
                if (exist(fn_ref, fsgame::game_meshes_reference, src_origin))
                {
                    Msg("[NFTC]: Mesh founded: [%s] in $game_meshes_reference$ folder. Copying begins...", src_origin);
                    file_copy(fn_ref, dest);
                }
            }
        }
    }

    // v2 ))

    if (gamedata_unused_references)
    {
        if (exist(dest))
        {
            if (strcmp(initial, fsgame::game_sounds) == 0
                || strcmp(initial, fsgame::game_textures) == 0
                || strcmp(initial, fsgame::game_meshes) == 0)
            {
                std::scoped_lock lock(file_copy_mutex);

                gamedata_unused_references->remove_line(initial, src_origin);
            }
        }
    }

    return r;
}

u32 CLocatorAPI::get_file_age(LPCSTR nm)
{
    files_it I = file_find_it(nm);
    return (I != files.end()) ? I->time_write : u32(-1);
}

void CLocatorAPI::rescan_physical_path(LPCSTR full_path, BOOL bRecurse)
{
    auto I = file_find_it(full_path);
    if (I == files.end())
        return;

    MsgDbg("[rescan_physical_path] files count before: [%d]", files.size());

    const size_t base_len = strlen(full_path);

    while (I != files.end())
    {
        const file& entry = *I;

        if (0 != strncmp(entry.name, full_path, base_len))
            break; // end of list

        const char* entry_begin = entry.name + base_len;

        if (entry.vfs != VFS_STANDARD_FILE
            || entry.folder || (!bRecurse && strchr(entry_begin, '\\')))
        {
            ++I;
            continue;
        }

            //Msg("[rescan_physical_path] erace file: [%s]", entry.name);

            // erase item
            free_file(entry);

            I = files.erase(I);
        }

    bool bNoRecurse = !bRecurse;
    RecurseScanPhysicalPath(full_path, false, bNoRecurse);

    MsgDbg("[rescan_physical_path] files count after: [%d]", files.size());
}

void CLocatorAPI::rescan_physical_pathes()
{
    for (auto& pathe : pathes)
    {
        FS_Path* P = pathe.second;
        if (P->m_Flags.is(FS_Path::flNeedRescan))
        {
            // рескан нужно делать только для реальных каталогов на диске
            if (is_dir_exists(P->m_Path))
            {
                rescan_physical_path(P->m_Path, P->m_Flags.is(FS_Path::flRecurse));
            }

            P->m_Flags.set(FS_Path::flNeedRescan, FALSE);
        }
    }
}

void CLocatorAPI::ProcessExternalMods(LPCSTR base_path)
{
    FS_FileSet fset;

    file_list(fset, fsgame::mod_dir, FS_ListFiles, "*.xdb*");
    file_list(fset, fsgame::mod_dir, FS_ListFiles, "*.xsq*");

    FS_FileSetIt it = fset.begin();
    FS_FileSetIt it_e = fset.end();

    string_path full_mod_name;

    for (; it != it_e; ++it)
    {
        Msg("--Found external mod: [%s], size: [%u]", it->name.c_str(), it->size);
        update_path(full_mod_name, fsgame::mod_dir, it->name.c_str());

        ProcessArchive(full_mod_name, base_path);
    }
}

void CLocatorAPI::free_file(const file& f)
{
    char* str = LPSTR(f.name);
    xr_free(str);
}