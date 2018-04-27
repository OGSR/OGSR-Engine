////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_storage.h"
#include "script_engine.h"
#include "ai_space.h"

// KRodin: this не убирать ни в коем случае! Он нужен для того, чтобы классы luabind'а регистрировались внутри модуля в котором находятся, а не в _G
// см. luabind/src/create_class.cpp
static constexpr const char* FILE_HEADER = "\
local function script_name() \
return '%s' \
end; \
local this; \
module('%s', package.seeall, function(m) this = m end); \
%s";

const char* get_lua_traceback(lua_State *L)
{
#if LUAJIT_VERSION_NUM < 20000
	static char buffer[32768]; // global buffer
	int top = lua_gettop(L);
	// alpet: Lua traceback added
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushstring(L, "\t");
	lua_pushinteger(L, 1);

	const char *traceback = "cannot get Lua traceback ";
	strcpy_s(buffer, 32767, traceback);
	__try
	{
		if (0 == lua_pcall(L, 2, 1, 0))
		{
			traceback = lua_tostring(L, -1);
			strcpy_s(buffer, 32767, traceback);
			lua_pop(L, 1);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Msg("!#EXCEPTION(get_lua_traceback): buffer = %s ", buffer);
	}
	lua_settop(L, top);
	return buffer;
#else
	luaL_traceback(L, L, nullptr, 0);
	auto tb = lua_tostring(L, -1);
	lua_pop(L, 1);
	return tb;
#endif
}

//*********************************************************************************************
void CScriptStorage::dump_state()
{
	static bool reentrantGuard = false;
	if (reentrantGuard) return;
	reentrantGuard = true;

	lua_State				*L = lua();
	lua_Debug				l_tDebugInfo;
	for (int i = 0; lua_getstack(L, i, &l_tDebugInfo); ++i) {
		lua_getinfo(L, "nSlu", &l_tDebugInfo);
		Msg("\tLocals: ");
		const char *name = nullptr;
		int VarID = 1;
		while ((name = lua_getlocal(L, &l_tDebugInfo, VarID++)) != NULL)
		{
			LogVariable(L, name, 1);

			lua_pop(L, 1);  /* remove variable value */
		}
		Msg("\tEnd");
	}
	reentrantGuard = false;
}

void CScriptStorage::LogTable(lua_State *l, LPCSTR S, int level)
{
	if (!lua_istable(l, -1))
		return;

	lua_pushnil(l);  /* first key */
	while (lua_next(l, -2) != 0) {
		char sname[256];
		char sFullName[256];
		xr_sprintf(sname, "%s", lua_tostring(l, -2));
		xr_sprintf(sFullName, "%s.%s", S, sname);
		LogVariable(l, sFullName, level + 1);

		lua_pop(l, 1);  /* removes `value'; keeps `key' for next iteration */
	}
}

void CScriptStorage::LogVariable(lua_State * l, const char* name, int level)
{
	const char * type;
	int ntype = lua_type(l, -1);
	type = lua_typename(l, ntype);

	char tabBuffer[32] = { 0 };
	memset(tabBuffer, '\t', level);

	char value[128];

	switch (ntype)
	{
	case LUA_TFUNCTION:
		xr_strcpy(value, "[[function]]");
		break;

	case LUA_TTHREAD:
		xr_strcpy(value, "[[thread]]");
		break;
				
	case LUA_TNUMBER:
		xr_sprintf(value, "%f", lua_tonumber(l, -1));
		break;

	case LUA_TBOOLEAN:
		xr_sprintf(value, "%s", lua_toboolean(l, -1) ? "true" : "false");
		break;

	case LUA_TSTRING:
		xr_sprintf(value, "%.127s", lua_tostring(l, -1));
		break;

	case LUA_TTABLE:
		if (level <= 3)
		{
			Msg("%s Table: %s", tabBuffer, name);
			LogTable(l, name, level + 1);
			return;
		}
		else
		{
			xr_sprintf(value, "[...]");
		}
		break;

	case LUA_TUSERDATA:
	{
		luabind::detail::object_rep* obj = static_cast<luabind::detail::object_rep*>(lua_touserdata(l, -1));
		luabind::detail::lua_reference& r = obj->get_lua_table();
		if (r.is_valid())
		{
			r.get(l);
			Msg("%s Userdata: %s", tabBuffer, name);
			LogTable(l, name, level + 1);
			lua_pop(l, 1); //Remove userobject
			return;
		}
		else
		{
			xr_strcpy(value, "[not available]");
		}
	}
	break;

	default:
		xr_strcpy(value, "[not available]");
		break;
	}


	Msg("%s %s %s : %s", tabBuffer, type, name, value);
}
//*********************************************************************************************

void ScriptCrashHandler()
{
	try
	{
		Msg("*********************************************************************************");
		ai().script_engine().print_stack();
		ai().script_engine().dump_state();
		Msg("*********************************************************************************");
	}
	catch (...)
	{
		Msg("Can't dump script call stack - Engine corrupted");
	}
}


CScriptStorage::~CScriptStorage()
{
	//Msg("[CScriptStorage] Closing LuaJIT - start");
	if (m_virtual_machine)
		lua_close(m_virtual_machine); //Вот тут закрывается LuaJIT.
	//Msg("[CScriptStorage] Closing LuaJIT - end");
	Debug.set_crashhandler(nullptr);
}

void CScriptStorage::reinit(lua_State *LSVM)
{
	if (m_virtual_machine) //Как выяснилось, такое происходит при загрузке игры на этапе старта сервера 
	{
		//Msg("[CScriptStorage] Found working LuaJIT WM! Close it!");
		lua_close(m_virtual_machine);
	}
	m_virtual_machine = LSVM;

	Debug.set_crashhandler(ScriptCrashHandler);
}

void CScriptStorage::print_stack()
{
	Log(get_lua_traceback(lua()));
}

void CScriptStorage::script_log(ScriptStorage::ELuaMessageType tLuaMessageType, const char* caFormat, ...) //Используется в очень многих местах //Очень много пишет в лог.
{
#ifdef DEBUG
	va_list marker;
	va_start(marker, caFormat);
	//
	LPCSTR S = "";
	LPSTR S1;
	string4096 S2;
	switch (tLuaMessageType)
	{
	case ScriptStorage::eLuaMessageTypeInfo:
		S = "[LUA INFO]";
		break;
	case ScriptStorage::eLuaMessageTypeError:
		S = "[LUA ERROR]";
		break;
	case ScriptStorage::eLuaMessageTypeMessage:
		S = "[LUA MESSAGE]";
		break;
	case ScriptStorage::eLuaMessageTypeHookCall:
		S = "[LUA HOOK_CALL]";
		break;
	case ScriptStorage::eLuaMessageTypeHookReturn:
		S = "[LUA HOOK_RETURN]";
		break;
	case ScriptStorage::eLuaMessageTypeHookLine:
		S = "[LUA HOOK_LINE]";
		break;
	case ScriptStorage::eLuaMessageTypeHookCount:
		S = "[LUA HOOK_COUNT]";
		break;
	case ScriptStorage::eLuaMessageTypeHookTailReturn:
		S = "[LUA HOOK_TAIL_RETURN]";
		break;
	default: NODEFAULT;
	}
	xr_strcpy(S2, S);
	S1 = S2 + xr_strlen(S);
	vsprintf(S1, caFormat, marker);
	Msg("-----------------------------------------");
	Msg("[script_log] %s", S2);
	print_stack();
	Msg("-----------------------------------------");
	va_end(marker);
#endif
}

bool CScriptStorage::load_buffer(lua_State *L, const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName) //KRodin: эта функция форматирует содержимое скрипта используя FILE_HEADER и после этого загружает его в lua
{
	int l_iErrorCode = 0;
	if (strcmp(GlobalNamespace, caNameSpaceName)) //Все скрипты кроме _G
	{
		//KRodin: обращаться к _G только с большой буквы! Иначе он загрузится ещё раз и это неизвестно к чему приведёт!
		//Глобальное пространство инитится один раз после запуска луаджита, и никогда больше.
		if (!strcmp("_g", caNameSpaceName))
			return false;
		//KRodin: Переделал, т.к. в оригинале тут происходило нечто, на мой взгляд, странное.
		int buf_len = std::snprintf(nullptr, 0, FILE_HEADER, caNameSpaceName, caNameSpaceName, caBuffer);
		auto strBuf = std::make_unique<char[]>(buf_len + 1);
		std::snprintf(strBuf.get(), buf_len + 1, FILE_HEADER, caNameSpaceName, caNameSpaceName, caBuffer);
		//Log("[CScriptStorage::load_buffer(1)] Loading buffer:");
		//Log(strBuf.get());
		l_iErrorCode = luaL_loadbuffer(L, strBuf.get(), buf_len /*+ 1 Нуль-терминатор на конце мешает походу*/, caScriptName);
	}
	else //_G.script и только он.
	{
		//Log("[CScriptStorage::load_buffer(2)] Loading buffer:");
		//Log(caBuffer);
		l_iErrorCode = luaL_loadbuffer(L, caBuffer, tSize, caScriptName);
	}
	if (l_iErrorCode)
	{
		print_output(L, caScriptName, l_iErrorCode);
		R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
		return false;
	}
	return true;
}

bool CScriptStorage::do_file(const char* caScriptName, const char* caNameSpaceName) //KRodin: эта функция открывает скрипт с диска и оправляет его содержимое в функцию load_buffer, после этого походу запускает скрипт.
{
	string_path l_caLuaFileName;
	auto l_tpFileReader = FS.r_open(caScriptName);
	if (!l_tpFileReader) { //заменить на ассерт?
		Msg("!![CScriptStorage::do_file] Cannot open file [%s]", caScriptName);
		return false;
	}
	strconcat(sizeof(l_caLuaFileName), l_caLuaFileName, "@", caScriptName); //KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\***.script
	//
	//KRodin: исправлено. Теперь содержимое скрипта сразу читается нормально, без мусора на конце.
	auto strBuf = std::make_unique<char[]>(l_tpFileReader->length() + 1);
	strncpy(strBuf.get(), (const char*)l_tpFileReader->pointer(), l_tpFileReader->length());
	strBuf.get()[l_tpFileReader->length()] = 0;
	//
	bool loaded = load_buffer(lua(), strBuf.get(), (size_t)l_tpFileReader->length(), l_caLuaFileName, caNameSpaceName);
	FS.r_close(l_tpFileReader);
	if (!loaded)
		return false;

	int	l_iErrorCode = lua_pcall(lua(), 0, 0, 0); //KRodin: без этого скрипты не работают!
	if (l_iErrorCode)
	{
		print_output(lua(), caScriptName, l_iErrorCode);
		R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
		return false;
	}
	return true;
}

bool CScriptStorage::namespace_loaded(const char* name, bool remove_from_stack) //KRodin: видимо, функция проверяет, загружен ли скрипт.
{
#ifdef DEBUG
	int start = lua_gettop(lua());
#endif
	lua_pushstring(lua(), GlobalNamespace);
	lua_rawget(lua(), LUA_GLOBALSINDEX);
	string256 S2;
	xr_strcpy(S2, name);
	auto S = S2;
	for (;;)
	{
		if (!xr_strlen(S))
		{
			VERIFY(lua_gettop(lua()) >= 1);
			lua_pop(lua(), 1);
			VERIFY(start == lua_gettop(lua()));
			return false;
		}
		auto S1 = strchr(S, '.');
		if (S1)
			*S1 = 0;
		lua_pushstring(lua(), S);
		lua_rawget(lua(), -2);
		if (lua_isnil(lua(), -1))
		{
			//lua_settop(lua(),0);
			VERIFY(lua_gettop(lua()) >= 2);
			lua_pop(lua(), 2);
			VERIFY(start == lua_gettop(lua()));
			return false; //there is no namespace!
		}
		else if (!lua_istable(lua(), -1))
		{
			//lua_settop(lua(), 0);
			VERIFY(lua_gettop(lua()) >= 1);
			lua_pop(lua(), 1);
			VERIFY(start == lua_gettop(lua()));
			R_ASSERT3(false, "Error : the namespace is already being used by the non-table object! Name: ", S);
			return false;
		}
		lua_remove(lua(), -2);
		if (S1)
			S = ++S1;
		else
			break;
	}
	if (!remove_from_stack)
		VERIFY(lua_gettop(lua()) == start + 1);
	else
	{
		VERIFY(lua_gettop(lua()) >= 1);
		lua_pop(lua(), 1);
		VERIFY(lua_gettop(lua()) == start);
	}
	return true;
}

bool CScriptStorage::object(const char* identifier, int type)
{
#ifdef DEBUG
	int start = lua_gettop(lua());
#endif
	lua_pushnil(lua());
	while (lua_next(lua(), -2))
	{
		if (lua_type(lua(), -1) == type && !xr_strcmp(identifier, lua_tostring(lua(), -2)))
		{
			VERIFY(lua_gettop(lua()) >= 3);
			lua_pop(lua(), 3);
			VERIFY(lua_gettop(lua()) == start - 1);
			return true;
		}
		lua_pop(lua(), 1);
	}
	VERIFY(lua_gettop(lua()) >= 1);
	lua_pop(lua(), 1);
	VERIFY(lua_gettop(lua()) == start - 1);
	return false;
}

bool CScriptStorage::object(const char* namespace_name, const char* identifier, int type)
{
#ifdef DEBUG
	int start = lua_gettop(lua());
#endif
	if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name, false))
	{
		VERIFY(lua_gettop(lua()) == start);
		return false;
	}
	bool result = object(identifier, type);
	VERIFY(lua_gettop(lua()) == start);
	return result;
}

