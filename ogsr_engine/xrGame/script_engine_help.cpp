////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_help.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script Engine help
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(LUABIND_NO_ERROR_CHECKING) && !defined(NDEBUG)

static IWriter* dumper = nullptr;
string1024 line_buf;

#include <sstream>

// redefinition for fast save
void OpenDumper()
{
    string_path dump_name;
    strcpy_s(dump_name, 260, "lua_help_OGSR.script");

    if (FS.path_exist("$logs$"))
        FS.update_path(dump_name, "$logs$", dump_name);

    dumper = FS.w_open(dump_name);
}

void CloseDumper()
{
    if (dumper)
    {
        FS.w_close(dumper);
        xr_delete(dumper);
        dumper = NULL;
    }
}

void FastMsg(LPCSTR format, ...)
{
    static u32 saldo = 0;
    va_list mark;
    va_start(mark, format);
    _vsnprintf(line_buf, sizeof(line_buf) - 1, format, mark);

    line_buf[sizeof(line_buf) - 1] = 0;
    va_end(mark);
    dumper->w_printf("%s\r\n", line_buf);
    saldo += xr_strlen(line_buf);
    if (saldo >= 32768)
    {
        saldo = 0;
        dumper->flush();
    }
}

luabind::internal_string to_string(luabind::object const& o)
{
    using namespace luabind;
    if (o.type() == LUA_TSTRING)
        return object_cast<luabind::internal_string>(o);
    lua_State* L = o.lua_state();
    LUABIND_CHECK_STACK(L);

    std::stringstream s;

    if (o.type() == LUA_TNUMBER)
    {
        s << object_cast<float>(o);
        return luabind::internal_string(s.str().c_str());
    }

    s << "<" << lua_typename(L, o.type()) << ">";
    return s.str().c_str();
}

void strreplaceall(luabind::internal_string& str, LPCSTR S, LPCSTR N)
{
    LPCSTR A;
    int S_len = xr_strlen(S);
    while ((A = strstr(str.c_str(), S)) != 0)
        str.replace(A - str.c_str(), S_len, N);
}

luabind::internal_string& process_signature(luabind::internal_string& str)
{
    strreplaceall(str, "custom [", "");
    strreplaceall(str, "]", "");
    strreplaceall(str, "float", "number");
    strreplaceall(str, "lua_State*, ", "thread");
    strreplaceall(str, " ,lua_State*", "thread");
    return (str);
}

luabind::internal_string& extract_last_params(luabind::internal_string& str)
{
    process_signature(str);
    LPCSTR s = str.c_str();
    LPCSTR c1 = strstr(s, "(");
    LPCSTR c2 = strstr(s, "*");
    LPCSTR c3 = strstr(s, ", ");
    if (c1 && c2 && c3 && c2 < c3)
    {
        str.erase(str.find('('), str.find(", ") + 1);
        if (str.at(0) == ' ')
            str.erase(0, 1);
        str = "(" + str;
    }
    return (str);
}

luabind::internal_string member_to_string(luabind::object const& e, LPCSTR function_signature)
{
    using namespace luabind;
    lua_State* L = e.lua_state();
    LUABIND_CHECK_STACK(L);

    if (e.type() == LUA_TFUNCTION)
    {
        e.pushvalue();
        detail::stack_pop p(L, 1);

        {
            if (lua_getupvalue(L, -1, 3) == 0)
                return to_string(e);
            detail::stack_pop p2(L, 1);
            if (lua_touserdata(L, -1) != reinterpret_cast<void*>(0x1337))
                return to_string(e);
        }

        std::stringstream s;
        {
            lua_getupvalue(L, -1, 2);
            detail::stack_pop p2(L, 1);
        }

        {
            lua_getupvalue(L, -1, 1);
            detail::stack_pop p2(L, 1);
            detail::method_rep* m = static_cast<detail::method_rep*>(lua_touserdata(L, -1));

            for (auto i = m->overloads().begin(); i != m->overloads().end(); ++i)
            {
                luabind::internal_string str;
                i->get_signature(L, str);
                if (i != m->overloads().begin())
                    s << "\n";
                s << function_signature << process_signature(str) << ";";
            }
        }
        return s.str().c_str();
    }

    return to_string(e);
}

