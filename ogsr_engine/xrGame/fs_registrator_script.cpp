#include "stdafx.h"
#include "fs_registrator.h"
#include "LocatorApi.h"

using namespace luabind;

LPCSTR get_file_age_str(CLocatorAPI* fs, LPCSTR nm);

LPCSTR update_path_script(CLocatorAPI* fs, LPCSTR initial, LPCSTR src)
{
    string_path temp;
    shared_str temp_2;
    fs->update_path(temp, initial, src);
    temp_2 = temp;
    return *temp_2;
}

class FS_file_list
{
    xr_vector<LPSTR>* m_p;

public:
    FS_file_list(xr_vector<LPSTR>* p) : m_p(p) {}
    u32 Size() { return m_p->size(); }
    LPCSTR GetAt(u32 idx) { return m_p->at(idx); }
    void Free() { FS.file_list_close(m_p); };
};

struct FS_item
{
    string_path name;
    u32 size;
    u32 modif;
    string256 buff;

    LPCSTR NameShort() { return name; }
    LPCSTR NameFull() { return name; }
    u32 Size() { return size; }
    LPCSTR Modif()
    {
        struct tm* newtime;
        time_t t = modif;
        newtime = localtime(&t);
        strcpy_s(buff, asctime(newtime));
        return buff;
    }

    LPCSTR ModifDigitOnly()
    {
        struct tm* newtime;
        time_t t = modif;
        newtime = localtime(&t);
        sprintf_s(buff, "%02d:%02d:%4d %02d:%02d", newtime->tm_mday, newtime->tm_mon + 1, newtime->tm_year + 1900, newtime->tm_hour, newtime->tm_min);
        return buff;
    }
};

template <bool b>
bool sizeSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (itm1.size < itm2.size);
    return (itm2.size < itm1.size);
}
template <bool b>
bool modifSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (itm1.modif < itm2.modif);
    return (itm2.modif < itm1.modif);
}
template <bool b>
bool nameSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (xr_strcmp(itm1.name, itm2.name) < 0);
    return (xr_strcmp(itm2.name, itm1.name) < 0);
}

class FS_file_list_ex
{
    xr_vector<FS_item> m_file_items;

public:
    enum
    {
        eSortByNameUp = 0,
        eSortByNameDown,
        eSortBySizeUp,
        eSortBySizeDown,
        eSortByModifUp,
        eSortByModifDown
    };
    FS_file_list_ex(LPCSTR path, u32 flags, LPCSTR mask);

    u32 Size() { return m_file_items.size(); }
    FS_item GetAt(u32 idx) { return m_file_items[idx]; }
    void Sort(u32 flags);
    decltype(m_file_items)& GetAll() { return m_file_items; }
};

FS_file_list_ex::FS_file_list_ex(LPCSTR path, u32 flags, LPCSTR mask)
{
    FS_Path* P = FS.get_path(path);
    P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
    FS.m_Flags.set(CLocatorAPI::flNeedCheck, TRUE);
    FS.rescan_physical_pathes();

    FS_FileSet files;
    FS.file_list(files, path, flags, mask);

    for (FS_FileSetIt it = files.begin(); it != files.end(); ++it)
    {
        auto& itm = m_file_items.emplace_back();
        ZeroMemory(itm.name, sizeof(itm.name));
        strcat_s(itm.name, it->name.c_str());
        itm.modif = (u32)it->time_write;
        itm.size = it->size;
    }

    FS.m_Flags.set(CLocatorAPI::flNeedCheck, FALSE);
}

void FS_file_list_ex::Sort(u32 flags)
{
    if (flags == eSortByNameUp)
        std::sort(m_file_items.begin(), m_file_items.end(), nameSorter<true>);
    else if (flags == eSortByNameDown)
        std::sort(m_file_items.begin(), m_file_items.end(), nameSorter<false>);
    else if (flags == eSortBySizeUp)
        std::sort(m_file_items.begin(), m_file_items.end(), sizeSorter<true>);
    else if (flags == eSortBySizeDown)
        std::sort(m_file_items.begin(), m_file_items.end(), sizeSorter<false>);
    else if (flags == eSortByModifUp)
        std::sort(m_file_items.begin(), m_file_items.end(), modifSorter<true>);
    else if (flags == eSortByModifDown)
        std::sort(m_file_items.begin(), m_file_items.end(), modifSorter<false>);
}

FS_file_list_ex file_list_open_ex(CLocatorAPI* fs, LPCSTR path, u32 flags, LPCSTR mask) { return FS_file_list_ex(path, flags, mask); }

FS_file_list file_list_open_script(CLocatorAPI* fs, LPCSTR initial, u32 flags) { return FS_file_list(fs->file_list_open(initial, flags)); }

