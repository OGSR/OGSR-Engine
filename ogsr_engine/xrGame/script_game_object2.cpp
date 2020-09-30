////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script2.cpp
//	Created 	: 17.11.2004
//  Modified 	: 17.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "script_engine.h"
#include "explosive.h"
#include "script_zone.h"
#include "object_handler.h"
#include "script_hit.h"
#include "../Include/xrRender/Kinematics.h"
#include "pda.h"
#include "InfoPortion.h"
#include "memory_manager.h"
#include "ai_phrasedialogmanager.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "xrMessages.h"
#include "custommonster.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "item_manager.h"
#include "danger_manager.h"
#include "memory_space.h"
#include "actor.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "../xr_3da/camerabase.h"
#include "ai/stalker/ai_stalker.h"
#include "car.h"
#include "movement_manager.h"
#include "detail_path_manager.h"

void CScriptGameObject::explode	(u32 level_time)
{
	CExplosive			*explosive = smart_cast<CExplosive*>(&object());
	if (object().H_Parent())
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot explode object wiht parent!");
		return;
	}

	if (!explosive)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot access class member explode!");
	else {
		Fvector normal;
		explosive->FindNormal(normal);
		explosive->SetInitiator(explosive->Initiator()); // устанавливаем инициатор только если он неизвестен.
		explosive->GenExplodeEvent(object().Position(), normal);
	}
}

bool CScriptGameObject::active_zone_contact		(u16 id)
{
	CScriptZone		*script_zone = smart_cast<CScriptZone*>(&object());
	if (!script_zone) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member active_zone_contact!");
		return		(false);
	}
	return			(script_zone->active_contact(id));
}

CScriptGameObject *CScriptGameObject::best_weapon()
{
	CObjectHandler	*object_handler = smart_cast<CAI_Stalker*>(&object());
	if (!object_handler) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member best_weapon!");
		return			(0);
	}
	else {
		CGameObject		*game_object = object_handler->best_weapon() ? &object_handler->best_weapon()->object() : 0;
		return			(game_object ? game_object->lua_game_object() : 0);
	}
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action)
{
	CObjectHandler			*object_handler = smart_cast<CAI_Stalker*>(&object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action);
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object)
{
	CObjectHandler			*object_handler = smart_cast<CAI_Stalker*>(&object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? &lua_game_object->object() : 0);
}

void CScriptGameObject::set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object, u32 queue_size)
{
	CObjectHandler			*object_handler = smart_cast<CAI_Stalker*>(&object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? &lua_game_object->object() : 0, queue_size, queue_size);
}

void CScriptGameObject::set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object, u32 queue_size, u32 queue_interval)
{
	CObjectHandler			*object_handler = smart_cast<CAI_Stalker*>(&object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? &lua_game_object->object() : 0, queue_size, queue_size, queue_interval, queue_interval);
}

void CScriptGameObject::play_cycle(LPCSTR anim, bool mix_in)
{
	IKinematicsAnimated* sa=smart_cast<IKinematicsAnimated*>(object().Visual());
	if(sa){
		MotionID m	= sa->ID_Cycle(anim);
		if (m) sa->PlayCycle(m,(BOOL)mix_in);
		else 
		{
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError ,"CGameObject : has not cycle %s",anim);
		}
	}
	else
	{
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : is not animated object");
	}
}

void CScriptGameObject::play_cycle(LPCSTR anim)
{
	play_cycle	(anim,true);
}