void print_class(lua_State* L, luabind::detail::class_rep* crep)
{
    luabind::internal_string S;
    // print class and bases
    {
        S = (crep->get_class_type() != luabind::detail::class_rep::cpp_class) ? "LUA class " : "C++ class ";
        S.append(crep->name());
        typedef luabind::internal_vector<luabind::detail::class_rep::base_info> BASES;
        const BASES& bases = crep->bases();
        BASES::const_iterator I = bases.begin(), B = I;
        BASES::const_iterator E = bases.end();
        if (B != E)
            S.append(" : ");
        for (; I != E; ++I)
        {
            if (I != B)
                S.append(",");
            S.append((*I).base->name());
        }
        FastMsg("%s {", S.c_str());
    }
    // print class constants
    {
        const luabind::detail::class_rep::STATIC_CONSTANTS& constants = crep->static_constants();
        luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator I = constants.begin();
        luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator E = constants.end();
        for (; I != E; ++I)
            FastMsg("    const %s = %d;", (*I).first, (*I).second);
        if (!constants.empty())
            FastMsg("    ");
    }
    // print class properties
    using namespace luabind::detail;
    {
        typedef luabind::internal_map<const char*, luabind::detail::class_rep::callback, luabind::detail::ltstr> PROPERTIES;
        const PROPERTIES& properties = crep->properties();
        const PROPERTIES& properties_rw = crep->properties_rw();
        PROPERTIES::const_iterator I = properties.begin();
        PROPERTIES::const_iterator E = properties.end();
        for (; I != E; ++I)
        {
            LPCSTR pname = (*I).first;
            PROPERTIES::const_iterator X = properties_rw.find(pname);

            luabind::internal_string cname = "";
            if (X != properties_rw.end())
            {
                luabind::detail::class_rep::callback cb = properties_rw.at(pname);
                if ((size_t)cb.sig > 0x1000u)
                    cb.sig(L, cname);

                FastMsg(" property\t\t\t%-25s%s;", pname, extract_last_params(cname).c_str());
            }
            else
                FastMsg(" property\t\t\t%s;", pname);
        }
        if (!properties.empty())
            FastMsg("    ");
    }
    // print class constructors

    {
        const luabind::internal_vector<luabind::detail::construct_rep::overload_t>& constructors = crep->constructors().overloads;
        auto I = constructors.cbegin();
        auto E = constructors.cend();
        for (; I != E; ++I)
        {
            luabind::internal_string S;
            (*I).get_signature(L, S);
            strreplaceall(S, "custom [", "");
            strreplaceall(S, "]", "");
            strreplaceall(S, "float", "number");
            strreplaceall(S, "lua_State*, ", "");
            strreplaceall(S, " ,lua_State*", "");
            FastMsg("    %s %s;", crep->name(), S.c_str());
        }
        if (!constructors.empty())
            FastMsg("    ");
    }

    // print class methods
    {
        crep->get_table(L);
        luabind::object table(L);
        table.set();
        for (luabind::object::iterator i = table.begin(); i != table.end(); ++i)
        {
            luabind::object object = *i;
            luabind::internal_string S;
            S = "    function ";
            S.append(to_string(i.key()).c_str());

            strreplaceall(S, "function __add", "operator +");
            strreplaceall(S, "function __sub", "operator -");
            strreplaceall(S, "function __mul", "operator *");
            strreplaceall(S, "function __div", "operator /");
            strreplaceall(S, "function __pow", "operator ^");
            strreplaceall(S, "function __lt", "operator <");
            strreplaceall(S, "function __le", "operator <=");
            strreplaceall(S, "function __gt", "operator >");
            strreplaceall(S, "function __ge", "operator >=");
            strreplaceall(S, "function __eq", "operator ==");
            FastMsg("%s", member_to_string(object, S.c_str()).c_str());
        }
    }
    FastMsg("};\n");
}