luabind::object CScriptStorage::name_space(const char* namespace_name)
{
	string256 S1;
	xr_strcpy(S1, namespace_name);
	auto S = S1;
#ifdef LUABIND_09
	auto lua_namespace = luabind::globals(lua());
#else
	auto lua_namespace = luabind::get_globals(lua());
#endif
	for (;;)
	{
		if (!xr_strlen(S))
			return lua_namespace;
		auto I = strchr(S, '.');
		if (!I)
#ifdef LUABIND_09
			return lua_namespace[(const char*)S];
#else
			return lua_namespace[S];
#endif
		*I = 0;
#ifdef LUABIND_09
		lua_namespace = lua_namespace[(const char*)S];
#else
		lua_namespace = lua_namespace[S];
#endif
		S = I + 1;
	}
}

bool CScriptStorage::print_output(lua_State *L, const char* caScriptFileName, int errorCode) //KRodin: вызывается из нескольких мест, в т.ч. из калбеков lua_error, lua_pcall_failed, lua_cast_failed, lua_panic
{
	auto Prefix = "";
	if (errorCode) {
		switch (errorCode) {
		case LUA_ERRRUN:
			Prefix = "SCRIPT RUNTIME ERROR";
			break;
		case LUA_ERRMEM:
			Prefix = "SCRIPT ERROR (memory allocation)";
			break;
		case LUA_ERRERR:
			Prefix = "SCRIPT ERROR (while running the error handler function)";
			break;
		case LUA_ERRFILE:
			Prefix = "SCRIPT ERROR (while running file)";
			break;
		case LUA_ERRSYNTAX:
			Prefix = "SCRIPT SYNTAX ERROR";
			break;
		case LUA_YIELD:
			Prefix = "Thread is yielded";
			break;
		default: NODEFAULT;
		}
	}

	auto traceback = get_lua_traceback(L);

	if (!lua_isstring(L, -1)) //НЕ УДАЛЯТЬ! Иначе будут вылeты без лога!
	{
		Msg("*********************************************************************************");
		Msg("[print_output(%s)] %s!\n%s", caScriptFileName, Prefix, traceback);
		Msg("*********************************************************************************");
		return false;
	}

	auto S = lua_tostring(L, -1);
	Msg("*********************************************************************************");
	Msg("[print_output(%s)] %s:\n%s\n%s", caScriptFileName, Prefix, S, traceback);
	Msg("*********************************************************************************");
	return true;
}
