////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomalous_zone.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife anomalous zone class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "alife_spawn_registry.h"
#include "alife_graph_registry.h"

#pragma warning(push)
#pragma warning(disable : 4995)
#include <malloc.h>
#pragma warning(pop)

CSE_ALifeItemWeapon* CSE_ALifeAnomalousZone::tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower)
{
    m_tpCurrentBestWeapon = 0;
    m_tTimeID = ai().alife().time_manager().game_time();
    fHitPower = 1.0f; // m_maxPower;
    tHitType = m_tHitType;
    return (m_tpCurrentBestWeapon);
}

ALife::EMeetActionType CSE_ALifeAnomalousZone::tfGetActionType(CSE_ALifeSchedulable* tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
    return (ALife::eMeetActionTypeAttack);
}

bool CSE_ALifeAnomalousZone::bfActive() { return !interactive(); }

CSE_ALifeDynamicObject* CSE_ALifeAnomalousZone::tpfGetBestDetector()
{
    VERIFY2(false, "This function shouldn't be called");
    NODEFAULT;
#ifdef DEBUG
    return (0);
#endif
}

void CSE_ALifeAnomalousZone::on_spawn() { inherited::on_spawn(); }
