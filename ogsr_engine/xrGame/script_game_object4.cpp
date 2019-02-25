////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"

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
#include "WeaponKnife.h"

#include "HangingLamp.h"
#include "CharacterPhysicsSupport.h"
#include "ai/monsters/controller/controller.h"
#include "ai/monsters/controller/controller_psy_hit.h"

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
	IInventoryBox* ib = smart_cast<IInventoryBox*>(&object());
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
#include "WeaponShotgun.h"
#include "Actor.h"
#include "../xr_3da/CameraBase.h"

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
TEST_OBJECT_CLASS(CScriptGameObject::IsWeaponShotgun, CWeaponShotgun)
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
TEST_OBJECT_CLASS(CScriptGameObject::IsInventoryBox, IInventoryBox)
TEST_OBJECT_CLASS(CScriptGameObject::IsWeaponBinoculars, CWeaponBinoculars)
TEST_OBJECT_CLASS(CScriptGameObject::IsKnife, CWeaponKnife)
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
        // в CEntityAlive::shedule_Update() вызовы идут именно в этом порядке
	e->conditions().UpdateConditionTime();
	e->conditions().UpdateCondition();
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

void CScriptGameObject::AddWound(float hit_power, int hit_type, u16 element)
{
	CEntityAlive						*e = smart_cast<CEntityAlive*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CEntityAlive : cannot access class member AddWound!");
		return;
	}
	e->conditions().AddWound(hit_power, ALife::EHitType(hit_type), element);
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
	IInventoryBox						*e = smart_cast<IInventoryBox*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "IInventoryBox : cannot access class member InvBoxCount!");
		return 0;
	}
	return e->GetSize();
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
	IInventoryBox						*trunk = smart_cast<IInventoryBox*>(&(obj->object()));
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
CScriptGameObject *CScriptGameObject::ObjectFromInvBox(int _i)
{
	IInventoryBox						*e = smart_cast<IInventoryBox*>(&object());
	if (!e) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "IInventoryBox : cannot access class member ObjectFromInvBox!");
		return nullptr;
	}
	return e->GetObjectByIndex( _i );
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