void CScriptGameObject::Hit(CScriptHit *tpLuaHit)
{
	CScriptHit		&tLuaHit = *tpLuaHit;
	NET_Packet		P;
	SHit			HS;
	HS.GenHeader(GE_HIT,object().ID());										//	object().u_EventGen(P,GE_HIT,object().ID());
	THROW2			(tLuaHit.m_tpDraftsman,"Where is hit initiator??!");	//	THROW2			(tLuaHit.m_tpDraftsman,"Where is hit initiator??!");
	HS.whoID  = u16(tLuaHit.m_tpDraftsman->ID());							//	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	HS.weaponID = 0;														//	P.w_u16			(0);
	HS.dir = tLuaHit.m_tDirection;											//	P.w_dir			(tLuaHit.m_tDirection);
	HS.power = tLuaHit.m_fPower;											//	P.w_float		(tLuaHit.m_fPower);
	IKinematics		*V = smart_cast<IKinematics*>(object().Visual());		//	IKinematics		*V = smart_cast<IKinematics*>(object().Visual());
	if (V && xr_strlen(tLuaHit.m_caBoneName))								//	if (xr_strlen	(tLuaHit.m_caBoneName))
		HS.boneID = 		(V->LL_BoneID(tLuaHit.m_caBoneName));			//		P.w_s16		(V->LL_BoneID(tLuaHit.m_caBoneName));
	else																	//	else
		HS.boneID = 		(s16(0));										//		P.w_s16		(s16(0));
	HS.p_in_bone_space = Fvector().set(0,0,0);								//	P.w_vec3		(Fvector().set(0,0,0));
	HS.impulse = tLuaHit.m_fImpulse;										//	P.w_float		(tLuaHit.m_fImpulse);
	HS.hit_type = (ALife::EHitType)(tLuaHit.m_tHitType);					//	P.w_u16			(u16(tLuaHit.m_tHitType));
	HS.Write_Packet(P);						

	object().u_EventSend(P);
}


#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CScriptGameObject::operator CObject*()
{
	return			(&object());
}

CScriptGameObject *CScriptGameObject::GetBestEnemy()
{
	const CCustomMonster	*monster = smart_cast<const CCustomMonster*>(&object());
	if (!monster)
		return				(0);

	if (monster->memory().enemy().selected())
		return				(monster->memory().enemy().selected()->lua_game_object());
	return					(0);
}

const CDangerObject *CScriptGameObject::GetBestDanger()
{
	const CCustomMonster	*monster = smart_cast<const CCustomMonster*>(&object());
	if (!monster)
		return				(0);

	if (!monster->memory().danger().selected())
		return				(0);

	return					(monster->memory().danger().selected());
}

CScriptGameObject *CScriptGameObject::GetBestItem()
{
	const CCustomMonster	*monster = smart_cast<const CCustomMonster*>(&object());
	if (!monster)
		return				(0);

	if (monster->memory().item().selected())
		return				(monster->memory().item().selected()->lua_game_object());
	return					(0);
}

u32 CScriptGameObject::memory_time(const CScriptGameObject &lua_game_object)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member memory!");
		return				(0);
	}
	else
		return				(monster->memory().memory_time(&lua_game_object.object()));
}

Fvector CScriptGameObject::memory_position(const CScriptGameObject &lua_game_object)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member memory!");
		return				(Fvector().set(0.f,0.f,0.f));
	}
	else
		return				(monster->memory().memory_position(&lua_game_object.object()));
}

void CScriptGameObject::enable_memory_object	(CScriptGameObject *game_object, bool enable)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member enable_memory_object!");
	else
		monster->memory().enable			(&game_object->object(),enable);
}

const xr_vector<CNotYetVisibleObject> &CScriptGameObject::not_yet_visible_objects() const
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member not_yet_visible_objects!");
		NODEFAULT;
	}
	return					(monster->memory().visual().not_yet_visible_objects());
}

float CScriptGameObject::visibility_threshold	() const
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member visibility_threshold!");
		NODEFAULT;
	}
	return					(monster->memory().visual().visibility_threshold());
}

void CScriptGameObject::enable_vision			(bool value)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member enable_vision!");
		return;
	}
	monster->memory().visual().enable		(value);
}

bool CScriptGameObject::vision_enabled			() const
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member vision_enabled!");
		return								(false);
	}
	return									(monster->memory().visual().enabled());
}

void CScriptGameObject::set_sound_threshold		(float value)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member set_sound_threshold!");
		return;
	}
	monster->memory().sound().set_threshold		(value);
}

void CScriptGameObject::restore_sound_threshold	()
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member restore_sound_threshold!");
		return;
	}
	monster->memory().sound().restore_threshold	();
}

void CScriptGameObject::SetStartDialog(LPCSTR dialog_id)
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(&object());
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}

void CScriptGameObject::GetStartDialog		()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(&object());
	if(!pDialogManager) return;
	pDialogManager->GetStartDialog();
}
void CScriptGameObject::RestoreDefaultStartDialog()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(&object());
	if(!pDialogManager) return;
	pDialogManager->RestoreDefaultStartDialog();
}

void CScriptGameObject::SetActorPosition			(Fvector pos)
{
	CActor* actor = smart_cast<CActor*>(&object());
	if(actor){
		Fmatrix F = actor->XFORM();
		F.c = pos;
		actor->ForceTransform(F);
//		actor->XFORM().c = pos;
	}else
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ScriptGameObject : attempt to call SetActorPosition method for non-actor object");

}

