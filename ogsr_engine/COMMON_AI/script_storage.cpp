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
#include <format>

// KRodin: this не убирать ни в коем случае! Он нужен для того, чтобы классы luabind'а регистрировались внутри модуля в котором находятся, а не в _G
// см. luabind/src/create_class.cpp
static constexpr const char* FILE_HEADER =
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}";

const char* get_lua_traceback(lua_State* L)
{
    luaL_traceback(L, L, nullptr, 0);
    auto tb = lua_tostring(L, -1);
    lua_pop(L, 1);
    return tb;
}

//*********************************************************************************************
void CScriptStorage::dump_state()
{
    static bool reentrantGuard = false;
    if (reentrantGuard)
        return;
    reentrantGuard = true;

    lua_State* L = lua();
    lua_Debug l_tDebugInfo;
    for (int i = 0; lua_getstack(L, i, &l_tDebugInfo); ++i)
    {
        lua_getinfo(L, "nSlu", &l_tDebugInfo);
        Msg("\tLocals: ");
        const char* name = nullptr;
        int VarID = 1;
        while ((name = lua_getlocal(L, &l_tDebugInfo, VarID++)) != NULL)
        {
            LogVariable(L, name, 1);

            lua_pop(L, 1); /* remove variable value */
        }
        m_dumpedObjList.clear();
        Msg("\tEnd");
    }
    reentrantGuard = false;
}

void CScriptStorage::LogTable(lua_State* l, LPCSTR S, int level)
{
    if (!lua_istable(l, -1))
        return;

    lua_pushnil(l); /* first key */
    while (lua_next(l, -2) != 0)
    {
        char sname[256];
        char sFullName[256];
        xr_sprintf(sname, "%s", lua_tostring(l, -2));
        xr_sprintf(sFullName, "%s.%s", S, sname);
        LogVariable(l, sFullName, level + 1);

        lua_pop(l, 1); /* removes `value'; keeps `key' for next iteration */
    }
}

void CScriptStorage::LogVariable(lua_State* l, const char* name, int level)
{
    const char* type;
    int ntype = lua_type(l, -1);
    type = lua_typename(l, ntype);

    auto tabBuffer = std::make_unique<char[]>(level + 1);
    memset(tabBuffer.get(), '\t', level);

    char value[128];

    switch (ntype)
    {
    case LUA_TFUNCTION: xr_strcpy(value, "[[function]]"); break;

    case LUA_TTHREAD: xr_strcpy(value, "[[thread]]"); break;

    case LUA_TNUMBER: xr_sprintf(value, "%f", lua_tonumber(l, -1)); break;

    case LUA_TBOOLEAN: xr_sprintf(value, "%s", lua_toboolean(l, -1) ? "true" : "false"); break;

    case LUA_TSTRING: xr_sprintf(value, "%.127s", lua_tostring(l, -1)); break;

    case LUA_TTABLE:
        if (level <= 3)
        {
            Msg("%s Table: %s", tabBuffer.get(), name);
            LogTable(l, name, level + 1);
            return;
        }
        else
        {
            xr_sprintf(value, "[...]");
        }
        break;

    case LUA_TUSERDATA: {
        auto obj = static_cast<luabind::detail::object_rep*>(lua_touserdata(l, -1));

        // Skip already dumped object
        if (m_dumpedObjList.find(obj) != m_dumpedObjList.end())
            return;
        m_dumpedObjList.insert(obj);

        auto& r = obj->get_lua_table();
        if (r.is_valid())
        {
            r.get(l);
            Msg("%s Userdata: %s", tabBuffer.get(), name);
            LogTable(l, name, level + 1);
            lua_pop(l, 1); // Remove userobject
            return;
        }
        else
        {
            // Dump class and element pointer if available
            if (const auto objectClass = obj->crep())
            {
                auto cpp_name = objectClass->type()->name();

                xr_sprintf(value, "(%s): %p", cpp_name ? cpp_name : objectClass->name(), obj->ptr());
            }
            else
                xr_strcpy(value, "[not available]");
        }
    }
    break;

    default: xr_strcpy(value, "[not available]"); break;
    }

    Msg("%s %s %s : %s", tabBuffer.get(), type, name, value);
}
//*********************************************************************************************

