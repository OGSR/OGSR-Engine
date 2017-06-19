////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_storage.h"
#include "script_thread.h"
#include <stdarg.h>
#include "doug_lea_memory_allocator.h"

LPCSTR	file_header_old = "\
local function script_name() \
return \"%s\" \
end \
local this = {} \
%s this %s \
setmetatable(this, {__index = _G}) \
setfenv(1, this) \
		";

LPCSTR	file_header_new = "\
local function script_name() \
return \"%s\" \
end \
local this = {} \
this._G = _G \
%s this %s \
setfenv(1, this) \
		";

LPCSTR	file_header = 0;

#ifndef ENGINE_BUILD
#	include "script_engine.h"
#	include "ai_space.h"
#else
#	define NO_XRGAME_SCRIPT_ENGINE
#endif

#ifndef XRGAME_EXPORTS
#	define NO_XRGAME_SCRIPT_ENGINE
#endif

#ifndef NO_XRGAME_SCRIPT_ENGINE
#	include "ai_debug.h"
#endif

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

#ifndef PURE_ALLOC
#	ifndef USE_MEMORY_MONITOR
#		define USE_DL_ALLOCATOR
#	endif // USE_MEMORY_MONITOR
#endif // PURE_ALLOC

#ifndef USE_DL_ALLOCATOR
static void *lua_alloc_xr	(void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;
  if (nsize == 0) {
    xr_free	(ptr);
    return	NULL;
  }
  else
#ifdef DEBUG_MEMORY_NAME
    return Memory.mem_realloc		(ptr, nsize, "LUA");
#else // DEBUG_MEMORY_MANAGER
    return Memory.mem_realloc		(ptr, nsize);
#endif // DEBUG_MEMORY_MANAGER
}
#else // USE_DL_ALLOCATOR
static void *lua_alloc_dl	(void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;
  if (nsize == 0)	{	dlfree			(ptr);	 return	NULL;  }
  else				return dlrealloc	(ptr, nsize);
}

u32 game_lua_memory_usage	()
{
	return					((u32)dlmallinfo().uordblks);
}
#endif // USE_DL_ALLOCATOR

CScriptStorage::CScriptStorage		()
{
	m_current_thread		= 0;

#ifdef OGSE_DEBUG
	m_stack_is_ready		= false;
#endif // DEBUG
	
	m_virtual_machine		= 0;
}

CScriptStorage::~CScriptStorage		()
{
	if (m_virtual_machine)
		lua_close			(m_virtual_machine);
}

void CScriptStorage::reinit	()
{
	if (m_virtual_machine)
		lua_close			(m_virtual_machine);

#ifndef USE_DL_ALLOCATOR
	m_virtual_machine		= lua_newstate(lua_alloc_xr, NULL);
#else // USE_DL_ALLOCATOR
	m_virtual_machine		= lua_newstate(lua_alloc_dl, NULL);
#endif // USE_DL_ALLOCATOR

	if (!m_virtual_machine) {
		Msg					("! ERROR : Cannot initialize script virtual machine!");
		return;
	}
	
	luaL_openlibs(m_virtual_machine);	//RvP
	
	if (strstr(Core.Params,"-_g"))
		file_header			= file_header_new;
	else
		file_header			= file_header_old;
}

int CScriptStorage::vscript_log		(ScriptStorage::ELuaMessageType tLuaMessageType, LPCSTR caFormat, va_list marker)
{
#ifndef NO_XRGAME_SCRIPT_ENGINE
#	ifdef DEBUG
	if (!psAI_Flags.test(aiLua) && (tLuaMessageType != ScriptStorage::eLuaMessageTypeError))
		return(0);
#	endif
#endif

#ifndef OGSE_DEBUG
	return		(0);
#else // DEBUG

	LPCSTR		S = "", SS = "";
	LPSTR		S1;
	string4096	S2;
	switch (tLuaMessageType) {
		case ScriptStorage::eLuaMessageTypeInfo : {
			S	= "* [LUA] ";
			SS	= "[INFO]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeError : {
			S	= "! [LUA] ";
			SS	= "[ERROR]       ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeMessage : {
			S	= "[LUA] ";
			SS	= "[MESSAGE]     ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookCall : {
			S	= "[LUA][HOOK_CALL] ";
			SS	= "[CALL]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookReturn : {
			S	= "[LUA][HOOK_RETURN] ";
			SS	= "[RETURN]      ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookLine : {
			S	= "[LUA][HOOK_LINE] ";
			SS	= "[LINE]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookCount : {
			S	= "[LUA][HOOK_COUNT] ";
			SS	= "[COUNT]       ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookTailReturn : {
			S	= "[LUA][HOOK_TAIL_RETURN] ";
			SS	= "[TAIL_RETURN] ";
			break;
		}
		default : NODEFAULT;
	}
	
	strcpy	(S2,S);
	S1		= S2 + xr_strlen(S);
	int		l_iResult = vsprintf(S1,caFormat,marker);
	Msg		("%s",S2);
	
	strcpy	(S2,SS);
	S1		= S2 + xr_strlen(SS);
	vsprintf(S1,caFormat,marker);
	strcat	(S2,"\r\n");

#ifndef ENGINE_BUILD
#	ifdef OGSE_DEBUG
		ai().script_engine().m_output.w(S2,xr_strlen(S2)*sizeof(char));
#	endif // DEBUG
#endif // DEBUG

	return	(l_iResult);
#endif
}

