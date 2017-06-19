#pragma once

// #define USE_NATIVE_LUA_STRINGS

#ifndef USE_NATIVE_LUA_STRINGS
#else
#	include "lobject.h"

//#	define DEBUG_NATIVE_LUA_STRINGS

//LUABIND_API 	int		luaL_ref				(lua_State *L, int t);
//LUABIND_API 	void	luaL_unref				(lua_State *L, int t, int ref);
extern "C" 	{
LUA_API 	void		lua_push_string_object		(lua_State *L, TString *string);
LUA_API 	int			luaV_strcmp					(const TString *ls, const TString *rs);
LUA_API 	TString	*	lua_tostring_object			(lua_State *L, int idx);
LUA_API 	TString *	luaS_newlstr				(lua_State *L, const char *str, size_t l);
};

class lua_string_holder {
private:
	lua_State		*m_state;
	int				m_reference;

public:
	TString			*m_object;

public:
	inline	lua_string_holder	(lua_State *state, LPCSTR string)
	{
		m_state					= state;
		VERIFY					(m_state);

		m_object				= luaS_newlstr(m_state,string,xr_strlen(string));
		VERIFY					(m_object);

		reference				();
	}

	inline	lua_string_holder	(lua_State *state, TString *object)
	{
		m_state					= state;
		VERIFY					(m_state);

		m_object				= object;
		VERIFY					(m_object);

		reference				();
	}

	inline	lua_string_holder	(TString *object)
	{
		m_state					= 0;
		m_object				= object;
		m_reference				= LUA_REGISTRYINDEX;
		VERIFY					(m_object);
	}

	inline	lua_string_holder	(const lua_string_holder &object)
	{
		*this					= object;
	}

	inline	lua_string_holder&	operator=	(const lua_string_holder &object)
	{
		m_state					= object.m_state;
		VERIFY					(m_state);

		m_object				= object.m_object;
		VERIFY					(m_object);

		reference				();

		return					(*this);
	}

	inline	~lua_string_holder	()
	{
		if (!m_state) {
			VERIFY				(m_object);
			VERIFY				(m_reference == LUA_REGISTRYINDEX);
			return;
		}
#ifdef DEBUG_NATIVE_LUA_STRINGS
		printf					("- 0x%08x\n",*(int*)(void**)&m_object);
#endif
		VERIFY					(m_state);
		luaL_unref				(m_state,LUA_REGISTRYINDEX,m_reference);
	}

	inline	void	reference	()
	{
#ifdef DEBUG_NATIVE_LUA_STRINGS
		printf					("+ 0x%08x\n",*(int*)(void**)&m_object);
#endif
		VERIFY					(m_state);
		VERIFY					(m_object);
		lua_push_string_object	(m_state,m_object);
		m_reference				= luaL_ref(m_state,LUA_REGISTRYINDEX);
	}
};

struct TString_hash_compare {
	const static size_t bucket_size = 4;
	const static size_t min_buckets = 8;

	inline	size_t	operator()	(const lua_string_holder &str) const
	{
		return		(str.m_object->tsv.hash);
	}
	
	inline	bool	operator()	(const lua_string_holder &str0, const lua_string_holder &str1) const
	{
		return		(luaV_strcmp(str0.m_object,str1.m_object) < 0);
	}
};

#endif