////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"

#include "script_zone.h"
#include "ai/trader/ai_trader.h"

#include "ai_space.h"
#include "alife_simulator.h"

#include "ai/stalker/ai_stalker.h"
#include "stalker_movement_manager.h"

#include "sight_manager_space.h"
#include "sight_control_action.h"
#include "sight_manager.h"
#include "inventoryBox.h"
#include "WeaponBinoculars.h"

#include "HangingLamp.h"

class CWeapon;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CScriptGameObject::is_body_turning		() const
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member is_turning!");
		return			(false);
	}

	CAI_Stalker			*stalker = smart_cast<CAI_Stalker*>(monster);
	if (!stalker)
		return			(!fsimilar(monster->movement().body_orientation().target.yaw,monster->movement().body_orientation().current.yaw));
	else
		return			(!fsimilar(stalker->movement().head_orientation().target.yaw,stalker->movement().head_orientation().current.yaw) || !fsimilar(monster->movement().body_orientation().target.yaw,monster->movement().body_orientation().current.yaw));
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
		return					(0);
	}
	else
		return					(monster->sound().add(prefix,max_count,type,priority,mask,internal_type,bone_name));
}

u32	CScriptGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type)
{
	return						(add_sound(prefix,max_count,type,priority,mask,internal_type,"bip01_head"));
}

void CScriptGameObject::remove_sound	(u32 internal_type)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		monster->sound().remove	(internal_type);
}

void CScriptGameObject::set_sound_mask	(u32 sound_mask)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member set_sound_mask!");
	else {
		CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(monster);
		if (entity_alive) {
			VERIFY2				(entity_alive->g_Alive(),"Stalkers talk after death??? Say why??");
		}
		monster->sound().set_sound_mask(sound_mask);
	}
}

void CScriptGameObject::play_sound		(u32 internal_type)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type,max_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type,max_start_time,min_start_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type,max_start_time,min_start_time,max_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time);
}

void CScriptGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		monster->sound().play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time,id);
}

int  CScriptGameObject::active_sound_count		(bool only_playing)
{
	CCustomMonster				*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member active_sound_count!");
		return								(-1);
	}
	else
		return								(monster->sound().active_sound_count(only_playing));
}

int CScriptGameObject::active_sound_count		()
{
	return									(active_sound_count(false));
}

bool CScriptGameObject::wounded					() const
{
	const CAI_Stalker			*stalker = smart_cast<const CAI_Stalker *>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member wounded!");
		return					(false);
	}

	return						(stalker->wounded());
}

void CScriptGameObject::wounded					(bool value)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker *>(&object());
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member wounded!");
		return;
	}

	stalker->wounded			(value);
}

CSightParams CScriptGameObject::sight_params	()
{
	CAI_Stalker						*stalker = smart_cast<CAI_Stalker*>(&object());
	if (!stalker) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member sight_params!");

		CSightParams				result;
		result.m_object				= 0;
		result.m_vector				= Fvector().set(flt_max,flt_max,flt_max);
		result.m_sight_type			= SightManager::eSightTypeDummy;
		return						(result);
	}

	const CSightControlAction		&action = stalker->sight().current_action();
	CSightParams					result;
	result.m_sight_type				= action.sight_type();
	result.m_object					= action.object_to_look() ? action.object_to_look()->lua_game_object() : 0;
	result.m_vector					= action.vector3d();
	return							(result);
}

bool CScriptGameObject::critically_wounded		()
{
	CCustomMonster						*custom_monster = smart_cast<CCustomMonster*>(&object());
	if (!custom_monster) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member critically_wounded!");
		return							(false);
	}

	return								(custom_monster->critically_wounded());
}

bool CScriptGameObject::IsInvBoxEmpty()
{
	CInventoryBox* ib = smart_cast<CInventoryBox*>(&object());
	if(!ib) 
		return			(false);
	else
		return			ib->IsEmpty		();
}
// KD
// functions for testing object class
#include "car.h"
#include "helicopter.h"
#include "actor.h"
#include "customoutfit.h"
#include "customzone.h"
#include "ai\monsters\basemonster\base_monster.h"
#include "Artifact.h"
#include "medkit.h"
#include "antirad.h"
#include "scope.h"
#include "silencer.h"
#include "torch.h"
#include "GrenadeLauncher.h"
#include "searchlight.h"
#include "WeaponAmmo.h"
#include "grenade.h"
#include "BottleItem.h"
#include "WeaponMagazinedWGrenade.h"
#include "Actor.h"
#include "..\CameraBase.h"

#define TEST_OBJECT_CLASS(A, B)\
bool A () const\
{\
	B				*l_tpEntity = smart_cast<B*>(&object());\
	if (!l_tpEntity)\
		return false;\
	else\
			return true;\
};\

