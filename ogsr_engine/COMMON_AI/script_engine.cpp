////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"

void export_classes(lua_State *L);

CScriptEngine::CScriptEngine()
{
	//KRodin: luabind_allocator инитится в ResourceManager_Scripting.cpp, т.к там luabind инитится раньше всего.
	//
	m_stack_level = 0;
	m_reload_modules = false;
	m_last_no_file_cnt    = 0;
	m_last_no_file_length = 0;
	*m_last_no_file = 0;
}

void CScriptEngine::unload()
{
	lua_settop(lua(), m_stack_level);
	m_last_no_file_cnt    = 0;
	m_last_no_file_length = 0;
	*m_last_no_file = 0;
}


#define DEF_LUA_ERROR_TEMPLATE( L )                  \
print_output( L, "[" __FUNCTION__ "]", LUA_ERRRUN ); \
FATAL( "[%s]: %s", __FUNCTION__, lua_isstring( L, -1 ) ? lua_tostring( L, -1 ) : "" );

int CScriptEngine::lua_panic( lua_State* L ) {
  DEF_LUA_ERROR_TEMPLATE( L )
  return 0;
}

#ifdef LUABIND_NO_EXCEPTIONS
void CScriptEngine::lua_error( lua_State* L ) {
  DEF_LUA_ERROR_TEMPLATE( L )
}
#endif

int CScriptEngine::lua_pcall_failed( lua_State* L ) {
  DEF_LUA_ERROR_TEMPLATE( L )
  if ( lua_isstring( L, -1 ) )
    lua_pop( L, 1 );
  return LUA_ERRRUN;
}


#ifdef LUABIND_NO_EXCEPTIONS
#ifdef LUABIND_09
void lua_cast_failed( lua_State* L, const luabind::type_id& info )
#else
void lua_cast_failed( lua_State* L, LUABIND_TYPE_INFO info )
#endif
{
  CScriptEngine::print_output( L, "[" __FUNCTION__ "]", LUA_ERRRUN );
#ifdef LUABIND_09
  const char* info_name = info.name();
#else
  const char* info_name = info->name();
#endif
  Msg( "!![%s] LUA error: cannot cast lua value to [%s]", __FUNCTION__, info_name );
  //FATAL("[%s] LUA error: cannot cast lua value to [%s]", __FUNCTION__, info_name); //KRodin: Тут наверное вылетать не надо.
}
#endif