BOOL CScriptGameObject::GetBoneVisible(LPCSTR _bone_name)
{
    CKinematics						*k = smart_cast<CKinematics*>(object().Visual());
    if (!k) {
        ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CKinematics : cannot access class member GetBoneVisible!");
        return FALSE;
    }
    return k->LL_GetBoneVisible(k->LL_BoneID(_bone_name));
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

BOOL CScriptGameObject::GetHudBoneVisible(LPCSTR _bone_name)
{
    CHudItem						*k = smart_cast<CHudItem*>(&object());
    if (!k) {
        ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member GetHudBoneVisible!");
        return FALSE;
    }
    CKinematics *kin = smart_cast<CKinematics *>(k->GetHUD()->Visual());
    return kin->LL_GetBoneVisible(kin->LL_BoneID(_bone_name));
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
bool CScriptGameObject::GetGLMode()
{
	CWeaponMagazinedWGrenade						*k = smart_cast<CWeaponMagazinedWGrenade*>(&object());
	if (!k) {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponMagazinedWGrenade : cannot access class member GetGLMode!");
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

u32 CScriptGameObject::GetAmmoElapsed2()
{
	const CWeaponMagazinedWGrenade *weapon = smart_cast<CWeaponMagazinedWGrenade*>(&object());
	if (!weapon)
		return (0);
	return (weapon->GetAmmoElapsed2());
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

u32 CScriptGameObject::GetHudItemState() {
	CHudItem *k = smart_cast<CHudItem*>( &object() );
	ASSERT_FMT( k, "CHudItem : cannot access class member GetState!" );
	return k->GetState();
}

float CScriptGameObject::GetRadius() {
	CGameObject *k = smart_cast<CGameObject*>( &object() );
	ASSERT_FMT( k, "CGameObject : cannot access class member Radius!" );
	return k->Radius();
}

void CScriptGameObject::play_hud_animation( LPCSTR anim, bool mix_in ) {
	CHudItem *k = smart_cast<CHudItem*>( &object() );
	if ( !k ) {
		ai().script_engine().script_log( ScriptStorage::eLuaMessageTypeError, "CHudItem : cannot access class member play_hud_animation!" );
		return;
	}
	CKinematicsAnimated* sa = smart_cast<CKinematicsAnimated*>( k->GetHUD()->Visual() );
	if( sa ) {
		MotionID m = sa->ID_Cycle( anim );
		if ( m )
			sa->PlayCycle( m, (BOOL) mix_in );
		else {
			ai().script_engine().script_log( ScriptStorage::eLuaMessageTypeError, "CHudItem : has not cycle %s", anim );
		}
	}
	else {
		ai().script_engine().script_log( ScriptStorage::eLuaMessageTypeError, "CHudItem : is not animated object" );
	}
}

void CScriptGameObject::play_hud_animation( LPCSTR anim ) {
	play_hud_animation( anim, true );
}


void CScriptGameObject::addFeelTouch( float radius, const luabind::object& lua_object, const luabind::functor<void>& new_delete ) {
  const luabind::functor<bool> contact;
  addFeelTouch( radius, lua_object, new_delete, contact );
}

void CScriptGameObject::addFeelTouch( float radius, const luabind::object& lua_object, const luabind::functor<void>& new_delete, const luabind::functor<bool>& contact ) {
  CGameObject* GO = smart_cast<CGameObject*>( &object() );
  GO->addFeelTouch( radius, lua_object, new_delete, contact );
}

void CScriptGameObject::removeFeelTouch( const luabind::object& lua_object, const luabind::functor<void>& new_delete ) {
  const luabind::functor<bool> contact;
  removeFeelTouch( lua_object, new_delete, contact );
}

void CScriptGameObject::removeFeelTouch( const luabind::object& lua_object, const luabind::functor<void>& new_delete, const luabind::functor<bool>& contact ) {
  CGameObject* GO = smart_cast<CGameObject*>( &object() );
  GO->removeFeelTouch( lua_object, new_delete, contact );
}


void CScriptGameObject::PHCaptureObject( CScriptGameObject* obj, LPCSTR capture_bone ) {
  auto ps = smart_cast<CPhysicsShellHolder*>( &(obj->object()) );
  ASSERT_FMT( ps, "[%s]: %s not a CPhysicsShellHolder", __FUNCTION__, obj->cName().c_str() );
  auto EA = smart_cast<CEntityAlive*>( &object() );
  ASSERT_FMT( EA, "[%s]: %s not a CEntityAlive", __FUNCTION__, cName().c_str() );
  EA->character_physics_support()->movement()->PHCaptureObject( ps, capture_bone, true );
}

void CScriptGameObject::PHCaptureObject( CScriptGameObject* obj ) {
  PHCaptureObject( obj, nullptr );
}


void CScriptGameObject::PHCaptureObject( CScriptGameObject* obj, u16 bone, LPCSTR capture_bone ) {
  auto ps = smart_cast<CPhysicsShellHolder*>( &(obj->object()) );
  ASSERT_FMT( ps, "[%s]: %s not a CPhysicsShellHolder", __FUNCTION__, obj->cName().c_str() );
  auto EA = smart_cast<CEntityAlive*>( &object() );
  ASSERT_FMT( EA, "[%s]: %s not a CEntityAlive", __FUNCTION__, cName().c_str() );
  EA->character_physics_support()->movement()->PHCaptureObject( ps, bone, capture_bone, true );
}

void CScriptGameObject::PHCaptureObject( CScriptGameObject* obj, u16 bone ) {
  PHCaptureObject( obj, bone, nullptr );
}


void CScriptGameObject::PHReleaseObject() {
  auto EA = smart_cast<CEntityAlive*>( &object() );
  ASSERT_FMT( EA, "[%s]: %s not a CEntityAlive", __FUNCTION__, cName().c_str() );
  EA->character_physics_support()->movement()->PHReleaseObject();
}


CPHCapture* CScriptGameObject::PHCapture() {
  auto EA = smart_cast<CEntityAlive*>( &object() );
  ASSERT_FMT( EA, "[%s]: %s not a CEntityAlive", __FUNCTION__, cName().c_str() );
  return EA->character_physics_support()->movement()->PHCapture();
}


bool CScriptGameObject::throw_target( const Fvector& position, CScriptGameObject* throw_ignore_object ) {
  CAI_Stalker *stalker = smart_cast<CAI_Stalker*>( &object() );
  ASSERT_FMT( stalker, "[%s]: %s not a CAI_Stalker", __FUNCTION__, object().cName().c_str() );
  CObject* obj;
  if ( throw_ignore_object ) {
    obj = smart_cast<CObject*>( &(throw_ignore_object->object()) );
    ASSERT_FMT( obj, "[%s]: %s not a CObject", __FUNCTION__, throw_ignore_object->cName().c_str() );
  }
  else
    obj = nullptr;
  stalker->throw_target( position, obj );
  return stalker->throw_enabled();
}


bool CScriptGameObject::throw_target( const Fvector& position, u32 const vertex_id, CScriptGameObject* throw_ignore_object ) {
  CAI_Stalker *stalker = smart_cast<CAI_Stalker*>( &object() );
  ASSERT_FMT( stalker, "[%s]: %s not a CAI_Stalker", __FUNCTION__, object().cName().c_str() );
  CObject* obj;
  if ( throw_ignore_object ) {
    obj = smart_cast<CObject*>( &(throw_ignore_object->object()) );
    ASSERT_FMT( obj, "[%s]: %s not a CObject", __FUNCTION__, throw_ignore_object->cName().c_str() );
  }
  else
    obj = nullptr;
  stalker->throw_target( position, vertex_id, obj );
  return stalker->throw_enabled();
}


void CScriptGameObject::g_fireParams( const CScriptGameObject* pHudItem, Fvector& P, Fvector& D ) {
  auto E = smart_cast<CEntity*>( &object() );
  ASSERT_FMT( E, "[%s]: %s not a CEntity", __FUNCTION__, object().cName().c_str() );
  const auto item = smart_cast<const CHudItem*>( &(pHudItem->object()) );
  ASSERT_FMT( item, "[%s]: %s not a CHudItem", __FUNCTION__, pHudItem->object().cName().c_str() );
  E->g_fireParams( item, P, D );
}


float CScriptGameObject::stalker_disp_base() {
  CAI_Stalker *stalker = smart_cast<CAI_Stalker*>( &object() );
  ASSERT_FMT( stalker, "[%s]: %s not a CAI_Stalker", __FUNCTION__, object().cName().c_str() );
  return stalker->m_fDispBase;
}

void CScriptGameObject::stalker_disp_base( float disp ) {
  CAI_Stalker *stalker = smart_cast<CAI_Stalker*>( &object() );
  ASSERT_FMT( stalker, "[%s]: %s not a CAI_Stalker", __FUNCTION__, object().cName().c_str() );
  stalker->m_fDispBase = disp;
}

void CScriptGameObject::stalker_disp_base( float range, float maxr ) {
  CAI_Stalker *stalker = smart_cast<CAI_Stalker*>( &object() );
  ASSERT_FMT( stalker, "[%s]: %s not a CAI_Stalker", __FUNCTION__, object().cName().c_str() );
  stalker->m_fDispBase = asin( maxr / range );
}


void CScriptGameObject::DropItemAndThrow( CScriptGameObject* pItem, Fvector speed ) {
  auto owner = smart_cast<CInventoryOwner*>( &object() );
  ASSERT_FMT( owner, "[%s]: %s not a CInventoryOwner", __FUNCTION__, object().cName().c_str() );

  auto item  = smart_cast<CPhysicsShellHolder*>( &pItem->object() );
  ASSERT_FMT( item, "[%s]: %s not a CPhysicsShellHolder", __FUNCTION__, pItem->object().cName().c_str() );

  item->SetActivationSpeedOverride( speed );
  NET_Packet P;
  CGameObject::u_EventGen( P, GE_OWNERSHIP_REJECT, object().ID() );
  P.w_u16( pItem->object().ID() );
  CGameObject::u_EventSend( P );
}


bool CScriptGameObject::controller_psy_hit_active() {
  auto controller = smart_cast<CController*>( &object() );
  ASSERT_FMT( controller, "[%s]: %s not a CController", __FUNCTION__, object().cName().c_str() );
  return controller->m_psy_hit->is_active();
}