TEST_OBJECT_CLASS(CScriptGameObject::IsGameObject, CGameObject)
TEST_OBJECT_CLASS(CScriptGameObject::IsCar, CCar)
TEST_OBJECT_CLASS(CScriptGameObject::IsHeli, CHelicopter)
TEST_OBJECT_CLASS(CScriptGameObject::IsHolderCustom, CHolderCustom)
TEST_OBJECT_CLASS(CScriptGameObject::IsEntityAlive, CEntityAlive)
TEST_OBJECT_CLASS(CScriptGameObject::IsInventoryItem, CInventoryItem)
TEST_OBJECT_CLASS(CScriptGameObject::IsInventoryOwner, CInventoryOwner)
TEST_OBJECT_CLASS(CScriptGameObject::IsActor, CActor)
TEST_OBJECT_CLASS(CScriptGameObject::IsCustomMonster, CCustomMonster)
TEST_OBJECT_CLASS(CScriptGameObject::IsWeapon, CWeapon)
TEST_OBJECT_CLASS(CScriptGameObject::IsMedkit, CMedkit)
TEST_OBJECT_CLASS(CScriptGameObject::IsEatableItem, CEatableItem)
TEST_OBJECT_CLASS(CScriptGameObject::IsAntirad, CAntirad)
TEST_OBJECT_CLASS(CScriptGameObject::IsCustomOutfit, CCustomOutfit)
TEST_OBJECT_CLASS(CScriptGameObject::IsScope, CScope)
TEST_OBJECT_CLASS(CScriptGameObject::IsSilencer, CSilencer)
TEST_OBJECT_CLASS(CScriptGameObject::IsGrenadeLauncher, CGrenadeLauncher)
TEST_OBJECT_CLASS(CScriptGameObject::IsWeaponMagazined, CWeaponMagazined)
TEST_OBJECT_CLASS(CScriptGameObject::IsSpaceRestrictor, CSpaceRestrictor)
TEST_OBJECT_CLASS(CScriptGameObject::IsStalker, CAI_Stalker)
TEST_OBJECT_CLASS(CScriptGameObject::IsAnomaly, CCustomZone)
TEST_OBJECT_CLASS(CScriptGameObject::IsMonster, CBaseMonster)
TEST_OBJECT_CLASS(CScriptGameObject::IsExplosive, CExplosive)
TEST_OBJECT_CLASS(CScriptGameObject::IsScriptZone, CScriptZone)
TEST_OBJECT_CLASS(CScriptGameObject::IsProjector, CProjector)
TEST_OBJECT_CLASS(CScriptGameObject::IsLamp, CHangingLamp)
TEST_OBJECT_CLASS(CScriptGameObject::IsTrader, CAI_Trader)
TEST_OBJECT_CLASS(CScriptGameObject::IsHudItem, CHudItem)
TEST_OBJECT_CLASS(CScriptGameObject::IsFoodItem, CFoodItem)
TEST_OBJECT_CLASS(CScriptGameObject::IsArtefact, CArtefact)
TEST_OBJECT_CLASS(CScriptGameObject::IsAmmo, CWeaponAmmo)
TEST_OBJECT_CLASS(CScriptGameObject::IsMissile, CMissile)
TEST_OBJECT_CLASS(CScriptGameObject::IsPhysicsShellHolder, CPhysicsShellHolder)
TEST_OBJECT_CLASS(CScriptGameObject::IsGrenade, CGrenade)
TEST_OBJECT_CLASS(CScriptGameObject::IsBottleItem, CBottleItem)
TEST_OBJECT_CLASS(CScriptGameObject::IsTorch, CTorch)
TEST_OBJECT_CLASS(CScriptGameObject::IsWeaponGL, CWeaponMagazinedWGrenade)
TEST_OBJECT_CLASS(CScriptGameObject::IsInventoryBox, CInventoryBox)
TEST_OBJECT_CLASS(CScriptGameObject::IsWeaponBinoculars, CWeaponBinoculars)
// KD

void CScriptGameObject::SetActorCamDir(Fvector _dir)
{
	CActor						*act = smart_cast<CActor*>(&object());
	if (!act) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member SetActorCamDir!");
		return;
	}

	act->cam_Active()->vDirection.set(_dir);
}

void CScriptGameObject::UpdateCondition()
{
	CEntityAlive						*e = smart_cast<CEntityAlive*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CEntityAlive : cannot access class member UpdateCondition!");
		return;
	}
	e->conditions().UpdateCondition();
	e->conditions().UpdateConditionTime();
	e->conditions().UpdateWounds();
}

