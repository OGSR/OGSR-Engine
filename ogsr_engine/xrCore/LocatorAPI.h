// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(push)
#pragma warning(disable : 4995)
#include <io.h>
#pragma warning(pop)

#include "LocatorAPI_defs.h"

class XRCORE_API CStreamReader;

class XRCORE_API CLocatorAPI
{
    friend class FS_Path;

public:
    struct file
    {
        LPCSTR name; // low-case name
        u32 vfs; // 0xffffffff - standart file
        u32 crc; // contents CRC
        u32 ptr; // pointer inside vfs
        u32 size_real; //
        u32 size_compressed; // if (size_real==size_compressed) - uncompressed
        u32 modif; // for editor
        bool folder{};
    };

private:
    struct file_pred
    {
        IC bool operator()(const file& x, const file& y) const { return xr_strcmp(x.name, y.name) < 0; }
    };
    struct archive
    {
        shared_str path;
        void *hSrcFile, *hSrcMap;
        u32 size;
    };

    DEFINE_MAP_PRED(LPCSTR, FS_Path*, PathMap, PathPairIt, pred_str);
    PathMap pathes;

    DEFINE_SET_PRED(file, files_set, files_it, file_pred);
    DEFINE_VECTOR(archive, archives_vec, archives_it);

    int m_iLockRescan;

    void rescan_physical_path(LPCSTR full_path, BOOL bRecurse);
    void check_pathes();

    files_set files;
    archives_vec archives;

    void Register(LPCSTR name, u32 vfs, u32 crc, u32 ptr, u32 size_real, u32 size_compressed, u32 modif);
    void ProcessArchive(LPCSTR path, LPCSTR base_path = NULL);
    void ProcessOne(LPCSTR path, const _finddata_t& F, bool bNoRecurse);
    bool RecurseScanPhysicalPath(const char* path, const bool log_if_found, bool bNoRecurse);

    files_it file_find_it(LPCSTR n);

public:
    enum
    {
        flNeedRescan = (1 << 0),
        flBuildCopy = (1 << 1),
        flReady = (1 << 2),
        flEBuildCopy = (1 << 3),
        flEventNotificator = (1 << 4),
        flTargetFolderOnly = (1 << 5),
        flCacheFiles = (1 << 6),
        flScanAppRoot = (1 << 7),
        flNeedCheck = (1 << 8),
        flDumpFileActivity = (1 << 9),
    };
    Flags32 m_Flags;
    u32 dwAllocGranularity;
    u32 dwOpenCounter;

private:
    void file_from_cache_impl(IReader*& R, LPSTR fname, const file& desc);
    void file_from_cache_impl(CStreamReader*& R, LPSTR fname, const file& desc);

    template <typename T>
    void file_from_cache(T*& R, LPSTR fname, const file& desc, LPCSTR& source_name);

    void file_from_archive(IReader*& R, LPCSTR fname, const file& desc);
    void file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc);

    bool check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname, const file*& desc);

    template <typename T>
    IC T* r_open_impl(LPCSTR path, LPCSTR _fname);
    void ProcessExternalArch();

public:
    CLocatorAPI();
    ~CLocatorAPI();
    void _initialize(u32 flags, LPCSTR target_folder = 0, LPCSTR fs_name = 0);
    void _destroy();

    CStreamReader* rs_open(LPCSTR initial, LPCSTR N);
    IReader* r_open(LPCSTR initial, LPCSTR N);
    IC IReader* r_open(LPCSTR N) { return r_open(0, N); }
    void r_close(IReader*& S);
    void r_close(CStreamReader*& fs);

    IWriter* w_open(LPCSTR initial, LPCSTR N);
    IC IWriter* w_open(LPCSTR N) { return w_open(0, N); }
    IWriter* w_open_ex(LPCSTR initial, LPCSTR N);
    IC IWriter* w_open_ex(LPCSTR N) { return w_open_ex(0, N); }
    void w_close(IWriter*& S);

    const file* exist(LPCSTR N);
    const file* exist(LPCSTR path, LPCSTR name);
    const file* exist(string_path& fn, LPCSTR path, LPCSTR name);
    const file* exist(string_path& fn, LPCSTR path, LPCSTR name, LPCSTR ext);

    void file_delete(LPCSTR path, LPCSTR nm);
    void file_delete(LPCSTR full_path) { file_delete(0, full_path); }
    void file_copy(LPCSTR src, LPCSTR dest);
    void file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite = true);
    int file_length(LPCSTR src);

    u32 get_file_age(LPCSTR nm);

    xr_vector<LPSTR>* file_list_open(LPCSTR initial, LPCSTR folder, u32 flags = FS_ListFiles);
    xr_vector<LPSTR>* file_list_open(LPCSTR path, u32 flags = FS_ListFiles);
    void file_list_close(xr_vector<LPSTR>*& lst);

    bool path_exist(LPCSTR path);
    FS_Path* get_path(LPCSTR path);
    FS_Path* append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive);
    LPCSTR update_path(string_path& dest, LPCSTR initial, LPCSTR src);

    int file_list(FS_FileSet& dest, LPCSTR path, u32 flags = FS_ListFiles, LPCSTR mask = 0);

    void register_archieve(LPCSTR path);

    // editor functions
    void rescan_physical_pathes();
    void lock_rescan();
    void unlock_rescan();
};

extern XRCORE_API std::unique_ptr<CLocatorAPI> xr_FS;
#define FS (*xr_FS)