void CScriptGameObject::SetActorDirection		(float dir)
{
	CActor* actor = smart_cast<CActor*>(&object());
	if(actor){
		actor->cam_Active()->Set(dir,0,0);
//		actor->XFORM().setXYZ(0,dir,0);
	}else
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"ScriptGameObject : attempt to call SetActorDirection method for non-actor object");
}

CHolderCustom* CScriptGameObject::get_current_holder()
{
	CActor* actor = smart_cast<CActor*>(&object());

	if(actor)
		return actor->Holder();
	else
		return NULL;
}

void CScriptGameObject::set_ignore_monster_threshold	(float ignore_monster_threshold)
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member set_ignore_monster_threshold!");
		return;
	}
	clamp				(ignore_monster_threshold,0.f,1.f);
	stalker->memory().enemy().ignore_monster_threshold	(ignore_monster_threshold);
}

void CScriptGameObject::restore_ignore_monster_threshold	()
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member restore_ignore_monster_threshold!");
		return;
	}
	stalker->memory().enemy().restore_ignore_monster_threshold	();
}

float CScriptGameObject::ignore_monster_threshold		() const
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member ignore_monster_threshold!");
		return			(0.f);
	}
	return				(stalker->memory().enemy().ignore_monster_threshold());
}

void CScriptGameObject::set_max_ignore_monster_distance	(const float &max_ignore_monster_distance)
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member set_max_ignore_monster_distance!");
		return;
	}
	stalker->memory().enemy().max_ignore_monster_distance	(max_ignore_monster_distance);
}

void CScriptGameObject::restore_max_ignore_monster_distance	()
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member restore_max_ignore_monster_distance!");
		return;
	}
	stalker->memory().enemy().restore_max_ignore_monster_distance	();
}

float CScriptGameObject::max_ignore_monster_distance	() const
{
	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member max_ignore_monster_distance!");
		return			(0.f);
	}
	return				(stalker->memory().enemy().max_ignore_monster_distance());
}

CCar* CScriptGameObject::get_car	()
{
	CCar		*car = smart_cast<CCar*>(&object());
	if (!car) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member get_car!");
		return nullptr;
	}
	return car;
}

#ifdef DEBUG
void CScriptGameObject::debug_planner				(const script_planner *planner)
{
	CAI_Stalker		*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member debug_planner!");
		return;
	}

	stalker->debug_planner	(planner);
}
#endif

u32 CScriptGameObject::location_on_path				(float distance, Fvector *location)
{
	if (!location) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : location_on_path -> specify destination location!");
		return								(u32(-1));
	}

	CCustomMonster							*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member location_on_path!");
		return								(u32(-1));
	}

	VERIFY									(location);
	return									(monster->movement().detail().location_on_path(monster,distance,*location));
}


void CScriptGameObject::explode_initiator( u16 who_id ) {
  ASSERT_FMT( !object().H_Parent(), "[%s]: cannot explode %s with parent", __FUNCTION__, cName().c_str() );
  CExplosive* explosive = smart_cast<CExplosive*>( &object() );
  ASSERT_FMT( explosive, "[%s]: %s not a CExplosive", __FUNCTION__, cName().c_str() );
  Fvector normal;
  explosive->FindNormal( normal );
  explosive->SetInitiator( who_id );
  explosive->GenExplodeEvent( object().Position(), normal );
}


bool CScriptGameObject::is_exploded() {
  CExplosive* explosive = smart_cast<CExplosive*>( &object() );
  ASSERT_FMT( explosive, "[%s]: %s not a CExplosive", __FUNCTION__, cName().c_str() );
  return explosive->IsExploded();
}


bool CScriptGameObject::is_ready_to_explode() {
  CExplosive* explosive = smart_cast<CExplosive*>( &object() );
  ASSERT_FMT( explosive, "[%s]: %s not a CExplosive", __FUNCTION__, cName().c_str() );
  return explosive->IsReadyToExplode();
}


void CScriptGameObject::remove_memory_object( CScriptGameObject *game_object ) {
  CCustomMonster* monster = smart_cast<CCustomMonster*>( &object() );
  ASSERT_FMT( monster, "[%s]: %s not a CCustomMonster", __FUNCTION__, cName().c_str() );
  monster->memory().remove_links( &game_object->object() );
}
