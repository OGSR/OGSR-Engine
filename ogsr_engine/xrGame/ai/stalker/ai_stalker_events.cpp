////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_events.cpp
//	Created 	: 26.02.2003
//  Modified 	: 26.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Events handling for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../pda.h"
#include "../../inventory.h"
#include "xrmessages.h"
#include "../../shootingobject.h"
#include "../../../xr_3da/NET_Server_Trash/net_utils.h"
#include "level.h"
#include "../../ai_monster_space.h"
#include "../../stalker_planner.h"
#include "../../script_game_object.h"
#include "../../stalker_decision_space.h"

using namespace StalkerSpace;
using namespace MonsterSpace;

#define SILENCE

void CAI_Stalker::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	switch (type)
	{
		case GE_TRADE_BUY :
		case GE_OWNERSHIP_TAKE :
		case GE_TRANSFER_TAKE : {

			u16			id;
			P.r_u16		(id);
			CObject		*O = Level().Objects.net_Find	(id);

			R_ASSERT	(O);

#ifndef SILENCE
			Msg("Trying to take - %s (%d)", *O->cName(),O->ID());
#endif
			CGameObject	*_O = smart_cast<CGameObject*>(O);
			if (inventory().CanTakeItem(smart_cast<CInventoryItem*>(_O))) { //GetScriptControl()
				O->H_SetParent(this);
				inventory().Take(_O,true, false);
				if (!inventory().ActiveItem() && GetScriptControl() && smart_cast<CShootingObject*>(O))
					CObjectHandler::set_goal	(eObjectActionIdle,_O);

				on_after_take			(_O);
#ifndef SILENCE
				Msg("TAKE - %s (%d)", *O->cName(),O->ID());
#endif
			}
			else {
//				DropItemSendMessage(O);
				NET_Packet				_P;
				u_EventGen				(_P,GE_OWNERSHIP_REJECT,ID());
				_P.w_u16					(u16(O->ID()));
				u_EventSend				(_P);

#ifndef SILENCE
				Msg("TAKE - can't take! - Dropping for valid server information %s (%d)", *O->cName(),O->ID());
#endif
			}
			break;
		}
		case GE_TRADE_SELL :
		case GE_OWNERSHIP_REJECT :
		case GE_TRANSFER_REJECT: {
			u16 id;
			P.r_u16		(id);
			CObject		*O = Level().Objects.net_Find(id);

			if (!O) {
				Msg("! [%s] Error: No object to reject/sell [%u]", __FUNCTION__, id);
				break;
			}

			bool just_before_destroy	= !P.r_eof() && P.r_u8();
			bool dont_create_shell = (type == GE_TRADE_SELL) || (type == GE_TRANSFER_REJECT) || just_before_destroy;

			O->SetTmpPreDestroy				(just_before_destroy);

			if (inventory().DropItem(smart_cast<CGameObject*>(O)) && !O->getDestroy()) {
				O->H_SetParent	(0, dont_create_shell);
				feel_touch_deny	(O,2000);
			}

			break;
		}
	}
}

void CAI_Stalker::feel_touch_new				(CObject* O)
{
//	Msg					("FEEL_TOUCH::NEW : %s",*O->cName());
	if ( !brain().CStalkerPlanner::m_storage.property( StalkerDecisionSpace::eWorldPropertyItems ) )
		return;
	if (!g_Alive())		return;
	if (Remote())		return;
	if ((O->spatial.type | STYPE_VISIBLEFORAI) != O->spatial.type) return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= smart_cast<CInventoryItem*>	(O);

	if (!wounded() && !critically_wounded() && I && I->useful_for_NPC() && can_take(I)) {
#ifndef SILENCE
		Msg("Taking item %s (%d)!",I->object().cName().c_str(),I->object().ID());
#endif
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->object().ID()));
		u_EventSend		(P);
	}
}

void CAI_Stalker::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (this != O->H_Parent()))
		return;

#ifndef SILENCE
	Msg("Dropping item!");
#endif
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

/////////////////////////
//PDA functions
/////////////////////////
/*
void CAI_Stalker::ReceivePdaMessage(u16 who, EPdaMsg msg, shared_str info_id)
{
	CInventoryOwner::ReceivePdaMessage(who, msg, info_id);
}*/


void CAI_Stalker::UpdateAvailableDialogs(CPhraseDialogManager* partner)
{
/*	m_AvailableDialogs.clear();
	m_CheckedDialogs.clear();

	if(CInventoryOwner::m_known_info_registry->registry().objects_ptr())
	{
		for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::m_known_info_registry->registry().objects_ptr()->begin();
			CInventoryOwner::m_known_info_registry->registry().objects_ptr()->end() != it; ++it)
		{
			//подгрузить кусочек информации с которым мы работаем
			CInfoPortion info_portion;
			info_portion.Load((*it).id);

			for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
				AddAvailableDialog(*info_portion.DialogNames()[i], partner);
		}
	}
*/
	CAI_PhraseDialogManager::UpdateAvailableDialogs(partner);
}