static void ScriptCrashHandler(bool dump_lua_locals)
{
    if (!Device.OnMainThread())
        return;

    try
    {
        Msg("***************************[ScriptCrashHandler]**********************************");
        ai().script_engine().print_stack();
        if (dump_lua_locals)
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
    // Msg("[CScriptStorage] Closing LuaJIT - start");
    if (m_virtual_machine)
        lua_close(m_virtual_machine); //Вот тут закрывается LuaJIT.
    // Msg("[CScriptStorage] Closing LuaJIT - end");
    Debug.set_crashhandler(nullptr);
}

void CScriptStorage::reinit(lua_State* LSVM)
{
    if (m_virtual_machine) //Как выяснилось, такое происходит при загрузке игры на этапе старта сервера
    {
        // Msg("[CScriptStorage] Found working LuaJIT WM! Close it!");
        lua_close(m_virtual_machine);
    }
    m_virtual_machine = LSVM;

    Debug.set_crashhandler(ScriptCrashHandler);
}

void CScriptStorage::print_stack() { Log(get_lua_traceback(lua())); }

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
    case ScriptStorage::eLuaMessageTypeInfo: S = "[LUA INFO]"; break;
    case ScriptStorage::eLuaMessageTypeError: S = "[LUA ERROR]"; break;
    case ScriptStorage::eLuaMessageTypeMessage: S = "[LUA MESSAGE]"; break;
    case ScriptStorage::eLuaMessageTypeHookCall: S = "[LUA HOOK_CALL]"; break;
    case ScriptStorage::eLuaMessageTypeHookReturn: S = "[LUA HOOK_RETURN]"; break;
    case ScriptStorage::eLuaMessageTypeHookLine: S = "[LUA HOOK_LINE]"; break;
    case ScriptStorage::eLuaMessageTypeHookCount: S = "[LUA HOOK_COUNT]"; break;
    case ScriptStorage::eLuaMessageTypeHookTailReturn: S = "[LUA HOOK_TAIL_RETURN]"; break;
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

bool CScriptStorage::load_buffer(lua_State* L, const char* caBuffer, size_t tSize, const char* caScriptName,
                                 const char* caNameSpaceName) // KRodin: эта функция форматирует содержимое скрипта используя FILE_HEADER и после этого загружает его в lua
{
    int l_iErrorCode = 0;
    const std::string_view strbuf{caBuffer, tSize};
    if (strcmp(GlobalNamespace, caNameSpaceName)) //Все скрипты кроме _G
    {
        // KRodin: обращаться к _G только с большой буквы! Иначе он загрузится ещё раз и это неизвестно к чему приведёт!
        //Глобальное пространство инитится один раз после запуска луаджита, и никогда больше.
        if (!strcmp("_g", caNameSpaceName))
            return false;

        const std::string script = std::format(FILE_HEADER, caNameSpaceName, strbuf);

        // Log("[CScriptStorage::load_buffer(1)] Loading buffer:");
        // Log(script.c_str());
        l_iErrorCode = luaL_loadbuffer(L, script.c_str(), script.size(), caScriptName);
    }
    else //_G.script и только он.
    {
        // Log("[CScriptStorage::load_buffer(2)] Loading buffer:");
        // Log(strbuf.c_str());
        l_iErrorCode = luaL_loadbuffer(L, strbuf.data(), strbuf.size(), caScriptName);
    }
    if (l_iErrorCode)
    {
        print_output(L, caScriptName, l_iErrorCode);
        R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
        return false;
    }
    return true;
}

bool CScriptStorage::do_file(
    const char* caScriptName,
    const char* caNameSpaceName) // KRodin: эта функция открывает скрипт с диска и оправляет его содержимое в функцию load_buffer, после этого походу запускает скрипт.
{
    auto l_tpFileReader = FS.r_open(caScriptName);
    if (!l_tpFileReader)
    {
        //заменить на ассерт?
        Msg("!![CScriptStorage::do_file] Cannot open file [%s]", caScriptName);
        return false;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    strconcat(sizeof(l_caLuaFileName), l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\***.script

    bool loaded = load_buffer(lua(), reinterpret_cast<const char*>(l_tpFileReader->pointer()), (size_t)l_tpFileReader->elapsed(), l_caLuaFileName, caNameSpaceName);

    FS.r_close(l_tpFileReader);
    if (!loaded)
        return false;

    int l_iErrorCode = lua_pcall(lua(), 0, 0, 0); // KRodin: без этого скрипты не работают!
    if (l_iErrorCode)
    {
        print_output(lua(), caScriptName, l_iErrorCode);
        R_ASSERT(false); //НЕ ЗАКОММЕНТИРОВАТЬ!
        return false;
    }
    return true;
}

bool CScriptStorage::namespace_loaded(const char* name, bool remove_from_stack) // KRodin: видимо, функция проверяет, загружен ли скрипт.
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
            // lua_settop(lua(),0);
            VERIFY(lua_gettop(lua()) >= 2);
            lua_pop(lua(), 2);
            VERIFY(start == lua_gettop(lua()));
            return false; // there is no namespace!
        }
        else if (!lua_istable(lua(), -1))
        {
            // lua_settop(lua(), 0);
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

    auto lua_namespace = luabind::get_globals(lua());

    for (;;)
    {
        if (!xr_strlen(S))
            return lua_namespace;
        auto I = strchr(S, '.');
        if (!I)
            return lua_namespace[S];
        *I = 0;
        lua_namespace = lua_namespace[S];
        S = I + 1;
    }
}

bool CScriptStorage::print_output(lua_State* L, const char* caScriptFileName,
                                  int errorCode) // KRodin: вызывается из нескольких мест, в т.ч. из калбеков lua_error, lua_pcall_failed, lua_cast_failed, lua_panic
{
    auto Prefix = "";
    if (errorCode)
    {
        switch (errorCode)
        {
        case LUA_ERRRUN: Prefix = "SCRIPT RUNTIME ERROR"; break;
        case LUA_ERRMEM: Prefix = "SCRIPT ERROR (memory allocation)"; break;
        case LUA_ERRERR: Prefix = "SCRIPT ERROR (while running the error handler function)"; break;
        case LUA_ERRFILE: Prefix = "SCRIPT ERROR (while running file)"; break;
        case LUA_ERRSYNTAX: Prefix = "SCRIPT SYNTAX ERROR"; break;
        case LUA_YIELD: Prefix = "Thread is yielded"; break;
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