void CScriptGameObject::ChangeBleeding(float _delta)
{
	CEntityAlive						*e = smart_cast<CEntityAlive*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CEntityAlive : cannot access class member ChangeBleeding!");
		return;
	}
	e->conditions().ChangeBleeding(_delta);
}

float CScriptGameObject::GetItemWeight()
{
	CInventoryItem						*e = smart_cast<CInventoryItem*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryItem : cannot access class member GetItemWeight!");
		return 0.f;
	}
	return e->Weight();
}

u32 CScriptGameObject::InvBoxCount()
{
	CInventoryBox						*e = smart_cast<CInventoryBox*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryBox : cannot access class member InvBoxCount!");
		return 0;
	}
	return e->Size();
}

float CScriptGameObject::GetCamFOV()
{
	return g_fov;
}

void CScriptGameObject::SetCamFOV(float _fov)
{
	g_fov = _fov;
}

#include "HudManager.h"
#include "UIGameSP.h"
void CScriptGameObject::OpenInvBox(CScriptGameObject *obj)
{
	CInventoryOwner						*e = smart_cast<CInventoryOwner*>(&object());
	CInventoryBox						*trunk = smart_cast<CInventoryBox*>(&(obj->object()));
	if (!e || !trunk) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryOwner : cannot access class member OpenInvBox!");
		return;
	}
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (pGameSP) pGameSP->StartCarBody(e, trunk);
}
#include "script_ini_file.h"
CScriptIniFile *CScriptGameObject::GetVisIni()
{
	CKinematics						*k = smart_cast<CKinematics*>(object().Visual());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CKinematics : cannot access class member GetVisIni!");
		return nullptr;
	}
	return (CScriptIniFile*)(k->GetIniFile());

}
CScriptGameObject *CScriptGameObject::ObjectFromInvBox(u16 _i)
{
	CInventoryBox						*e = smart_cast<CInventoryBox*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CInventoryBox : cannot access class member ObjectFromInvBox!");
		return nullptr;
	}
	CObject *item = Level().Objects.net_Find(e->Item(_i));
	if (item)
		return (smart_cast<CGameObject*>(item))->lua_game_object();
	return nullptr;
}

void CScriptGameObject::SetBoneVisible(LPCSTR _bone_name, BOOL _visible)
{
	CKinematics						*k = smart_cast<CKinematics*>(object().Visual());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CKinematics : cannot access class member SetBoneVisible!");
		return;
	}
	k->LL_SetBoneVisible(k->LL_BoneID(_bone_name), _visible, TRUE);
	k->CalculateBones_Invalidate();
	k->CalculateBones();
}

void CScriptGameObject::SetHudBoneVisible(LPCSTR _bone_name, BOOL _visible)
{
	CHudItem						*k = smart_cast<CHudItem*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member SetHudBoneVisible!");
		return;
	}
	CKinematics *kin = smart_cast<CKinematics *>(k->GetHUD()->Visual());
	kin->LL_SetBoneVisible(kin->LL_BoneID(_bone_name), _visible, TRUE);
	kin->CalculateBones_Invalidate();
}

