////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file_script.cpp
//	Created 	: 25.06.2004
//  Modified 	: 25.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"

bool r_line(CScriptIniFile* self, LPCSTR S, int L, std::string& N, std::string& V)
{
    THROW3(self->section_exist(S), "Cannot find section", S);
    THROW2((int)self->line_count(S) > L, "Invalid line number");

    N = "";
    V = "";

    LPCSTR n, v;
    bool result = !!self->r_line(S, L, &n, &v);
    if (!result)
        return (false);

    N = n;
    if (v)
        V = v;
    return (true);
}

void iterate_sections(CScriptIniFile* self, const luabind::functor<void>& functor)
{
    for (const auto& it : self->sections())
        functor(it.first.c_str());
}

using namespace luabind;
#pragma optimize("s", on)
void CScriptIniFile::script_register(lua_State* L)
{
    module(L)[class_<CScriptIniFile>("ini_file")
                  .def(constructor<LPCSTR>())
                  .def(constructor<LPCSTR, bool>())
                  .def("section_exist", &CScriptIniFile::section_exist)
                  .def("line_exist", &CScriptIniFile::line_exist)
                  .def("line_count", &CScriptIniFile::line_count)
                  .def("remove_line", &CScriptIniFile::remove_line)
                  .def("remove_section", &CScriptIniFile::remove_section)
                  .def("get_as_string", &CScriptIniFile::get_as_string)
                  .def("save", &CScriptIniFile::save_as)
                  .def("iterate_sections", &iterate_sections)

                  .def_readwrite("readonly", &CScriptIniFile::bReadOnly)

                  .def("r_clsid", &CScriptIniFile::r_clsid)
                  .def("r_bool", &CScriptIniFile::r_bool)
                  .def("r_string_wq", &CScriptIniFile::r_string_wb)
                  .def("r_string", &CScriptIniFile::r_string)
                  .def("r_u32", &CScriptIniFile::r_u32)
                  .def("r_s32", &CScriptIniFile::r_s32)
                  .def("r_float", &CScriptIniFile::r_float)
                  .def("r_vector", &CScriptIniFile::r_fvector3)
			      .def("r_line",			&::r_line, out_value<4>() + out_value<5>())

        .def("w_bool", &CScriptIniFile::w_bool)
                  .def("w_string", &CScriptIniFile::w_string)
                  .def("w_u32", &CScriptIniFile::w_u32)
                  .def("w_s32", &CScriptIniFile::w_s32)
                  .def("w_float", &CScriptIniFile::w_float)
                  .def("w_vector", &CScriptIniFile::w_fvector3),

              def("system_ini", [] { return reinterpret_cast<CScriptIniFile*>(pSettings); }), 
              def("game_ini", [] { return reinterpret_cast<CScriptIniFile*>(pGameIni); }),
              def(
                  "create_ini_file", // чтение ini как текста, без возможности сохранить
                  [](const char* ini_string) {
                      IReader reader((void*)ini_string, strlen(ini_string));
                      return static_cast<CScriptIniFile*>(xr_new<CInifile>(&reader, FS.get_path("$game_config$")->m_Path));
                  },
                  adopt<result>())
    ];
}