#ifdef OGSE_DEBUG
void CScriptStorage::print_stack		()
{
	if (!m_stack_is_ready)
		return;

	m_stack_is_ready		= false;

	lua_State				*L = lua();
	lua_Debug				l_tDebugInfo;
	for (int i=0; lua_getstack(L,i,&l_tDebugInfo);++i ) {
		lua_getinfo			(L,"nSlu",&l_tDebugInfo);
		if (!l_tDebugInfo.name)
			script_log		(ScriptStorage::eLuaMessageTypeError,"%2d : [%s] %s(%d) : %s",i,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,"");
		else
			if (!xr_strcmp(l_tDebugInfo.what,"C"))
				script_log	(ScriptStorage::eLuaMessageTypeError,"%2d : [C  ] %s",i,l_tDebugInfo.name);
			else
				script_log	(ScriptStorage::eLuaMessageTypeError,"%2d : [%s] %s(%d) : %s",i,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
	}
}
#endif

int __cdecl CScriptStorage::script_log	(ScriptStorage::ELuaMessageType tLuaMessageType, LPCSTR caFormat, ...)
{

	va_list		marker;
	va_start	(marker,caFormat);
	int			result = vscript_log(tLuaMessageType,caFormat,marker);
	va_end		(marker);

#ifdef OGSE_DEBUG
#	ifndef ENGINE_BUILD
	static bool	reenterability = false;
	if (!reenterability) {
		reenterability = true;
		if (eLuaMessageTypeError == tLuaMessageType)
			ai().script_engine().print_stack	();
		reenterability = false;
	}
#	endif
#endif

	return		(result);
}

bool CScriptStorage::parse_namespace(LPCSTR caNamespaceName, LPSTR b, LPSTR c)
{
	strcpy			(b,"");
	strcpy			(c,"");
	LPSTR			S2	= xr_strdup(caNamespaceName);
	LPSTR			S	= S2;
	for (int i=0;;++i) {
		if (!xr_strlen(S)) {
			script_log	(ScriptStorage::eLuaMessageTypeError,"the namespace name %s is incorrect!",caNamespaceName);
			xr_free		(S2);
			return		(false);
		}
		LPSTR			S1 = strchr(S,'.');
		if (S1)
			*S1				= 0;

		if (i)
			strcat		(b,"{");
		strcat			(b,S);
		strcat			(b,"=");
		if (i)
			strcat		(c,"}");
		if (S1)
			S			= ++S1;
		else
			break;
	}
	xr_free			(S2);
	return			(true);
}

bool CScriptStorage::load_buffer	(lua_State *L, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName)
{
	int					l_iErrorCode;
	if (caNameSpaceName && xr_strcmp("_G",caNameSpaceName)) {
		string512		insert, a, b;

		LPCSTR			header = file_header;

		if (!parse_namespace(caNameSpaceName,a,b))
			return		(false);
		sprintf_s			(insert,header,caNameSpaceName,a,b);
		u32				str_len = xr_strlen(insert);
		LPSTR			script = xr_alloc<char>(str_len + tSize);
		strcpy			(script,insert);
		CopyMemory	(script + str_len,caBuffer,u32(tSize));
//		try 
		{
			l_iErrorCode= luaL_loadbuffer(L,script,tSize + str_len,caScriptName);
		}
//		catch(...) {
//			l_iErrorCode= LUA_ERRSYNTAX;
//		}
		xr_free			(script);
	}
	else {
//		try
		{
			l_iErrorCode= luaL_loadbuffer(L,caBuffer,tSize,caScriptName);
		}
//		catch(...) {
//			l_iErrorCode= LUA_ERRSYNTAX;
//		}
	}

	if (l_iErrorCode) {
#ifdef OGSE_DEBUG
		print_output(L,caScriptName,l_iErrorCode);
#endif
		return			(false);
	}
	return				(true);
}

bool CScriptStorage::do_file	(LPCSTR caScriptName, LPCSTR caNameSpaceName)
{
	int				start = lua_gettop(lua());
	string_path		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	if (!l_tpFileReader) {
		script_log	(eLuaMessageTypeError,"Cannot open file \"%s\"",caScriptName);
		return		(false);
	}
	strconcat		(sizeof(l_caLuaFileName),l_caLuaFileName,"@",caScriptName);
	
	if (!load_buffer(lua(),static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName,caNameSpaceName)) {
//		VERIFY		(lua_gettop(lua()) >= 4);
//		lua_pop		(lua(),4);
//		VERIFY		(lua_gettop(lua()) == start - 3);
		lua_settop	(lua(),start);
		FS.r_close	(l_tpFileReader);
		return		(false);
	}
	FS.r_close		(l_tpFileReader);

	int errFuncId = -1;
#ifdef USE_DEBUGGER
	if( ai().script_engine().debugger() )
	errFuncId = ai().script_engine().debugger()->PrepareLua(lua());
#endif
	if (0)	//.
	{
	    for (int i=0; lua_type(lua(), -i-1); i++)
            Msg	("%2d : %s",-i-1,lua_typename(lua(), lua_type(lua(), -i-1)));
	}

	// because that's the first and the only call of the main chunk - there is no point to compile it
//	luaJIT_setmode	(lua(),0,LUAJIT_MODE_ENGINE|LUAJIT_MODE_OFF);						// Oles
	int	l_iErrorCode = lua_pcall(lua(),0,0,(-1==errFuncId)?0:errFuncId);				// new_Andy
//	luaJIT_setmode	(lua(),0,LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);						// Oles

#ifdef USE_DEBUGGER
	if( ai().script_engine().debugger() )
		ai().script_engine().debugger()->UnPrepareLua(lua(),errFuncId);
#endif
	if (l_iErrorCode) {

#ifdef OGSE_DEBUG
		print_output(lua(),caScriptName,l_iErrorCode);
#endif
		lua_settop	(lua(),start);
		return		(false);
	}

	return			(true);
}

bool CScriptStorage::load_file_into_namespace(LPCSTR caScriptName, LPCSTR caNamespaceName)
{
	int				start = lua_gettop(lua());
	if (!do_file(caScriptName,caNamespaceName)) {
		lua_settop	(lua(),start);
		return		(false);
	}
	VERIFY			(lua_gettop(lua()) == start);
	return			(true);
}

bool CScriptStorage::namespace_loaded(LPCSTR N, bool remove_from_stack)
{
	int						start = lua_gettop(lua());
	lua_pushstring 			(lua(),"_G"); 
	lua_rawget 				(lua(),LUA_GLOBALSINDEX); 
	string256				S2;
	strcpy					(S2,N);
	LPSTR					S = S2;
	for (;;) { 
		if (!xr_strlen(S))
			return			(false); 
		LPSTR				S1 = strchr(S,'.'); 
		if (S1)
			*S1				= 0; 
		lua_pushstring 		(lua(),S); 
		lua_rawget 			(lua(),-2); 
		if (lua_isnil(lua(),-1)) { 
//			lua_settop		(lua(),0);
			VERIFY			(lua_gettop(lua()) >= 2);
			lua_pop			(lua(),2); 
			VERIFY			(start == lua_gettop(lua()));
			return			(false);	//	there is no namespace!
		}
		else 
			if (!lua_istable(lua(),-1)) { 
//				lua_settop	(lua(),0);
				VERIFY		(lua_gettop(lua()) >= 1);
				lua_pop		(lua(),1); 
				VERIFY		(start == lua_gettop(lua()));
				FATAL		(" Error : the namespace name is already being used by the non-table object!\n");
				return		(false); 
			} 
			lua_remove		(lua(),-2); 
			if (S1)
				S			= ++S1; 
			else
				break; 
	} 
	if (!remove_from_stack) {
		VERIFY				(lua_gettop(lua()) == start + 1);
	}
	else {
		VERIFY				(lua_gettop(lua()) >= 1);
		lua_pop				(lua(),1);
		VERIFY				(lua_gettop(lua()) == start);
	}
	return					(true); 
}

bool CScriptStorage::object	(LPCSTR identifier, int type)
{
	int						start = lua_gettop(lua());
	lua_pushnil				(lua()); 
	while (lua_next(lua(), -2)) { 
		if ((lua_type(lua(), -1) == type) && !xr_strcmp(identifier,lua_tostring(lua(), -2))) { 
			VERIFY			(lua_gettop(lua()) >= 3);
			lua_pop			(lua(), 3); 
			VERIFY			(lua_gettop(lua()) == start - 1);
			return			(true); 
		} 
		lua_pop				(lua(), 1); 
	} 
	VERIFY					(lua_gettop(lua()) >= 1);
	lua_pop					(lua(), 1); 
	VERIFY					(lua_gettop(lua()) == start - 1);
	return					(false); 
}

bool CScriptStorage::object	(LPCSTR namespace_name, LPCSTR identifier, int type)
{
	int						start = lua_gettop(lua());
	if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name,false)) {
		VERIFY				(lua_gettop(lua()) == start);
		return				(false); 
	}
	bool					result = object(identifier,type);
	VERIFY					(lua_gettop(lua()) == start);
	return					(result); 
}

