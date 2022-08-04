////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
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

struct lua_State;

class CScriptStorage
{
protected:
    lua_State* m_virtual_machine = nullptr;
    xr_set<void*> m_dumpedObjList;
    bool do_file(const char* caScriptName, const char* caNameSpaceName);
    bool load_buffer(lua_State* L, const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName);
    bool namespace_loaded(const char* caName, bool remove_from_stack = true);
    bool object(const char* caIdentifier, int type);
    bool object(const char* caNamespaceName, const char* caIdentifier, int type);
    luabind::object name_space(const char* namespace_name);
    void reinit(lua_State* LSVM);

public:
    lua_State* lua() { return m_virtual_machine; };
    CScriptStorage() = default;
    virtual ~CScriptStorage();
    void script_log(ScriptStorage::ELuaMessageType message, const char* caFormat, ...);
    static bool print_output(lua_State* L, const char* caScriptName, int iErorCode = 0);
    static constexpr const char* GlobalNamespace = "_G";
    void print_stack();
    void dump_state();
    void LogTable(lua_State* l, LPCSTR S, int level);
    void LogVariable(lua_State* l, const char* name, int level);
};
