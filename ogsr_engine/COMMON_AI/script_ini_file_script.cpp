////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file_script.cpp
//	Created 	: 25.06.2004
//  Modified 	: 25.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"
#include <Utils\cdecl_cast.hpp>

bool r_line(CScriptIniFile *self, LPCSTR S, int L, std::string &N, std::string &V)
{
	THROW3			(self->section_exist(S),"Cannot find section",S);
	THROW2			((int)self->line_count(S) > L,"Invalid line number");
	
	N				= "";
	V				= "";
	
	LPCSTR			n,v;
	bool			result = !!self->r_line(S,L,&n,&v);
	if (!result)
		return		(false);

	N				= n;
	if (v)
		V			= v;
	return			(true);
}

void iterate_sections(CScriptIniFile *self, const luabind::functor<void>& functor)
{
	for (const auto& it : self->sections())
		functor(it.first.c_str());
}

using namespace luabind;
#pragma optimize("s",on)
void CScriptIniFile::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptIniFile>("ini_file")
			.def(					constructor<LPCSTR>())
			.def("section_exist",	&CScriptIniFile::section_exist	)
			.def("line_exist",		&CScriptIniFile::line_exist		)
			.def("r_clsid",			&CScriptIniFile::r_clsid		)
			.def("r_bool",			&CScriptIniFile::r_bool			)
			.def("r_token",			&CScriptIniFile::r_token		)
			.def("r_string_wq",		&CScriptIniFile::r_string_wb	)
			.def("line_count",		&CScriptIniFile::line_count)
			.def("r_string",		&CScriptIniFile::r_string)
			.def("r_u32",			&CScriptIniFile::r_u32)
			.def("r_s32",			&CScriptIniFile::r_s32)
			.def("r_float",			&CScriptIniFile::r_float)
			.def("r_vector",		&CScriptIniFile::r_fvector3)
			.def_readwrite( "readonly", &CScriptIniFile::bReadOnly )
			.def( "remove_line", &CScriptIniFile::remove_line )
			.def( "remove_section", &CScriptIniFile::remove_section )
			.def( "w_bool",   &CScriptIniFile::w_bool )
			.def( "w_string", &CScriptIniFile::w_string )
			.def( "w_u32",    &CScriptIniFile::w_u32 )
			.def( "w_s32",    &CScriptIniFile::w_s32 )
			.def( "w_float",  &CScriptIniFile::w_float )
			.def( "w_vector", &CScriptIniFile::w_fvector3 )
			.def( "get_as_string", &CScriptIniFile::get_as_string )
#ifdef LUABIND_09
			.def("r_line",			&::r_line, out_value(_4) + out_value(_5))
#else
			.def("r_line",			&::r_line, out_value<4>() + out_value<5>())
#endif
            .def("iterate_sections", &iterate_sections)
		,
#pragma warning(push)
#pragma warning(disable:4238 4239)
		def("system_ini", cdecl_cast([] { return reinterpret_cast<CScriptIniFile*>(pSettings); })),
		def("game_ini",   cdecl_cast([] { return reinterpret_cast<CScriptIniFile*>(pGameIni);  })),
		def("create_ini_file", cdecl_cast([](const char* ini_string) {
			return static_cast<CScriptIniFile*>(
				new CInifile(
					&IReader((void*)ini_string, strlen(ini_string)),
					FS.get_path("$game_config$")->m_Path
				)
			);
		}), adopt<result>())
#pragma warning(pop)
	];
}
