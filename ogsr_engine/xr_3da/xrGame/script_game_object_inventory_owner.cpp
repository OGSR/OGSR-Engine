////////////////////////////////////////////////////////////////////////////
// script_game_object_inventory_owner.сpp :	функции для inventory owner
//////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "InventoryOwner.h"
#include "Pda.h"
#include "xrMessages.h"
#include "character_info.h"
#include "gametask.h"
#include "actor.h"
#include "level.h"
#include "date_time.h"
#include "uigamesp.h"
#include "hudmanager.h"
#include "restricted_object.h"
#include "script_engine.h"
#include "attachable_item.h"
#include "script_entity.h"
#include "string_table.h"
#include "alife_registry_wrappers.h"
#include "relation_registry.h"
#include "custommonster.h"
#include "movement_manager.h"
#include "actorcondition.h"
#include "level_graph.h"
#include "huditem.h"
#include "ui/UItalkWnd.h"
#include "ui/UITradeWnd.h"
#include "inventory.h"
#include "infoportion.h"
#include "AI/Monsters/BaseMonster/base_monster.h"
#include "weaponmagazined.h"
#include "ai/stalker/ai_stalker.h"
#include "Torch.h"

bool CScriptGameObject::GiveInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(info_id, true);
	return			true;
}

bool CScriptGameObject::DisableInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	if (!HasInfo(info_id)) return true;
	pInventoryOwner->TransferInfo(info_id, false);
	return true;
}

void _AddIconedTalkMessage(LPCSTR text, LPCSTR texture_name, const Frect& tex_rect, LPCSTR templ_name);

void  CScriptGameObject::AddIconedTalkMessage		(LPCSTR text, LPCSTR texture_name, Frect tex_rect, LPCSTR templ_name)
{
	return _AddIconedTalkMessage	(text,
									texture_name, 
									tex_rect, 
									templ_name); 
}

void _AddIconedTalkMessage(LPCSTR text, LPCSTR texture_name, const Frect& tex_rect, LPCSTR templ_name)
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu->IsShown())
		pGameSP->TalkMenu->AddIconedMessage(text, texture_name, tex_rect, templ_name?templ_name:"iconed_answer_item" );
}
bool _give_news	(LPCSTR news, LPCSTR texture_name, const Frect& tex_rect, int delay, int show_time);

bool  CScriptGameObject::GiveGameNews		(LPCSTR news, LPCSTR texture_name, Frect tex_rect, int delay, int show_time)
{
	return _give_news				(news,
									texture_name, 
									tex_rect, 
									delay, 
									show_time);
}
bool _give_news	(LPCSTR text, LPCSTR texture_name, const Frect& tex_rect, int delay, int show_time)
{
	GAME_NEWS_DATA				news_data;
	news_data.news_text			= text;
	if(show_time!=0)
		news_data.show_time		= show_time;// override default

	VERIFY(xr_strlen(texture_name)>0);

	news_data.texture_name			= texture_name;
	news_data.tex_rect				= tex_rect;


	if(delay==0)
		Actor()->AddGameNews(news_data);
	else
		Actor()->AddGameNews_deffered(news_data,delay);

	return true;
}

bool  CScriptGameObject::HasInfo				(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;

	return pInventoryOwner->HasInfo(info_id);

}
bool  CScriptGameObject::DontHasInfo			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return true;

	return !pInventoryOwner->HasInfo(info_id);
}

xrTime CScriptGameObject::GetInfoTime			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return xrTime(0);

	INFO_DATA info_data;
	if(pInventoryOwner->GetInfo(info_id, info_data))
		return xrTime(info_data.receive_time);
	else
		return xrTime(0);
}



bool CScriptGameObject::IsTalking()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CScriptGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}

void CScriptGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CScriptGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CScriptGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}

