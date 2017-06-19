////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script4.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"
#include "alife_human_brain.h"

using namespace luabind;

CALifeMonsterBrain *monster_brain	(CSE_ALifeMonsterAbstract *monster)
{
	THROW	(monster);
	return	(&monster->brain());
}

CALifeHumanBrain *human_brain		(CSE_ALifeHumanAbstract *human)
{
	THROW	(human);
	return	(&human->brain());
}

void clear_smart_terrain			(CSE_ALifeMonsterAbstract *monster)
{
	THROW						(monster);
	monster->m_smart_terrain_id	= 0xffff;
}

void smart_terrain_task_activate	(CSE_ALifeMonsterAbstract *monster)
{
	THROW						(monster);
	monster->m_task_reached		= true;
}

void smart_terrain_task_deactivate	(CSE_ALifeMonsterAbstract *monster)
{
	THROW						(monster);
	monster->m_task_reached		= false;
}


ALife::_OBJECT_ID smart_terrain_id	(CSE_ALifeMonsterAbstract *monster)
{
	THROW						(monster);
	return						(monster->m_smart_terrain_id);
}

#pragma optimize("s",on)
void CSE_ALifeMonsterAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterAbstract,
			"cse_alife_monster_abstract",
			CSE_ALifeCreatureAbstract,
			CSE_ALifeSchedulable
		)
		.def("smart_terrain_id",				&smart_terrain_id)
		.def("clear_smart_terrain",				&clear_smart_terrain)
		.def("brain",							&monster_brain)
		.def("rank",							&CSE_ALifeMonsterAbstract::Rank)
		.def("smart_terrain_task_activate",		&smart_terrain_task_activate)
		.def("smart_terrain_task_deactivate",	&smart_terrain_task_deactivate)
	];
}

void CSE_ALifeHumanAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeHumanAbstract,
			"cse_alife_human_abstract",
			CSE_ALifeTraderAbstract,
			CSE_ALifeMonsterAbstract
		)
		.def("brain",				&human_brain)
#ifdef XRGAME_EXPORTS
		.def("rank",				&CSE_ALifeTraderAbstract::Rank)
		.def("set_rank",			&CSE_ALifeTraderAbstract::SetRank)

#endif
	];
}

void CSE_ALifePsyDogPhantom::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifePsyDogPhantom,
			"cse_alife_psydog_phantom",
			CSE_ALifeMonsterBase
			)
	];
}

