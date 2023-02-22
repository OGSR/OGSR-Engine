////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

LPCSTR profile_name_script(CSE_ALifeTraderAbstract* ta) { return *ta->character_profile(); }

LPCSTR character_name_script(CSE_ALifeTraderAbstract* ta) { return ta->m_character_name.c_str(); }

void set_character_name_script(CSE_ALifeTraderAbstract* ta, LPCSTR name) { ta->m_character_name = name; }

#pragma optimize("s", on)
void CSE_ALifeTraderAbstract::script_register(lua_State* L)
{
    module(L)[class_<CSE_ALifeTraderAbstract>("cse_alife_trader_abstract")
//			.def(		constructor<LPCSTR>())
#ifdef XRGAME_EXPORTS
                  .def("community", &CommunityName)
                  .def("profile_name", &profile_name_script)
                  .def("rank", &Rank)
                  .def("reputation", &Reputation)
#endif // XRGAME_EXPORTS
                  .def_readwrite("money", &CSE_ALifeTraderAbstract::m_dwMoney)
                  .property("character_name", &character_name_script, &set_character_name_script)];
}

ALife::_OBJECT_ID CSE_AlifeTrader__smart_terrain_id(CSE_ALifeTrader* trader)
{
    THROW(trader);
    return 0xffff;
}

void CSE_ALifeTrader::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife2(CSE_ALifeTrader, "cse_alife_trader", CSE_ALifeDynamicObjectVisual, CSE_ALifeTraderAbstract)
                  .def("smart_terrain_id", &CSE_AlifeTrader__smart_terrain_id)];
}

void CSE_ALifeCustomZone::script_register(lua_State* L) { module(L)[luabind_class_dynamic_alife1(CSE_ALifeCustomZone, "cse_custom_zone", CSE_ALifeSpaceRestrictor)]; }

void CSE_ALifeAnomalousZone::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife1(CSE_ALifeAnomalousZone, "cse_anomalous_zone", CSE_ALifeCustomZone).def("spawn_artefacts", &CSE_ALifeAnomalousZone::spawn_artefacts)];
}
