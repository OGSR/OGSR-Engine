////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader.cpp
//	Created 	: 03.09.2003
//  Modified 	: 03.09.2003
//	Author		: Dmitriy Iassenev
//	Description : ALife trader class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"

void CSE_ALifeTrader::spawn_supplies()
{
    inherited1::spawn_supplies();
    inherited2::spawn_supplies();
}

void CSE_ALifeTrader::add_online(const bool& update_registries) { CSE_ALifeTraderAbstract::add_online(update_registries); }

void CSE_ALifeTrader::add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
    CSE_ALifeTraderAbstract::add_offline(saved_children, update_registries);
}