FS_file_list file_list_open_script_2(CLocatorAPI* fs, LPCSTR initial, LPCSTR folder, u32 flags) { return FS_file_list(fs->file_list_open(initial, folder, flags)); }

LPCSTR get_file_age_str(CLocatorAPI* fs, LPCSTR nm)
{
    time_t t = fs->get_file_age(nm);
    struct tm* newtime;
    newtime = localtime(&t);
    return asctime(newtime);
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// SCRIPT C++17 FILESYSTEM - START ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#include <iomanip>
#include <sstream>
#include <filesystem>
namespace stdfs = std::filesystem;

//Путь до папки с движком
static std::string get_engine_dir() { return stdfs::current_path().string(); }

//Перебор файлов в папке, подкаталоги не учитываются.
static void directory_iterator(const char* dir, const luabind::functor<void>& iterator_func)
{
    if (!stdfs::exists(dir))
        return;

    for (const auto& file : stdfs::directory_iterator(dir))
        if (stdfs::is_regular_file(file)) //Папки не учитываем
            iterator_func(file);
}

//Перебор файлов в папке включая подкаталоги.
static void recursive_directory_iterator(const char* dir, const luabind::functor<void>& iterator_func)
{
    if (!stdfs::exists(dir))
        return;

    for (const auto& file : stdfs::recursive_directory_iterator(dir))
        if (stdfs::is_regular_file(file)) //Папки не учитываем
            iterator_func(file);
}

//полный путь до файла с расширением.
static std::string get_full_path(const stdfs::directory_entry& file) { return file.path().string(); }

//имя файла без пути, но с расширением.
static std::string get_full_filename(const stdfs::directory_entry& file) { return file.path().filename().string(); }

//имя файла без пути, и без расширения.
static std::string get_short_filename(const stdfs::directory_entry& file) { return file.path().stem().string(); }

//расширение файла.
static std::string get_extension(const stdfs::directory_entry& file) { return file.path().extension().string(); }

extern "C" int64_t __cdecl _Last_write_time(const wchar_t*); //так не надо делать нигде и никогда

//Время последнего изменения файла
static decltype(auto) get_last_write_time(const stdfs::directory_entry& file)
{
    // Это ужасный костыль на самом деле, надо переписать нормально под новый стандарт, но мне щас лень возиться.
    const auto ftime = std::chrono::system_clock::time_point{std::chrono::system_clock::duration{_Last_write_time(file.path().c_str())}};
    const auto cftime = decltype(ftime)::clock::to_time_t(ftime);
    return cftime;
}

static auto format_last_write_time = [](const stdfs::directory_entry& file, const char* fmt) {
    static std::ostringstream ss;
    static const std::locale loc{""};
    if (loc != ss.getloc())
        ss.imbue(loc); //Устанавливаем системную локаль потоку, чтоб месяц/день недели были на системном языке.

    ss.str("");

    const auto write_time_c = get_last_write_time(file);

    ss << std::put_time(std::localtime(&write_time_c), fmt);
    return ss.str();
};

//Время последнего изменения файла в формате [вторник 02 янв 2018 14:03:32]
static std::string get_last_write_time_string(const stdfs::directory_entry& file) { return format_last_write_time(file, "[%A %d %b %Y %T]"); }

//Время последнего изменения файла в формате [02:01:2018 14:03:32]
static std::string get_last_write_time_string_short(const stdfs::directory_entry& file) { return format_last_write_time(file, "[%d:%m:%Y %T]"); }

#pragma optimize("s", on)
void script_register_stdfs(lua_State* L)
{
    using self = stdfs::directory_entry;

    module(L, "stdfs")[def("VerifyPath", [](const char* path) { VerifyPath(path); }), def("directory_iterator", &directory_iterator),
                       def("recursive_directory_iterator", &recursive_directory_iterator),
                       class_<self>("path")
                           .def(constructor<const char*>())
                           // TODO: при необходимости можно будет добавить возможность изменения некоторых свойств.
                           .property("full_path_name", &get_full_path)
                           .property("full_filename", &get_full_filename)
                           .property("short_filename", &get_short_filename)
                           .property("extension", &get_extension)
                           .property("last_write_time", &get_last_write_time)
                           .property("last_write_time_string", &get_last_write_time_string)
                           .property("last_write_time_string_short", &get_last_write_time_string_short)
                           .def("exists", (bool(self::*)() const)(&self::exists))
                           .def("is_regular_file", (bool(self::*)() const)(&self::is_regular_file))
                           .def("is_directory", (bool(self::*)() const)(&self::is_directory))
                           .def("file_size", (uintmax_t(self::*)() const)(&self::file_size))];
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// SCRIPT C++17 FILESYSTEM - END ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#pragma optimize("s", on)
void fs_registrator::script_register(lua_State* L)
{
    //
    script_register_stdfs(L);
    //
    module(L)[class_<FS_item>("FS_item")
                  .def("NameFull", &FS_item::NameFull)
                  .def("NameShort", &FS_item::NameShort)
                  .def("Size", &FS_item::Size)
                  .def("ModifDigitOnly", &FS_item::ModifDigitOnly)
                  .def("Modif", &FS_item::Modif),

              class_<FS_file_list_ex>("FS_file_list_ex")
                  .def("Size", &FS_file_list_ex::Size)
                  .def("GetAt", &FS_file_list_ex::GetAt)
                  .def("Sort", &FS_file_list_ex::Sort)
                  .def( "GetAll", &FS_file_list_ex::GetAll, return_stl_iterator)
              ,

              class_<FS_file_list>("FS_file_list").def("Size", &FS_file_list::Size).def("GetAt", &FS_file_list::GetAt).def("Free", &FS_file_list::Free),

              /*		class_<FS_Path>("FS_Path")
                          .def_readonly("m_Path",						&FS_Path::m_Path)
                          .def_readonly("m_Root",						&FS_Path::m_Root)
                          .def_readonly("m_Add",						&FS_Path::m_Add)
                          .def_readonly("m_DefExt",					&FS_Path::m_DefExt)
                          .def_readonly("m_FilterCaption",			&FS_Path::m_FilterCaption),
              */
              class_<CLocatorAPI::file>("fs_file")
                  .def_readonly("name", &CLocatorAPI::file::name)
                  .def_readonly("vfs", &CLocatorAPI::file::vfs)
                  .def_readonly("ptr", &CLocatorAPI::file::ptr)
                  .def_readonly("size_real", &CLocatorAPI::file::size_real)
                  .def_readonly("size_compressed", &CLocatorAPI::file::size_compressed)
                  .def_readonly("modif", &CLocatorAPI::file::modif),

              class_<CLocatorAPI>("FS")
                  .enum_("FS_sort_mode")[value("FS_sort_by_name_up", int(FS_file_list_ex::eSortByNameUp)), value("FS_sort_by_name_down", int(FS_file_list_ex::eSortByNameDown)),
                                         value("FS_sort_by_size_up", int(FS_file_list_ex::eSortBySizeUp)), value("FS_sort_by_size_down", int(FS_file_list_ex::eSortBySizeDown)),
                                         value("FS_sort_by_modif_up", int(FS_file_list_ex::eSortByModifUp)), value("FS_sort_by_modif_down", int(FS_file_list_ex::eSortByModifDown))]
                  .enum_("FS_List")[value("FS_ListFiles", int(FS_ListFiles)), value("FS_ListFolders", int(FS_ListFolders)), value("FS_ClampExt", int(FS_ClampExt)),
                                    value("FS_RootOnly", int(FS_RootOnly)), value("FS_NoLower", int(FS_NoLower))]

                  .def("path_exist", &CLocatorAPI::path_exist)
                  .def("update_path", &update_path_script)
                  .def("get_path", &CLocatorAPI::get_path)
                  .def("append_path", &CLocatorAPI::append_path)

                  .def("file_delete", (void(CLocatorAPI::*)(LPCSTR, LPCSTR))(&CLocatorAPI::file_delete))
                  .def("file_delete", (void(CLocatorAPI::*)(LPCSTR))(&CLocatorAPI::file_delete))

                  //.def("dir_delete", &dir_delete_script)
                  //.def("dir_delete", &dir_delete_script_2)

                  .def("application_dir", &get_engine_dir)

                  .def("file_rename", &CLocatorAPI::file_rename)
                  .def("file_length", &CLocatorAPI::file_length)
                  .def("file_copy", &CLocatorAPI::file_copy)

                  .def("exist", (const CLocatorAPI::file* (CLocatorAPI::*)(LPCSTR))(&CLocatorAPI::exist))
                  .def("exist", (const CLocatorAPI::file* (CLocatorAPI::*)(LPCSTR, LPCSTR))(&CLocatorAPI::exist))

                  .def("get_file_age", &CLocatorAPI::get_file_age)
                  .def("get_file_age_str", &get_file_age_str)

                  .def("r_open", (IReader * (CLocatorAPI::*)(LPCSTR, LPCSTR))(&CLocatorAPI::r_open))
                  .def("r_open", (IReader * (CLocatorAPI::*)(LPCSTR))(&CLocatorAPI::r_open))
                  .def("r_close", (void(CLocatorAPI::*)(IReader*&))(&CLocatorAPI::r_close))

                  .def("w_open", (IWriter * (CLocatorAPI::*)(LPCSTR, LPCSTR))(&CLocatorAPI::w_open))
                  .def("w_close", &CLocatorAPI::w_close)

                  .def("file_list_open", &file_list_open_script)
                  .def("file_list_open", &file_list_open_script_2)
                  .def("file_list_open_ex", &file_list_open_ex),

              def("getFS", [] { return &FS; })];
}
