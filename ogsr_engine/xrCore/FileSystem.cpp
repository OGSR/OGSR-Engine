//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"

#include "cderr.h"
#include "commdlg.h"

std::unique_ptr<EFS_Utils> xr_EFS;

EFS_Utils::EFS_Utils() {}

EFS_Utils::~EFS_Utils() {}

xr_string EFS_Utils::ExtractFileName(LPCSTR src)
{
    string_path name;
    _splitpath(src, 0, 0, name, 0);
    return xr_string(name);
}

xr_string EFS_Utils::ExtractFileExt(LPCSTR src)
{
    string_path ext;
    _splitpath(src, 0, 0, 0, ext);
    return xr_string(ext);
}

xr_string EFS_Utils::ExtractFilePath(LPCSTR src)
{
    string_path drive, dir;
    _splitpath(src, drive, dir, 0, 0);
    return xr_string(drive) + dir;
}

xr_string EFS_Utils::ExcludeBasePath(LPCSTR full_path, LPCSTR excl_path)
{
    LPCSTR sub = strstr(full_path, excl_path);
    if (0 != sub)
        return xr_string(sub + xr_strlen(excl_path));
    else
        return xr_string(full_path);
}

xr_string EFS_Utils::ChangeFileExt(LPCSTR src, LPCSTR ext)
{
    xr_string tmp;
    LPSTR src_ext = strext(src);
    if (src_ext)
    {
        size_t ext_pos = src_ext - src;
        tmp.assign(src, 0, ext_pos);
    }
    else
    {
        tmp = src;
    }
    tmp += ext;
    return tmp;
}

xr_string EFS_Utils::ChangeFileExt(const xr_string& src, LPCSTR ext) { return ChangeFileExt(src.c_str(), ext); }

//----------------------------------------------------
LPCSTR EFS_Utils::AppendFolderToName(LPSTR tex_name, int depth, BOOL full_name)
{
    string256 _fn;
    strcpy(tex_name, AppendFolderToName(tex_name, _fn, depth, full_name));
    return tex_name;
}

LPCSTR EFS_Utils::AppendFolderToName(LPCSTR src_name, LPSTR dest_name, int depth, BOOL full_name)
{
    shared_str tmp = src_name;
    LPCSTR s = src_name;
    LPSTR d = dest_name;
    int sv_depth = depth;
    for (; *s && depth; s++, d++)
    {
        if (*s == '_')
        {
            depth--;
            *d = '\\';
        }
        else
        {
            *d = *s;
        }
    }
    if (full_name)
    {
        *d = 0;
        if (depth < sv_depth)
            strcat(dest_name, *tmp);
    }
    else
    {
        for (; *s; s++, d++)
            *d = *s;
        *d = 0;
    }
    return dest_name;
}

LPCSTR EFS_Utils::GenerateName(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name)
{
    int cnt = 0;
    string_path fn;
    if (base_name)
        strconcat(sizeof(fn), fn, base_path, base_name, def_ext);
    else
        sprintf_s(fn, sizeof(fn), "%s%02d%s", base_path, cnt++, def_ext);

    while (FS.exist(fn))
        if (base_name)
            sprintf_s(fn, sizeof(fn), "%s%s%02d%s", base_path, base_name, cnt++, def_ext);
        else
            sprintf_s(fn, sizeof(fn), "%s%02d%s", base_path, cnt++, def_ext);
    strcpy(out_name, fn);
    return out_name;
}

//#endif