void CScriptGameObject::EnableTrade			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTrade();
}
void CScriptGameObject::DisableTrade		()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTrade();
}
bool CScriptGameObject::IsTradeEnabled		()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTradeEnabled();
}

void CScriptGameObject::ForEachInventoryItems(const luabind::functor<void> &functor)
{
	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(&object());
	if(!owner){
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::ForEachInventoryItems non-CInventoryOwner object !!!");
		return;
	}
	
	CInventory* pInv = &owner->inventory();
	TIItemContainer item_list;
	pInv->AddAvailableItems(item_list, true);

	TIItemContainer::iterator it;
	for(it =  item_list.begin(); item_list.end() != it; ++it) 
	{
		CGameObject* inv_go = smart_cast<CGameObject*>(*it);
		if( inv_go ){
			functor(inv_go->lua_game_object(),this);
		}
	}
}

//1
void CScriptGameObject::IterateInventory	(luabind::functor<void> functor, luabind::object object)
{
	CInventoryOwner			*inventory_owner = smart_cast<CInventoryOwner*>(&this->object());
	if (!inventory_owner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::IterateInventory non-CInventoryOwner object !!!");
		return;
	}

	TIItemContainer::iterator	I = inventory_owner->inventory().m_all.begin();
	TIItemContainer::iterator	E = inventory_owner->inventory().m_all.end();
	for ( ; I != E; ++I)
		functor				(object,(*I)->object().lua_game_object());
}

void CScriptGameObject::MarkItemDropped		(CScriptGameObject *item)
{
	CInventoryOwner			*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::MarkItemDropped non-CInventoryOwner object !!!");
		return;
	}

	CInventoryItem			*inventory_item = smart_cast<CInventoryItem*>(&item->object());
	if (!inventory_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::MarkItemDropped non-CInventoryItem object !!!");
		return;
	}

	inventory_item->SetDropManual	(TRUE);
}

bool CScriptGameObject::MarkedDropped		(CScriptGameObject *item)
{
	CInventoryOwner			*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::MarkedDropped non-CInventoryOwner object !!!");
		return				(false);
	}

	CInventoryItem			*inventory_item = smart_cast<CInventoryItem*>(&item->object());
	if (!inventory_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::MarkedDropped non-CInventoryItem object !!!");
		return				(false);
	}

	return					(!!inventory_item->GetDropManual());
}

void CScriptGameObject::UnloadMagazine		()
{
	CWeaponMagazined		*weapon_magazined = smart_cast<CWeaponMagazined*>(&object());
	if (!weapon_magazined) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::UnloadMagazine non-CWeaponMagazined object !!!");
		return;
	}

	CAI_Stalker				*stalker = smart_cast<CAI_Stalker*>(weapon_magazined->H_Parent());
	if (stalker && stalker->hammer_is_clutched())
		return;

	weapon_magazined->UnloadMagazine	(false);
}
//

void CScriptGameObject::DropItem			(CScriptGameObject* pItem)
{
//	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(&object());
	CInventoryItem* item = smart_cast<CInventoryItem*>(&pItem->object());
	if(/*!owner||*/!item){
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject::DropItem non-CInventoryOwner object !!!");
		return;
	}

	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, object().ID());
	P.w_u16							(pItem->object().ID());
	CGameObject::u_EventSend		(P);
}

void CScriptGameObject::DropItemAndTeleport	(CScriptGameObject* pItem, Fvector position)
{
	DropItem						(pItem);

	NET_Packet						PP;
	CGameObject::u_EventGen			(PP,GE_CHANGE_POS, pItem->object().ID());
	PP.w_vec3						(position);
	CGameObject::u_EventSend		(PP);
}