void print_free_functions(lua_State* L, const luabind::object& object, LPCSTR header, const luabind::internal_string& indent)
{
    u32 count = 0;
    luabind::object::iterator I = object.begin();
    luabind::object::iterator E = object.end();
    for (; I != E; ++I)
    {
        if ((*I).type() != LUA_TFUNCTION)
            continue;
        (*I).pushvalue();
        luabind::detail::free_functions::function_rep* rep = 0;
        if (lua_iscfunction(L, -1))
        {
            if (lua_getupvalue(L, -1, 2) != 0)
            {
                // check the magic number that identifies luabind's functions
                if (lua_touserdata(L, -1) == (void*)0x1337)
                {
                    if (lua_getupvalue(L, -2, 1) != 0)
                    {
                        if (!count)
                            FastMsg("\n%snamespace %s {", indent.c_str(), header);
                        ++count;
                        rep = static_cast<luabind::detail::free_functions::function_rep*>(lua_touserdata(L, -1));
                        auto i = rep->overloads().cbegin();
                        auto e = rep->overloads().cend();
                        for (; i != e; ++i)
                        {
                            luabind::internal_string S;
                            (*i).get_signature(L, S);
                            FastMsg("    %sfunction %s%s;", indent.c_str(), rep->name(), process_signature(S).c_str());
                        }
                        lua_pop(L, 1);
                    }
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
    }
    {
        static xr_vector<luabind::internal_string> nesting_path;

        luabind::internal_string _indent = indent;
        luabind::internal_string last_key = "?";
        _indent.append("    ");

        object.pushvalue();
        int n_table = lua_gettop(L);
        // Msg("# n_table = %d ", n_table);
        lua_pushnil(L);
        int save_top = lua_gettop(L);

        // #pragma todo("alpet : при загруженной сохраненке здесь иногда происходит сбой invalid key to 'next', а потом креш в недрах Direct3D ")
        while (lua_next(L, n_table) != 0)
        {
            last_key = "~";
            int key_type = lua_type(L, -2);
            if (lua_type(L, -1) == LUA_TTABLE && key_type == LUA_TSTRING && lua_objlen(L, -2) > 0)
            {
                last_key = lua_tostring(L, -2);
                LPCSTR S = last_key.c_str();
                string_path script_name;
                sprintf_s(script_name, sizeof(script_name) - 1, "%s.script", S);
                if (nesting_path.size() == 0 && // скан глобального пространства имен
                    (last_key == "" || last_key == "config" || last_key == "package" || last_key == "jit" || last_key == "loaded" || last_key == "md_list" ||
                     FS.exist("$game_scripts$", script_name))) // с дампом экспортируемых luabind вещей, возникают сбои!
                {
                    Msg("! skipping namespace %s ", last_key.c_str());
                    lua_pop(L, 1);
                    continue;
                }

                if (xr_strcmp("_G", S))
                {
                    luabind::object object(L);
                    object.set();
                    // if (!xr_strcmp("security", S)) { S = S; } /// wtf?
                    luabind::internal_string path_dump = "";
                    for (u32 ns = 0; ns < nesting_path.size(); ns++)
                        path_dump = path_dump + nesting_path.at(ns) + ".";

                    path_dump = path_dump + S;
                    Msg("#dumping namespace %s ", path_dump.c_str());
                    nesting_path.push_back(S);

                    u32 nest_level = nesting_path.size();
                    // если слишком много вложений или начали повторяться строки
                    if (nest_level < 2 && !(nest_level > 1 && nesting_path.at(0) == S))
                    {
                        print_free_functions(L, object, S, _indent);
                    }
                    else
                    {
                        // problem detected
                        Msg("! WARN: to many nested levels for export = %d, or self-reference detected ", nest_level);
                        dumper->flush();
                    }
                    nesting_path.pop_back();
                }
            }
            // #pragma todo("Dima to Dima : Remove this hack if find out why")

            // */
            // lua_pop	(L, 1);	// remove value from stack
            lua_pop(L, 1);
            if (lua_gettop(L) > save_top)
            {
                Msg("lua_gettop returned %d vs expected %d", lua_gettop(L), save_top);
                lua_settop(L, save_top);
            }
        }
    }
    if (count)
        FastMsg("%s};", indent.c_str());
}

void print_help(lua_State* L)
{
    if (g_pGameLevel)
    {
        Msg("!WARN: Рекомендуется lua_help выполнять до загрузки уровня (из главного меню).");
        // return;
    }

    OpenDumper();
    BOOL paused = Device.Paused();
    Device.Pause(TRUE, TRUE, FALSE, "lua_help");
    // L = lua_newthread (L);
    int top = lua_gettop(L);
    SleepEx(10, FALSE);

#if !defined(_CPPUNWIND)
    __try
#else
    try
#endif
    {
        FastMsg("\nList of the classes exported to LUA\n");
        luabind::detail::class_registry::get_registry(L)->iterate_classes(L, &print_class);
        FastMsg("End of list of the classes exported to LUA\n");
        FastMsg("\nList of the namespaces exported to LUA\n");
#if !defined(_CPPUNWIND)
        __try
#else
        try
#endif
        {
            print_free_functions(L, luabind::get_globals(L), "", " ");
            FastMsg("End of list of the namespaces exported to LUA\n");
        }
#if !defined(_CPPUNWIND)
        __except (EXCEPTION_EXECUTE_HANDLER)
#else
        catch (...)
#endif
        {
            Msg("Fatal: Exception catched in print_free_functions  ");
            FastMsg("WARNING: incomplete list of the namespaces exported to LUA\n");
        }
#if !defined(_CPPUNWIND)
    }
    __finally
    {
#endif
        dumper->flush();
        CloseDumper();
        Device.Pause(paused, TRUE, FALSE, "lua_help");
        lua_settop(L, top);
    }
#if defined(_CPPUNWIND)
    catch (...)
    {
        Msg("Fatal: Exception catched in print_help!");
        FastMsg("WARNING: incomplete list of the namespaces exported to LUA\n");
    }
#endif
}
#else
void print_help(lua_State* L) { Msg("! For a dump lua_help, uncomment #define ENABLE_DUMP_LUA_HELP in config.hpp"); }
#endif