luabind::object CScriptStorage::name_space(LPCSTR namespace_name)
{
	string256			S1;
	strcpy				(S1,namespace_name);
	LPSTR				S = S1;
	luabind::object		lua_namespace = luabind::get_globals(lua());
	for (;;) {
		if (!xr_strlen(S))
			return		(lua_namespace);
		LPSTR			I = strchr(S,'.');
		if (!I)
			return		(lua_namespace[S]);
		*I				= 0;
		lua_namespace	= lua_namespace[S];
		S				= I + 1;
	}
}

bool CScriptStorage::print_output(lua_State *L, LPCSTR caScriptFileName, int iErorCode)
{
	if (iErorCode)
		print_error		(L,iErorCode);

	if (!lua_isstring(L,-1))
		return			(false);

	LPCSTR				S = lua_tostring(L,-1);
	if (!xr_strcmp(S,"cannot resume dead coroutine")) {
		VERIFY2	("Please do not return any values from main!!!",caScriptFileName);
#ifdef USE_DEBUGGER
		if(ai().script_engine().debugger() && ai().script_engine().debugger()->Active() ){
			ai().script_engine().debugger()->Write(S);
			ai().script_engine().debugger()->ErrorBreak();
		}
#endif
	}
	else {
		if (!iErorCode)
			script_log	(ScriptStorage::eLuaMessageTypeInfo,"Output from %s",caScriptFileName);
		script_log		(iErorCode ? ScriptStorage::eLuaMessageTypeError : ScriptStorage::eLuaMessageTypeMessage,"%s",S);
#ifdef USE_DEBUGGER
		if (ai().script_engine().debugger() && ai().script_engine().debugger()->Active()) {
			ai().script_engine().debugger()->Write		(S);
			ai().script_engine().debugger()->ErrorBreak	();
		}
#endif
	}
	return				(true);
}

void CScriptStorage::print_error(lua_State *L, int iErrorCode)
{
	switch (iErrorCode) {
		case LUA_ERRRUN : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT RUNTIME ERROR");
			break;
		}
		case LUA_ERRMEM : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (memory allocation)");
			break;
		}
		case LUA_ERRERR : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (while running the error handler function)");
			break;
		}
		case LUA_ERRFILE : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (while running file)");
			break;
		}
		case LUA_ERRSYNTAX : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT SYNTAX ERROR");
			break;
		}
		case LUA_YIELD : {
			script_log (ScriptStorage::eLuaMessageTypeInfo,"Thread is yielded");
			break;
		}
		default : NODEFAULT;
	}
}

#ifdef OGSE_DEBUG
void CScriptStorage::flush_log()
{
	string_path			log_file_name;
	strconcat           (sizeof(log_file_name),log_file_name,Core.ApplicationName,"_",Core.UserName,"_lua.log");
	FS.update_path      (log_file_name,"$logs$",log_file_name);
	m_output.save_to	(log_file_name);
}
#endif // DEBUG
