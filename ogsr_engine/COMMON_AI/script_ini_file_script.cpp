////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file_script.cpp
//	Created 	: 25.06.2004
//  Modified 	: 25.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"

bool r_line(CInifile* self, LPCSTR S, int L, std::string& N, std::string& V)
{
    THROW3(self->section_exist(S), "Cannot find section", S);
    THROW2((int)self->line_count(S) > L, "Invalid line number");

    N = "";
    V = "";

    LPCSTR n, v;
    const bool result = !!self->r_line(S, L, &n, &v);
    if (!result)
        return (false);

    N = n;
    if (v)
        V = v;
    return (true);
}

void iterate_sections(CInifile* self, const luabind::functor<void>& functor)
{
    for (const auto& it : self->sections())
        functor(it.first.c_str());
}

CInifile* reload_system_ini()
{
    CInifile::Destroy(pSettings);

    CInifile* tmp{};
    string_path fname;

    FS.update_path(fname, fsgame::game_configs, "system_mods.ltx");
    if (FS.exist(fname))
    {
        tmp = xr_new<CInifile>(fname, TRUE, FALSE);

        tmp->load_file(TRUE);

        Msg("~ Apply system_mods.ltx...");
    }

    FS.update_path(fname, fsgame::game_configs, "system.ltx");
    pSettings = xr_new<CInifile>(fname, TRUE, FALSE);
    pSettings->load_file(FALSE, tmp);

    CHECK_OR_EXIT(!pSettings->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    if (tmp)
    {
        xr_delete(tmp);
    }

    return ((CInifile*)pSettings);
}

#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"

bool line_exist_script(CInifile* self, LPCSTR S, LPCSTR L) { return (!!self->line_exist(S, L)); }

bool section_exist_script(CInifile* self, LPCSTR S) { return (!!self->section_exist(S)); }

int r_clsid_script(CInifile* self, LPCSTR S, LPCSTR L) { return (object_factory().script_clsid(self->r_clsid(S, L))); }

bool r_bool_script(CInifile* self, LPCSTR S, LPCSTR L) { return (!!self->r_bool(S, L)); }

LPCSTR r_string_wb_script(CInifile* self, LPCSTR S, LPCSTR L) { return (self->r_string_wb(S, L).c_str()); }

inline CInifile* initialize_ini_file_full(LPCSTR szFileName, bool updatePath)
{
    LPCSTR path{szFileName};
    string_path path_upd{};
    if (updatePath)
    {
        FS.update_path(path_upd, fsgame::game_configs, szFileName);
        path = &path_upd[0];
    }

    if (IReader* F = FS.r_open(path))
    {
        // Костыль от ситуации когда в редких случаях почему-то у игроков бьётся создание новых файлов движком - оказывается набит нулями
        // Не понятно почему так происходит, поэтому сделал тут обработку такой ситуации.

        if (F->elapsed() >= sizeof(u8) && F->r_u8() == 0)
        {
            Msg("!![%s] file [%s] broken!", __FUNCTION__, path);

            FS.r_close(F);

            FS.file_delete(path);

            F = nullptr;
        }

        if (F)
            FS.r_close(F);
    }

    return xr_new<CInifile>(path, TRUE, TRUE, TRUE);
}

inline CInifile* initialize_ini_file(LPCSTR szFileName) { return initialize_ini_file_full(szFileName, true); }

using namespace luabind;

void CScriptIniFile::script_register(lua_State* L)
{
    module(L)[def("ini_file", &initialize_ini_file, adopt<result>()), def("ini_file", &initialize_ini_file_full, adopt<result>()),

              class_<CInifile>("__ini_file")
                  .def(constructor<LPCSTR>())
                  .def("section_exist", &section_exist_script)
                  .def("line_exist", &line_exist_script)
                  .def("line_count", (u32(CInifile::*)(LPCSTR)) & CInifile::line_count)
                  .def("remove_line", &CInifile::remove_line)
                  .def("remove_section", &CInifile::remove_section)
                  .def("get_as_string", &CInifile::get_as_string)
                  .def("save", &CInifile::save_as)
                  .def("name", &CInifile::fname)
                  .def("iterate_sections", &iterate_sections)

                  .def_readwrite("readonly", &CInifile::bReadOnly)

                  .def("r_line", &::r_line, pure_out_value<4>() + pure_out_value<5>())

                  .def("r_bool", &r_bool_script)
                  .def("r_string", (LPCSTR(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_string)
                  .def("r_u32", (u32(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u32)
                  .def("r_s32", (s32(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_s32)
                  .def("r_u16", (u16(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u16)
                  .def("r_s16", (s16(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_s16)
                  .def("r_u8", (u8(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u8)
                  .def("r_s8", (s8(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_s8)

                  .def("r_u32_hex", (u32(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u32_hex)
                  .def("r_u16_hex", (u16(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u16_hex)
                  .def("r_u8_hex", (u8(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_u8_hex)

                  .def("r_float", (float(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_float)
                  .def("r_vector2", (Fvector2(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_fvector2)
                  .def("r_vector", (Fvector3(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_fvector3)
                  .def("r_vector4", (Fvector4(CInifile::*)(LPCSTR, LPCSTR)) & CInifile::r_fvector4)

                  .def("r_clsid", &r_clsid_script)
                  .def("r_string_wq", &r_string_wb_script)

                  //.def("annotation", &CInifile::annotation)

                  .def("w_bool", &CInifile::w_bool)
                  .def("w_string", &CInifile::w_string)
                  .def("w_u32", &CInifile::w_u32)
                  .def("w_s32", &CInifile::w_s32)
                  .def("w_u16", &CInifile::w_u16)
                  .def("w_s16", &CInifile::w_s16)
                  .def("w_u8", &CInifile::w_u8)
                  .def("w_s8", &CInifile::w_s8)

                  .def("w_u32_hex", &CInifile::w_u32_hex)
                  .def("w_u16_hex", &CInifile::w_u16_hex)
                  .def("w_u8_hex", &CInifile::w_u8_hex)

                  .def("w_float", &CInifile::w_float)
                  .def("w_vector2", &CInifile::w_fvector2)
                  .def("w_vector", &CInifile::w_fvector3)
                  .def("w_vector4", &CInifile::w_fvector4),

              def("system_ini", [] { return pSettings; }), def("game_ini", [] { return pGameIni; }),
              def(
                  "create_ini_file", // чтение ini как текста, без возможности сохранить
                  [](const char* ini_string) {
                      IReader reader((void*)ini_string, strlen(ini_string));
                      return xr_new<CInifile>(&reader, FS.get_path(fsgame::game_configs)->m_Path);
                  },
                  adopt<result>()),
              def("reload_system_ini", &reload_system_ini)];
}
