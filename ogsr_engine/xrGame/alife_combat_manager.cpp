////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_combat_manager.h
//	Created 	: 12.08.2003
//  Modified 	: 14.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife combat manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_combat_manager.h"
#include "alife_graph_registry.h"
#include "alife_schedule_registry.h"

CALifeCombatManager::CALifeCombatManager(xrServer* server, LPCSTR section) : CALifeSimulatorBase(server, section) {}