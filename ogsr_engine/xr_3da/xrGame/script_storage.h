////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage_space.h"
#include "script_space_forward.h"

struct lua_State;
class CScriptThread;

#ifdef	OGSE_DEBUG
//	#ifndef ENGINE_BUILD
	#	define	USE_DEBUGGER
//	#endif
#endif

using namespace ScriptStorage;

class CScriptStorage {
private:
	lua_State					*m_virtual_machine	;
	CScriptThread				*m_current_thread	;
	BOOL						m_jit				;

#ifdef OGSE_DEBUG
public:
	bool						m_stack_is_ready	;
#endif

#ifdef OGSE_DEBUG
protected:
	CMemoryWriter				m_output;
#endif // DEBUG

protected:
	static	int					vscript_log					(ScriptStorage::ELuaMessageType tLuaMessageType, LPCSTR caFormat, va_list marker);
			bool				parse_namespace				(LPCSTR caNamespaceName, LPSTR b, LPSTR c);
			bool				do_file						(LPCSTR	caScriptName, LPCSTR caNameSpaceName);
			void				reinit						();

public:
#ifdef OGSE_DEBUG
			void				print_stack					();
#endif

public:
								CScriptStorage				();
	virtual						~CScriptStorage				();
	IC		lua_State			*lua						();
	IC		void				current_thread				(CScriptThread *thread);
	IC		CScriptThread		*current_thread				() const;
			bool				load_buffer					(lua_State *L, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName = 0);
			bool				load_file_into_namespace	(LPCSTR	caScriptName, LPCSTR caNamespaceName);
			bool				namespace_loaded			(LPCSTR	caName, bool remove_from_stack = true);
			bool				object						(LPCSTR	caIdentifier, int type);
			bool				object						(LPCSTR	caNamespaceName, LPCSTR	caIdentifier, int type);
			luabind::object		name_space					(LPCSTR	namespace_name);
	static	int		__cdecl		script_log					(ELuaMessageType message,	LPCSTR	caFormat, ...);
	static	bool				print_output				(lua_State *L,		LPCSTR	caScriptName,		int		iErorCode = 0);
	static	void				print_error					(lua_State *L,		int		iErrorCode);

#ifdef OGSE_DEBUG
public:
			void				flush_log					();
#endif // DEBUG
};

#include "script_storage_inline.h"