int auto_load(lua_State *L)
{
	if ((lua_gettop(L) < 2) || !lua_istable(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushnil(L);
		return 1;
	}
	ai().script_engine().process_file_if_exists(lua_tostring(L, 2), false);
	lua_rawget(L, 1);
	return 1;
}


using script_list_type = std::map<std::string, std::string>;
static script_list_type xray_scripts;

void CScriptEngine::setup_auto_load()
{
	lua_pushstring(lua(), GlobalNamespace);
	lua_gettable(lua(), LUA_GLOBALSINDEX);
	int value_index = lua_gettop(lua());  // alpet: во избежания оставления в стеке лишней метатаблицы
	luaL_newmetatable(lua(), "XRAY_AutoLoadMetaTable");
	lua_pushstring(lua(), "__index");
	lua_pushcfunction(lua(), auto_load);
	lua_settable(lua(), -3);
	// luaL_getmetatable(lua(), "XRAY_AutoLoadMetaTable");
	lua_setmetatable(lua(), value_index);

	xray_scripts.clear();

#ifdef LOAD_SCRIPTS_SUBDIRS
	FS_FileSet fset;
	FS.file_list(fset, "$game_scripts$", FS_ListFiles, "*.script");
	FS_FileSetIt fit = fset.begin();
	FS_FileSetIt fit_e = fset.end();

	for (; fit != fit_e; ++fit)
	{
		string_path	fn1, fn2;
		_splitpath((*fit).name.c_str(), 0, fn1, fn2, 0);

		FS.update_path(fn1, "$game_scripts$", fn1);
		strconcat(sizeof(fn1), fn1, fn1, fn2, ".script");

		xray_scripts.emplace(fn2, fn1);
	}
#endif
}

void CScriptEngine::init()
{
	//Msg("[CScriptEngine::init] Starting LuaJIT!");
	lua_State* LSVM = luaL_newstate(); //Запускаем LuaJIT. Память себе он выделит сам.
	R_ASSERT2(LSVM, "! ERROR : Cannot initialize LUA VM!");
	reinit(LSVM);
#ifdef LUABIND_09
	luabind::disable_super_deprecation();
#endif
	luabind::open(LSVM); //Запуск луабинда
	//--------------Установка калбеков------------------//
#ifdef LUABIND_NO_EXCEPTIONS
	luabind::set_error_callback(lua_error);
	luabind::set_cast_failed_callback(lua_cast_failed);
#endif
	luabind::set_pcall_callback(lua_pcall_failed); //KRodin: НЕ ЗАКОММЕНТИРОВАТЬ НИ В КОЕМ СЛУЧАЕ!!!
	lua_atpanic(LSVM, lua_panic);
	//-----------------------------------------------------//
	export_classes(LSVM); //Тут регистрируются все движковые функции, импортированные в скрипты
	luaL_openlibs(LSVM); //Инициализация функций LuaJIT
	setup_auto_load(); //Построение метатаблицы
	bool save = m_reload_modules;
	m_reload_modules = true;
	process_file_if_exists(GlobalNamespace, false); //Компиляция _G.script
	m_reload_modules = save;

	m_stack_level = lua_gettop(LSVM); //?

	register_script_classes(); //Походу, запуск class_registrator.script
	object_factory().register_script(); //Регистрация классов
	//Msg("[CScriptEngine::init] LuaJIT Started!");
}

void CScriptEngine::parse_script_namespace(const char *name, char *ns, u32 nsSize, char *func, u32 funcSize)
{
	auto p = strrchr(name, '.');
	if (!p)
	{
		xr_strcpy(ns, nsSize, GlobalNamespace);
		p = name - 1;
	}
	else
	{
		VERIFY(u32(p - name + 1) <= nsSize);
		strncpy(ns, name, p - name);
		ns[p - name] = 0;
	}
	xr_strcpy(func, funcSize, p + 1);
}


#ifdef LOAD_SCRIPTS_SUBDIRS
// KRodin: Функция проверяет существует ли скрипт на диске. Если
// существует - отправляет его в do_file. Вызывается из process_file,
// auto_load и не только.
bool CScriptEngine::process_file_if_exists( const char* file_name, bool warn_if_not_exist ) {
  if ( !m_reload_modules && ( *file_name && namespace_loaded( file_name ) ) )
    return true;

  u32 string_length = xr_strlen( file_name );
  // Это походу для оптимизации только, чтоб типа если один раз
  // убедились что файла нет, постоянно не проверять, есть ли он.
  if ( !warn_if_not_exist && no_file_exists( file_name, string_length ) ) {
    ++m_last_no_file_cnt;
    return false;
  }

  script_list_type::iterator it = xray_scripts.find( *file_name ? ( strcmp( file_name, GlobalNamespace ) == 0 ? "_g" : file_name ) : "_g" );
  if ( it != xray_scripts.end() ) {
    MsgDbg( "* loading script %s.script", file_name );
    m_reload_modules = false;
    return do_file( it->second.c_str(), *file_name ? file_name : GlobalNamespace );
  }

  if ( warn_if_not_exist )
    MsgDbg( "[CScriptEngine::process_file_if_exists] Variable %s not found; No script by this name exists, either.", file_name );
  else {
    LogDbg( "-------------------------" );
    MsgDbg( "[CScriptEngine::process_file_if_exists] Variable %s not found; No script by this name exists, either.", file_name );
    FuncDbg(print_stack());
    LogDbg( "-------------------------" );
    add_no_file( file_name, string_length );
  }

  return false;
}


#else  // #ifdef LOAD_SCRIPTS_SUBDIRS
const char* ExtractFileName(const char* fname)
{
	const char* result = fname;
	for (size_t c = 0; c < strlen(fname); c++)
		if (fname[c] == '\\') result = &fname[c + 1];
	return result;
}

void CollectScriptFiles(script_list_type &map, const char* path)
{
	if (!strlen(path))
		return;
	string_path fname;
	auto folders = FS.file_list_open(path, FS_ListFolders);
	if (folders)
	{
		std::for_each(folders->begin(), folders->end(), [&](const char* folder)
		{
			if (strchr(folder, '.'))
			{
				strconcat(sizeof(fname), fname, path, folder);
				CollectScriptFiles(map, fname);
			}
		});
		FS.file_list_close(folders);
	}

	string_path buff;
	auto files = FS.file_list_open(path, FS_ListFiles);
	if (!files)
		return;
	std::for_each(files->begin(), files->end(), [&](const char* file)
	{
		strconcat(sizeof(fname), fname, path, file);
		if ((strstr(fname, ".script") /*|| strstr(fname, ".lua")*/) && FS.exist(fname))
		{
			const char* fstart = ExtractFileName(fname);
			strcpy_s(buff, fstart);
			_strlwr_s(buff);
			const char* nspace = strtok(buff, ".");
			map.emplace(nspace, fname);
		}
	});
	FS.file_list_close(files);
}

bool LookupScript(string_path &fname, const char* base)
{
	string_path lc_base;
	if (xray_scripts.empty())
	{
		FS.update_path(lc_base, "$game_scripts$", "");
		CollectScriptFiles(xray_scripts, lc_base);
	}
	strcpy_s(lc_base, base);
	_strlwr_s(lc_base);
	auto it = xray_scripts.find(lc_base);
	if (it != xray_scripts.end())
	{
		strcpy_s(fname, it->second.c_str());
		return true;
	}
	return false;
}

bool CScriptEngine::process_file_if_exists(const char* file_name, bool warn_if_not_exist) //KRodin: Функция проверяет существует ли скрипт на диске. Если существует - отправляет его в do_file. Вызывается из process_file, auto_load и не только.
{
	u32 string_length = strlen(file_name);
	if (!warn_if_not_exist && no_file_exists(file_name, string_length)) //Это походу для оптимизации только, чтоб типа если один раз убедились что файла нет, постоянно не проверять, есть ли он.
		return false;
	if (m_reload_modules || (*file_name && !namespace_loaded(file_name)))
	{
		string_path S;
		if (!LookupScript(S, file_name))
		{
			if (warn_if_not_exist)
				MsgDbg("[CScriptEngine::process_file_if_exists] Variable %s not found; No script by this name exists, either.", file_name);
			else
			{
				LogDbg("-------------------------");
				MsgDbg("[CScriptEngine::process_file_if_exists] WARNING: Access to nonexistent variable or loading nonexistent script '%s'", file_name);
				FuncDbg(print_stack());
				LogDbg("-------------------------");
				add_no_file(file_name, string_length);
			}
			return false;
		}
#ifdef DEBUG
		MsgDbg("[CScriptEngine::process_file_if_exists] loading script: [%s]", file_name);
#endif
		m_reload_modules = false;
		return do_file(S, file_name);
	}
	return true;
}
#endif  // #ifdef LOAD_SCRIPTS_SUBDIRS


bool CScriptEngine::process_file(const char* file_name)
{
	return process_file_if_exists(file_name, true);
}

bool CScriptEngine::process_file(const char* file_name, bool reload_modules)
{
	m_reload_modules = reload_modules;
	bool result = process_file_if_exists(file_name, true);
	m_reload_modules = false;
	return result;
}

void CScriptEngine::register_script_classes() {
  luabind::functor<void> result;
  ASSERT_FMT( functor( "class_registrator.register", result ), "[%s] Cannot load class_registrator!", __FUNCTION__ );
  result( const_cast<CObjectFactory*>( &object_factory() ) );
}

bool CScriptEngine::function_object(const char* function_to_call, luabind::object &object, int type)
{
	if (!strlen(function_to_call))
		return false;
	string256 name_space, function;
	parse_script_namespace(function_to_call, name_space, sizeof(name_space), function, sizeof(function));
	if (xr_strcmp(name_space, GlobalNamespace))
	{
		auto file_name = strchr(name_space, '.');
		if (!file_name)
			process_file(name_space);
		else
		{
			*file_name = 0;
			process_file(name_space);
			*file_name = '.';
		}
	}

	if (!this->object(name_space, function, type))
		return false;
	auto lua_namespace = this->name_space(name_space);
	object = lua_namespace[function];
	return true;
}

bool CScriptEngine::no_file_exists(const char* file_name, u32 string_length)
{
	if (m_last_no_file_length != string_length)
		return false;
	return !memcmp(m_last_no_file, file_name, string_length);
}

void CScriptEngine::add_no_file(const char* file_name, u32 string_length)
{
	m_last_no_file_cnt    = 0;
	m_last_no_file_length = string_length;
	std::memcpy(m_last_no_file, file_name, string_length + 1);
}

void CScriptEngine::collect_all_garbage()
{
	lua_gc(lua(), LUA_GCCOLLECT, 0);
	lua_gc(lua(), LUA_GCCOLLECT, 0);
	lua_gc(lua(), LUA_GCCOLLECT, 0);
}