//передаче вещи из своего инвентаря в инвентарь партнера
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	if (!pItem || !pForWho) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"cannot transfer NULL item");
		return;
	}

	CInventoryItem* pIItem = smart_cast<CInventoryItem*>(&pItem->object());

	if (!pIItem) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot transfer not CInventoryItem item");
		return ;
	}

	// выбросить у себя 
	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, object().ID());
	P.w_u16							(pIItem->object().ID());
	CGameObject::u_EventSend		(P);

	// отдать партнеру
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_TAKE, pForWho->object().ID());
	P.w_u16							(pIItem->object().ID());
	CGameObject::u_EventSend		(P);
}

u32 CScriptGameObject::Money	()
{
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(&object()); VERIFY(pOurOwner);
	return pOurOwner->get_money();
}

void CScriptGameObject::TransferMoney(int money, CScriptGameObject* pForWho)
{
	if (!pForWho) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"cannot transfer money for NULL object");
		return;
	}
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(&object()); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= smart_cast<CInventoryOwner*>(&pForWho->object()); VERIFY(pOtherOwner);

	if (pOurOwner->get_money()-money<0) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Character does not have enought money");
		return;
	}

	pOurOwner->set_money		(pOurOwner->get_money() - money, true );
	pOtherOwner->set_money		(pOtherOwner->get_money() + money, true );
}

void CScriptGameObject::GiveMoney(int money)
{
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(&object()); VERIFY(pOurOwner);


	pOurOwner->set_money		(pOurOwner->get_money() + money, true );
}
//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"GetGoodwill available only for InventoryOwner");
		return 0;
	}
	return RELATION_REGISTRY().GetGoodwill(pInventoryOwner->object_id(), pToWho->object().ID());
}

void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SetGoodwill available only for InventoryOwner");
		return ;
	}
	return RELATION_REGISTRY().SetGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), goodwill);
}

void CScriptGameObject::ChangeGoodwill(int delta_goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ChangeGoodwill available only for InventoryOwner");
		return ;
	}
	RELATION_REGISTRY().ChangeGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), delta_goodwill);
}


//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SetRelation available only for InventoryOwner");
		return ;
	}

	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(&pWhoToSet->object());
	VERIFY(pOthersInventoryOwner);
	if (!pOthersInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SetRelation available only for InventoryOwner");
		return ;
	}
	RELATION_REGISTRY().SetRelationType(pInventoryOwner, pOthersInventoryOwner, relation);
}

//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetAttitude			(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());VERIFY(pInventoryOwner);
	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(&pToWho->object());VERIFY(pOthersInventoryOwner);
	return RELATION_REGISTRY().GetAttitude(pInventoryOwner, pOthersInventoryOwner);
}


//////////////////////////////////////////////////////////////////////////

LPCSTR CScriptGameObject::ProfileName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ProfileName available only for InventoryOwner");
		return NULL;
	}
	
	shared_str profile_id =  pInventoryOwner->CharacterInfo().Profile();
	if(!profile_id || !profile_id.size() )
		return NULL;
	else
		return *profile_id;
}


LPCSTR CScriptGameObject::CharacterName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CharacterName available only for InventoryOwner");
		return NULL;
	}
	return pInventoryOwner->Name();
}
int CScriptGameObject::CharacterRank			()
{
	// rank support for monster
	CBaseMonster *monster = smart_cast<CBaseMonster*>(&object());
	if (!monster) {
		CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
		if (!pInventoryOwner) {
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CharacterRank available only for InventoryOwner and BaseMonster");
			return 0;
		}
		return pInventoryOwner->Rank();
	} 	
	return monster->Rank();
}
void CScriptGameObject::SetCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SetCharacterRank available only for InventoryOwner");
		return ;
	}
	return pInventoryOwner->SetRank(char_rank);
}

void CScriptGameObject::ChangeCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ChangeCharacterRank available only for InventoryOwner");
		return;
	}
	return pInventoryOwner->ChangeRank(char_rank);
}

int CScriptGameObject::CharacterReputation			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CharacterReputation available only for InventoryOwner");
		return 0;
	}
	return pInventoryOwner->Reputation();
}


