////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "alife_simulator.h"
#include "xrServer_Objects_ALife.h"
#include "ai_space.h"
#include "../IGame_Persistent.h"
#include "script_engine.h"
#include "mainmenu.h"
#include "object_factory.h"

LPCSTR alife_section = "alife";

extern void destroy_lua_wpn_params	();

void restart_all				()
{
	if (strstr(Core.Params,"-keep_lua"))
		return;

	destroy_lua_wpn_params		();
	MainMenu()->DestroyInternal	(true);
	xr_delete					(g_object_factory);
	ai().script_engine().init	();
}

CALifeSimulator::CALifeSimulator		(xrServer *server, shared_str *command_line) :
	CALifeUpdateManager			(server,alife_section),
	CALifeInteractionManager	(server,alife_section),
	CALifeSimulatorBase			(server,alife_section)
{
	restart_all					();

	ai().set_alife				(this);

	setup_command_line			(command_line);

	typedef IGame_Persistent::params params;
	params						&p = g_pGamePersistent->m_game_params;
	
	R_ASSERT2					(
		xr_strlen(p.m_game_or_spawn) && 
		!xr_strcmp(p.m_alife,"alife") && 
		!xr_strcmp(p.m_game_type,"single"),
		"Invalid server options!"
	);
	
	string256					temp;
	strcpy						(temp,p.m_game_or_spawn);
	strcat						(temp,"/");
	strcat						(temp,p.m_game_type);
	strcat						(temp,"/");
	strcat						(temp,p.m_alife);
	*command_line				= temp;
	
	LPCSTR						start_game_callback = pSettings->r_string(alife_section,"start_game_callback");
	luabind::functor<void>		functor;
	R_ASSERT2					(ai().script_engine().functor(start_game_callback,functor),"failed to get start game callback");
	functor						();

	load						(p.m_game_or_spawn,!xr_strcmp(p.m_new_or_load,"load") ? false : true, !xr_strcmp(p.m_new_or_load,"new"));
}

CALifeSimulator::~CALifeSimulator		()
{
	VERIFY						(!ai().get_alife());
}

void CALifeSimulator::destroy			()
{
//	validate					();
	CALifeUpdateManager::destroy();
	VERIFY						(ai().get_alife());
	ai().set_alife				(0);
}

void CALifeSimulator::setup_simulator	(CSE_ALifeObject *object)
{
//	VERIFY2						(!object->m_alife_simulator,object->s_name_replace);
	object->m_alife_simulator	= this;
}

void CALifeSimulator::reload			(LPCSTR section)
{
	CALifeUpdateManager::reload	(section);
}
