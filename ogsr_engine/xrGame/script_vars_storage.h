////////////////////////////////////////////////////////////////////////////
//	Module 		: script_vars_storage.h
//	Created 	: 19.10.2014
//  Modified 	: 22.10.2014
//	Author		: Alexander Petrov
//	Description : global script vars class, with saving content to savegame
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "script_export_space.h"

#define LUA_TNETPACKET 0x00000100
#define SVT_ARRAY_TABLE 0x00020000 // используются только численные индексы
#define SVT_ARRAY_ZEROK 0x00040000 // нестандартный массив с нулевым индексом
#define SVT_KEY_BOOLEAN 0x00100000
#define SVT_KEY_NUMERIC 0x00300000
#define SVT_KEY_MASK 0x00F00000

#define SVT_ALLOCATED 0x10000000

class NET_Packet;
class CScriptVarsTable;

typedef struct _SCRIPT_VAR
{
    // string32	   name;
    int type; // tag
    u32 size;
    union
    {
        void* data;
        double n_value;
        bool b_value;
        char s_value[8];
        CScriptVarsTable* T;
        NET_Packet* P;
    };

    u32 eff_type() { return type & 0xffff; }
    void release();
    void* smart_alloc(int new_type, u32 cb = 4);

    bool is_key_boolean() { return (type & SVT_KEY_MASK) == SVT_KEY_BOOLEAN; }
    bool is_key_numeric() { return (type & SVT_KEY_MASK) == SVT_KEY_NUMERIC; }

} SCRIPT_VAR;

typedef xr_map<shared_str, SCRIPT_VAR> SCRIPT_VARS_MAP;

class CScriptVarsTable // таблица переменных
{
private:
    SCRIPT_VARS_MAP m_map;
    shared_str m_name;
    int ref_count;

public:
    bool is_array;
    bool zero_key;

    CScriptVarsTable() { ref_count = 0; }
    virtual ~CScriptVarsTable();
    virtual int assign(lua_State* L, int index);
    virtual void clear();

    virtual void add_ref() { ref_count++; }
    virtual void release();

    ICF SCRIPT_VARS_MAP& map() { return m_map; }
    virtual int load(IReader& memory_stream);
    virtual int save(IWriter& memory_stream);
    void get(lua_State* L, LPCSTR k, bool unpack);
    void set(lua_State* L, int key_index, int value_index);
    void set(lua_State* L, LPCSTR k, int index, int key_type);
    ICF int size() { return map().size(); };

    ICF LPCSTR name()
    {
        LPCSTR res = *m_name;
        return res ? res : "";
    }
    ICF void set_name(LPCSTR n) { m_name = n; }
};

class CScriptVarsStorage : public CScriptVarsTable
{
    typedef CScriptVarsTable inherited;

public:
    virtual int load(IReader& memory_stream);
    virtual int save(IWriter& memory_stream);
    virtual void release(){};
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptVarsStorage)
#undef script_type_list
#define script_type_list save_type_list(CScriptVarsStorage)

    extern CScriptVarsStorage g_ScriptVars;
extern int lua_pushsvt(lua_State* L, CScriptVarsTable* T);