void CScriptGameObject::ChangeCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ChangeCharacterReputation available only for InventoryOwner");
		return ;
	}
	pInventoryOwner->ChangeReputation(char_rep);
}

LPCSTR CScriptGameObject::CharacterCommunity	()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());

	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CharacterCommunity available only for InventoryOwner");
		return NULL;
	}
	return *pInventoryOwner->CharacterInfo().Community().id();
}

void CScriptGameObject::SetCharacterCommunity	(LPCSTR comm, int squad, int group)
{
	CInventoryOwner*	pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	CEntity*			entity			= smart_cast<CEntity*>(&object());

	if (!pInventoryOwner || !entity) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SetCharacterCommunity available only for InventoryOwner");
		return;
	}
	CHARACTER_COMMUNITY	community;
	community.set(comm);
	pInventoryOwner->SetCommunity(community.index());
	entity->ChangeTeam(community.team(), squad, group);
}

LPCSTR CScriptGameObject::sound_voice_prefix () const
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if (!pInventoryOwner) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"sound_voice_prefix available only for InventoryOwner");
		return NULL;
	}

	return pInventoryOwner->SpecificCharacter().sound_voice_prefix();
}

#include "GameTaskManager.h"
ETaskState CScriptGameObject::GetGameTaskState	(LPCSTR task_id, int objective_num)
{
/*	CActor* pActor = smart_cast<CActor*>(&object());
	if (!pActor) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"GetGameTaskState available only for actor");
		return eTaskStateDummy;
	}
*/
	shared_str shared_name = task_id;
	CGameTask* t= Actor()->GameTaskManager().HasGameTask(shared_name);
	if(NULL==t) return eTaskStateDummy;

	if ((std::size_t)objective_num >= t->m_Objectives.size()) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"wrong objective num", task_id);
		return eTaskStateDummy;
	}
	return t->m_Objectives[objective_num].TaskState();

}

void CScriptGameObject::SetGameTaskState	(ETaskState state, LPCSTR task_id, int objective_num)
{
	shared_str shared_name = task_id;
	Actor()->GameTaskManager().SetTaskState(shared_name, (u16)objective_num, state);
}

//////////////////////////////////////////////////////////////////////////

void  CScriptGameObject::SwitchToTrade		()
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu->IsShown())
	{
		pGameSP->TalkMenu->SwitchToTrade();
	}
}
void  CScriptGameObject::SwitchToTalk		()
{
	R_ASSERT("switch_to_talk called ;)");
}

void  CScriptGameObject::RunTalkDialog			(CScriptGameObject* pToWho)
{
	CActor* pActor = smart_cast<CActor*>(&object());	
//	R_ASSERT2(pActor, "RunTalkDialog applicable only for actor");

	if (!pActor) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"RunTalkDialog applicable only for actor");
		return;
	}

	CInventoryOwner* pPartner = smart_cast<CInventoryOwner*>(&pToWho->object());	VERIFY(pPartner);
	pActor->RunTalkDialog(pPartner);
}
/*
void  CScriptGameObject::ActorSleep			(int hours, int minutes)
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;
	pActor->conditions().GoSleep(generate_time(1,1,1,hours, minutes, 0, 0), true);
}

void  CScriptGameObject::ActorFakeSleep			(int hours, int minutes)
{
//.	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;
//.	pActor->conditions().ProcessSleep(generate_time(1,1,1,hours, minutes, 0, 0));
}

bool CScriptGameObject::IsActorSleepeng()
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return false;
	return pActor->conditions().IsSleeping();
}
*/
//////////////////////////////////////////////////////////////////////////

void construct_restriction_vector(shared_str restrictions, xr_vector<ALife::_OBJECT_ID> &result)
{
	result.clear();
	string64	temp;
	u32			n = _GetItemCount(*restrictions);
	for (u32 i=0; i<n; ++i) {
		CObject	*object = Level().Objects.FindObjectByName(_GetItem(*restrictions,i,temp));
		if (!object)
			continue;
		result.push_back(object->ID());
	}
}

