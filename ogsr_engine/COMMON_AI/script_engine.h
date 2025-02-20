////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ScriptStorage
{
enum ELuaMessageType
{
    eLuaMessageTypeInfo = u32(0),
    eLuaMessageTypeError,
    eLuaMessageTypeMessage,
    eLuaMessageTypeHookCall,
    eLuaMessageTypeHookReturn,
    eLuaMessageTypeHookLine,
    eLuaMessageTypeHookCount,
    eLuaMessageTypeHookTailReturn = u32(-1),
};
}
using namespace ScriptStorage;

#include "script_export_space.h"

class CScriptEngine
{
    bool m_reload_modules{};
    string_unordered_map<shared_str, bool> no_files;
    string_unordered_map<shared_str, shared_str> xray_scripts;

    inline bool no_file_exists(const char* file_name) const { return no_files.contains(file_name); }
    inline void add_no_file(const char* file_name) { no_files.emplace(file_name, true); }

    int m_stack_level{};

    lua_State* m_virtual_machine{};
    xr_set<void*> m_dumpedObjList;
    bool do_file(const char* caScriptName, const char* caNameSpaceName);
    bool load_buffer(lua_State* L, const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName);
    bool namespace_loaded(const char* caName, bool remove_from_stack = true);
    bool object(const char* caIdentifier, int type);
    bool object(const char* caNamespaceName, const char* caIdentifier, int type);
    luabind::object name_space(const char* namespace_name);
    void reinit(lua_State* LSVM);
    void close();
    void CollectScriptFiles(const char* path);
    bool LookupScript(string_path& fname, const char* base);

public:
    CScriptEngine() = default;
    ~CScriptEngine();

    void init();
    virtual void unload();
    static int lua_panic(lua_State* L);
#ifdef LUABIND_NO_EXCEPTIONS
    static void lua_error(lua_State* L);
#endif
    static int lua_pcall_failed(lua_State* L);
    void setup_auto_load();
    bool process_file_if_exists(const char* file_name, bool warn_if_not_exist);
    bool process_file(const char* file_name);
    bool process_file(const char* file_name, bool reload_modules);
    bool function_object(const char* function_to_call, luabind::object& object, int type = LUA_TFUNCTION);
    void register_script_classes();
    void parse_script_namespace(const char* name, char* ns, u32 nsSize, char* func, u32 funcSize);

    void collect_all_garbage();

    lua_State* lua() { return m_virtual_machine; };
    void script_log(ScriptStorage::ELuaMessageType message, const char* caFormat, ...);
    static bool print_output(lua_State* L, const char* caScriptName, int iErorCode = 0);
    static constexpr const char* GlobalNamespace = "_G";
    void print_stack();
    void dump_state();
    void LogTable(lua_State* l, LPCSTR S, int level);
    void LogVariable(lua_State* l, const char* name, int level);

    template <typename TResult>
    IC bool functor(const char* function_to_call, luabind::functor<TResult>& lua_function)
    {
        luabind::object object;
        if (!function_object(function_to_call, object))
            return false;
        // try {
        lua_function = luabind::object_cast<luabind::functor<TResult>>(object);
        //}
        // catch (...) {
        //	return false;
        //}
        return true;
    }

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)