u16 CScriptGameObject::GetBoneID(LPCSTR _bone_name)
{
	CKinematics						*k = smart_cast<CKinematics*>(object().Visual());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CKinematics : cannot access class member GetBoneID!");
		return 0;
	}
	return k->LL_BoneID(_bone_name);
}
#include "WeaponBinoculars.h"
float CScriptGameObject::GetBinocZoomFactor()
{
	CWeaponBinoculars						*k = smart_cast<CWeaponBinoculars*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponBinoculars : cannot access class member GetBinocZoomFactor!");
		return 0.f;
	}
	return k->GetZoomFactor();
}
void CScriptGameObject::SetBinocZoomFactor(float _zoom)
{
	CWeaponBinoculars						*k = smart_cast<CWeaponBinoculars*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponBinoculars : cannot access class member SetBinocZoomFactor!");
		return;
	}
	k->SetZoomFactor(_zoom);
}
float CScriptGameObject::GetZoomFactor()
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member GetZoomFactor!");
		return 0.f;
	}
	return k->GetZoomFactor();
}
u8 CScriptGameObject::GetAddonFlags()
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member GetAddonFlags!");
		return 0;
	}
	return k->GetAddonsState();
}
void CScriptGameObject::SetAddonFlags(u8 _flags)
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member SetAddonFlags!");
		return;
	}
	k->SetAddonsState(_flags);
}
u32 CScriptGameObject::GetMagazineSize()
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member GetMagazineSize!");
		return 0;
	}
	return k->GetAmmoMagSize();
}
void CScriptGameObject::SetMagazineSize(int _size)
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member SetMagazineSize!");
		return;
	}
	k->SetAmmoMagSize(_size);
}
bool CScriptGameObject::GrenadeLauncherAttachable()
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member GrenadeLauncherAttachable!");
		return false;
	}
	return k->GrenadeLauncherAttachable();
}
u32 CScriptGameObject::GetAmmoType()
{
	CWeapon						*k = smart_cast<CWeapon*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeapon : cannot access class member GetAmmoType!");
		return 0;
	}
	return k->m_ammoType;
}
u32 CScriptGameObject::GetUnderbarrelAmmoType()
{
	CWeaponMagazinedWGrenade						*k = smart_cast<CWeaponMagazinedWGrenade*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponMagazinedWGrenade : cannot access class member GetUnderbarrelAmmoType!");
		return 0;
	}
	return k->m_ammoType2;
}
u32 CScriptGameObject::GetUnderbarrelNumTypes()
{
	CWeaponMagazinedWGrenade						*k = smart_cast<CWeaponMagazinedWGrenade*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponMagazinedWGrenade : cannot access class member GetUnderbarrelNumTypes!");
		return 0;
	}
	return k->m_ammoTypes2.size();
}
bool CScriptGameObject::GetGLMode()
{
	CWeaponMagazinedWGrenade						*k = smart_cast<CWeaponMagazinedWGrenade*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponMagazinedWGrenade : cannot access class member GetUnderbarrelNumTypes!");
		return false;
	}
	return k->m_bGrenadeMode;
}
u32 CScriptGameObject::GetCurrAmmo()
{
	CWeaponAmmo						*k = smart_cast<CWeaponAmmo*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponAmmo : cannot access class member GetCurrAmmo!");
		return 0;
	}
	return k->m_boxCurr;
}
void CScriptGameObject::SetHudOffset(Fvector _offset)
{
	CHudItem						*k = smart_cast<CHudItem*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member SetHudOffset!");
		return;
	}
	k->GetHUD()->SetZoomOffset(_offset);
}

void CScriptGameObject::SetHudRotate(Fvector2 _v)
{
	CHudItem						*k = smart_cast<CHudItem*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member SetHudRotate!");
		return;
	}
	k->GetHUD()->SetZoomRotateX(_v.x);
	k->GetHUD()->SetZoomRotateY(_v.y);
}
/*
void CScriptGameObject::SetHudRotate(float _x, float _y)
{
	CHudItem						*k = smart_cast<CHudItem*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member SetHudRotate!");
		return;
	}
	k->GetHUD()->SetZoomRotateX(_x);
	k->GetHUD()->SetZoomRotateY(_y);
}*/
CScriptGameObject *CScriptGameObject::GetActorHolder()
{
	CActor						*k = smart_cast<CActor*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member GetActorHolder!");
		return nullptr;
	}
	CHolderCustom *obj = k->Holder();
	if (obj)
		return (smart_cast<CGameObject *>(obj))->lua_game_object();
	else
		return nullptr;
}
class CCameraBase;
CCameraBase *CScriptGameObject::GetCarCamera()
{
	CCar						*k = smart_cast<CCar*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CCar : cannot access class member GetCarCamera!");
		return nullptr;
	}
	return k->get_active_camera();
}
Fmatrix *CScriptGameObject::GetXForm()
{
	return &(object().renderable.xform);
}
bool CScriptGameObject::ZoomMode()
{
	CActor						*k = smart_cast<CActor*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member ZoomMode!");
		return false;
	}
	return k->IsZoomAimingMode();
}
void CScriptGameObject::ResetState()
{
	CActor						*k = smart_cast<CActor*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member ZoomMode!");
		return;
	}
	k->ResetStates();
}

void CScriptGameObject::ZeroEffects()
{
	CEatableItem						*item = smart_cast<CEatableItem*>(&object());
	if (!item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CEatableItem : cannot access class member ZeroEffects!");
		return;
	}
	item->ZeroAllEffects();
}
void CScriptGameObject::SetRadiationInfluence(float rad)
{
	CEatableItem						*item = smart_cast<CEatableItem*>(&object());
	if (!item) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CEatableItem : cannot access class member ZeroEffects!");
		return;
	}
	item->SetRadiation(rad);
}

void CScriptGameObject::SetDrugRadProtection(float _prot)
{
	CActor						*k = smart_cast<CActor*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member SetDrugRadProtection!");
		return;
	}
	k->SetDrugRadProtection(_prot);
}

void CScriptGameObject::SetDrugPsyProtection(float _prot)
{
	CActor						*k = smart_cast<CActor*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CActor : cannot access class member SetDrugPsyProtection!");
		return;
	}
	k->SetDrugPsyProtection(_prot);
}