void CScriptGameObject::add_restrictions		(LPCSTR out, LPCSTR in)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member add_restrictions!");
		return;
	}
	
//	xr_vector<ALife::_OBJECT_ID>			temp0;
//	xr_vector<ALife::_OBJECT_ID>			temp1;

//	construct_restriction_vector			(out,temp0);
//	construct_restriction_vector			(in,temp1);

//	if (!xr_strcmp(monster->cName(),"mil_freedom_stalker0004")) {
//		int i = 0;
//		if (!xr_strcmp(in,"mil_freedom_wall_restrictor")) {
//			int j = 0;
//		}
//	}
	
	monster->movement().restrictions().add_restrictions(out,in);
}

void CScriptGameObject::remove_restrictions		(LPCSTR out, LPCSTR in)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_restrictions!");
		return;
	}

//	xr_vector<ALife::_OBJECT_ID>			temp0;
//	xr_vector<ALife::_OBJECT_ID>			temp1;

//	construct_restriction_vector			(out,temp0);
//	construct_restriction_vector			(in,temp1);

	monster->movement().restrictions().remove_restrictions(out,in);
}

void CScriptGameObject::remove_all_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_all_restrictions!");
		return;
	}
	monster->movement().restrictions().remove_all_restrictions	();
}

LPCSTR CScriptGameObject::in_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member in_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().in_restrictions());
}

LPCSTR CScriptGameObject::out_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member out_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().out_restrictions());
}

LPCSTR CScriptGameObject::base_in_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member base_in_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().base_in_restrictions());
}

LPCSTR CScriptGameObject::base_out_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member base_out_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().base_out_restrictions());
}

bool CScriptGameObject::accessible_position	(const Fvector &position)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(monster->movement().restrictions().accessible(position));
}

bool CScriptGameObject::accessible_vertex_id(u32 level_vertex_id)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	THROW2									(ai().level_graph().valid_vertex_id(level_vertex_id),"Cannot check if level vertex id is accessible, because it is invalid");
	return									(monster->movement().restrictions().accessible(level_vertex_id));
}

u32	 CScriptGameObject::accessible_nearest	(const Fvector &position, Fvector &result)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(u32(-1));
	}
	return									(monster->movement().restrictions().accessible_nearest(position,result));
}

void CScriptGameObject::enable_attachable_item	(bool value)
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(&object());
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member enable_attachable_item!");
		return;
	}
	attachable_item->enable					(value);
}

bool CScriptGameObject::attachable_item_enabled	() const
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(&object());
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member attachable_item_enabled!");
		return								(false);
	}
	return									(attachable_item->enabled());
}



void  CScriptGameObject::RestoreWeapon		()
{
	Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL,false);
}

void  CScriptGameObject::HideWeapon			()
{
	Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL,true);
}

int	CScriptGameObject::animation_slot			() const
{
	CHudItem		*hud_item = smart_cast<CHudItem*>(&object());
	if (!hud_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CHudItem : cannot access class member animation_slot!");
		return		(u32(-1));
	}
	return			(hud_item->animation_slot());
}

CScriptGameObject *CScriptGameObject::item_in_slot	(u32 slot_id) const
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member item_in_slot!");
		return		(0);
	}

	if (inventory_owner->inventory().m_slots.size() <= slot_id) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : invalid slot id for class member item_in_slot : %d!",slot_id);
		return		(0);
	}

	CInventoryItem	*result = inventory_owner->inventory().m_slots[slot_id].m_pIItem;
	return			(result ? result->object().lua_game_object() : 0);
}

void CScriptGameObject::GiveTaskToActor(CGameTask* t, u32 dt, bool bCheckExisting)
{
	Actor()->GameTaskManager().GiveGameTaskToActor(t, dt, bCheckExisting);
}

