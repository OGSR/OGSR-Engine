////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_binder.h"
#include "xrServer_Objects_ALife.h"
#include "script_binder_object.h"
#include "script_game_object.h"
#include "gameobject.h"
#include "level.h"

CScriptBinder::CScriptBinder() { init(); }

CScriptBinder::~CScriptBinder() { VERIFY(!m_object); }

void CScriptBinder::init() { m_object = 0; }

void CScriptBinder::clear()
{
    // try {
    xr_delete(m_object);
    //}
    // catch(...) {
    //	m_object			= 0;
    //}
    init();
}

void CScriptBinder::reinit()
{
    if (m_object)
    {
        m_object->reinit();
    }
}

void CScriptBinder::Load(LPCSTR section) {}

void CScriptBinder::reload(LPCSTR section)
{
    VERIFY(!m_object);

    if (!pSettings->line_exist(section, "script_binding"))
        return;

    auto script_func_name = pSettings->r_string(section, "script_binding");
    luabind::functor<void> lua_function;
    if (!ai().script_engine().functor(script_func_name, lua_function))
    {
        Msg("!![CScriptBinder::reload] function [%s] not loaded!", script_func_name);
        return;
    }

    auto game_object = smart_cast<CGameObject*>(this);
    if (!game_object) //Объекта нет - значит тут делать нечего.
    {
        Msg("!![[CScriptBinder::reload] failed cast to CGameObject!");
        return;
    }

    lua_function(game_object->lua_game_object());

    if (m_object)
    {
        m_object->reload(section);
    }
}

BOOL CScriptBinder::net_Spawn(CSE_Abstract* DC)
{
    CSE_Abstract* abstract = (CSE_Abstract*)DC;
    CSE_ALifeObject* object = smart_cast<CSE_ALifeObject*>(abstract);
    if (object && m_object)
    {
        return (BOOL)m_object->net_Spawn(object);
    }
    return TRUE;
}

void CScriptBinder::net_Destroy()
{
    if (m_object)
    {
#ifdef DEBUG
        Msg("* Core object %s is UNbinded from the script object", smart_cast<CGameObject*>(this) ? *smart_cast<CGameObject*>(this)->cName() : "");
#endif // DEBUG
        m_object->net_Destroy();
    }
    xr_delete(m_object);
}

void CScriptBinder::set_object(CScriptBinderObject* object)
{
    VERIFY2(!m_object, "Cannot bind to the object twice!");
#ifdef DEBUG
    Msg("* Core object %s is binded with the script object", smart_cast<CGameObject*>(this) ? *smart_cast<CGameObject*>(this)->cName() : "");
#endif // DEBUG
    m_object = object;
}

void CScriptBinder::shedule_Update(u32 time_delta)
{
    if (m_object)
    {
        m_object->shedule_Update(time_delta);
    }
}

void CScriptBinder::save(NET_Packet& output_packet)
{
    if (m_object)
    {
        m_object->save(&output_packet);
    }
}

void CScriptBinder::load(IReader& input_packet)
{
    if (m_object)
    {
        m_object->load(&input_packet);
    }
}

BOOL CScriptBinder::net_SaveRelevant()
{
    if (m_object)
    {
        return m_object->net_SaveRelevant();
    }
    return FALSE;
}

void CScriptBinder::net_Relcase(CObject* object)
{
    CGameObject* game_object = smart_cast<CGameObject*>(object);
    if (m_object && game_object)
    {
        m_object->net_Relcase(game_object->lua_game_object());
    }
}