u32	CScriptGameObject::active_slot()
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member active_slot!");
		return		(0);
	}
	return inventory_owner->inventory().GetActiveSlot();
}

void CScriptGameObject::activate_slot	(u32 slot_id)
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CInventoryOwner : cannot access class member activate_slot!");
		return						;
	}
	inventory_owner->inventory().Activate(slot_id);
}

void CScriptGameObject::SwitchTorch(bool enable)
{
	CTorch	*torch = smart_cast<CTorch*>(&object());
	if (!torch) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member activate_slot!");
		return;
	}
	torch->Switch(enable);
}

void CScriptGameObject::enable_movement	(bool enable)
{
	CCustomMonster						*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member movement_enabled!");
		return;
	}

	monster->movement().enable_movement	(enable);
}

bool CScriptGameObject::movement_enabled()
{
	CCustomMonster						*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member movement_enabled!");
		return							(false);
	}

	return								(monster->movement().enabled());
}
// KD
// functions for CInventoryOwner class
CScriptGameObject *CScriptGameObject::ItemOnBelt(u32 item_id) const
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member item_on_belt!");
		return		(0);
	}

	TIItemContainer *belt = &(inventory_owner->inventory().m_belt);
	if (belt->size() < item_id) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "item_on_belt: item id outside belt!");
		return		(0);
	}

	CInventoryItem	*result = belt->at(item_id);
	return			(result ? result->object().lua_game_object() : 0);
}

CScriptGameObject *CScriptGameObject::ItemInRuck(u32 item_id) const
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member item_on_belt!");
		return		(0);
	}

	TIItemContainer *ruck = &(inventory_owner->inventory().m_ruck);
	if (ruck->size() < item_id) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "item_in_ruck: item id outside ruck!");
		return		(0);
	}

	CInventoryItem	*result = ruck->at(item_id);
	return			(result ? result->object().lua_game_object() : 0);
}

bool CScriptGameObject::IsOnBelt(CScriptGameObject *obj) const
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member is_on_belt!");
		return		(0);
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member is_on_belt!");
		return		(0);
	}

	return inventory_owner->inventory().InBelt(inventory_item);
}

bool CScriptGameObject::IsInRuck(CScriptGameObject *obj) const
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member is_in_ruck!");
		return		(0);
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member is_in_ruck!");
		return		(0);
	}

	return inventory_owner->inventory().InRuck(inventory_item);
}

bool CScriptGameObject::IsInSlot(CScriptGameObject *obj) const
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member is_in_slot!");
		return		(0);
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member is_in_slot!");
		return		(0);
	}

	return inventory_owner->inventory().InSlot(inventory_item);
}

void CScriptGameObject::MoveToRuck(CScriptGameObject *obj)
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member move_to_ruck!");
		return;
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member move_to_ruck!");
		return;
	}

	inventory_owner->inventory().Ruck(inventory_item);
}

void CScriptGameObject::MoveToSlot(CScriptGameObject *obj, bool bNotActivate)
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member move_to_slot!");
		return;
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member move_to_slot!");
		return;
	}

	inventory_owner->inventory().Slot(inventory_item, bNotActivate);
}

void CScriptGameObject::MoveToBelt(CScriptGameObject *obj)
{
	CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(&(obj->object()));
	if (!inventory_item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member move_to_belt!");
		return;
	}

	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member move_to_belt!");
		return;
	}

	inventory_owner->inventory().Belt(inventory_item);
}

u32 CScriptGameObject::BeltSize() const
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member move_to_belt!");
		return (0);
	}

	return inventory_owner->inventory().m_belt.size();
}

u32 CScriptGameObject::RuckSize() const
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member move_to_belt!");
		return (0);
	}

	return inventory_owner->inventory().m_ruck.size();
}

void CScriptGameObject::InvalidateInventory()
{
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member InvalidateInventory!");
		return;
	}

	inventory_owner->inventory().InvalidateState();
}