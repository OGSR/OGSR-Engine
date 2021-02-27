////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrMessages.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "clsid_game.h"
#include "xrServer_Objects_ALife_Items.h"
#include "clsid_game.h"

#ifdef XRGAME_EXPORTS
#	ifdef DEBUG
#		define PHPH_DEBUG
#	endif
#endif
#ifdef PHPH_DEBUG
#include "PHDebug.h"
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeInventoryItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeInventoryItem::CSE_ALifeInventoryItem(LPCSTR caSection)
{
	//текущее состояние вещи
	m_fCondition				= 1.0f;

	m_fMass						= pSettings->r_float(caSection, "inv_weight");
	m_dwCost					= pSettings->r_u32(caSection, "cost");

	if (pSettings->line_exist(caSection, "condition"))
		m_fCondition			= pSettings->r_float(caSection, "condition");

	if (pSettings->line_exist(caSection, "health_value"))
		m_iHealthValue			= pSettings->r_s32(caSection, "health_value");
	else
		m_iHealthValue			= 0;

	if (pSettings->line_exist(caSection, "food_value"))
		m_iFoodValue			= pSettings->r_s32(caSection, "food_value");
	else
		m_iFoodValue			= 0;

	m_fDeteriorationValue		= 0;

	m_last_update_time			= 0;

	State.quaternion.x			= 0.f;
	State.quaternion.y			= 0.f;
	State.quaternion.z			= 1.f;
	State.quaternion.w			= 0.f;

	State.angular_vel.set		(0.f,0.f,0.f);
	State.linear_vel.set		(0.f,0.f,0.f);
}

CSE_Abstract *CSE_ALifeInventoryItem::init	()
{
	m_self						= smart_cast<CSE_ALifeObject*>(this);
	R_ASSERT					(m_self);
//	m_self->m_flags.set			(CSE_ALifeObject::flSwitchOffline,TRUE);
	return						(base());
}

CSE_ALifeInventoryItem::~CSE_ALifeInventoryItem	()
{
}

void CSE_ALifeInventoryItem::STATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fCondition);
	State.position				= base()->o_Position;
}

void CSE_ALifeInventoryItem::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	u16 m_wVersion = base()->m_wVersion;
	if (m_wVersion > 52)
		tNetPacket.r_float		(m_fCondition);

	State.position				= base()->o_Position;
}

static inline bool check (const u8 &mask, const u8 &test)
{
	return							(!!(mask & test));
}

void CSE_ALifeInventoryItem::UPDATE_Write	(NET_Packet &tNetPacket)
{
	if (!m_u8NumItems) {
		tNetPacket.w_u8				(0);
		return;
	}

	mask_num_items					num_items;
	num_items.mask					= 0;
	num_items.num_items				= m_u8NumItems;

	R_ASSERT2						(
		num_items.num_items < (u8(1) << 5),
		make_string("%d",num_items.num_items)
	);

	if (State.enabled)									num_items.mask |= inventory_item_state_enabled;
	if (fis_zero(State.angular_vel.square_magnitude()))	num_items.mask |= inventory_item_angular_null;
	if (fis_zero(State.linear_vel.square_magnitude()))	num_items.mask |= inventory_item_linear_null;

	tNetPacket.w_u8					(num_items.common);

	tNetPacket.w_vec3				(State.position);

	tNetPacket.w_float_q8			(State.quaternion.x,0.f,1.f);
	tNetPacket.w_float_q8			(State.quaternion.y,0.f,1.f);
	tNetPacket.w_float_q8			(State.quaternion.z,0.f,1.f);
	tNetPacket.w_float_q8			(State.quaternion.w,0.f,1.f);	

	if (!check(num_items.mask,inventory_item_angular_null)) {
		tNetPacket.w_float_q8		(State.angular_vel.x,0.f,10*PI_MUL_2);
		tNetPacket.w_float_q8		(State.angular_vel.y,0.f,10*PI_MUL_2);
		tNetPacket.w_float_q8		(State.angular_vel.z,0.f,10*PI_MUL_2);
	}

	if (!check(num_items.mask,inventory_item_linear_null)) {
		tNetPacket.w_float_q8		(State.linear_vel.x,-32.f,32.f);
		tNetPacket.w_float_q8		(State.linear_vel.y,-32.f,32.f);
		tNetPacket.w_float_q8		(State.linear_vel.z,-32.f,32.f);
	}
};

void CSE_ALifeInventoryItem::UPDATE_Read	(NET_Packet &tNetPacket)
{
	tNetPacket.r_u8					(m_u8NumItems);
	if (!m_u8NumItems) {
		return;
	}

	mask_num_items					num_items;
	num_items.common				= m_u8NumItems;
	m_u8NumItems					= num_items.num_items;

	R_ASSERT2						(
		m_u8NumItems < (u8(1) << 5),
		make_string("%d",m_u8NumItems)
	);

	tNetPacket.r_vec3				(State.position);

	tNetPacket.r_float_q8			(State.quaternion.x,0.f,1.f);
	tNetPacket.r_float_q8			(State.quaternion.y,0.f,1.f);
	tNetPacket.r_float_q8			(State.quaternion.z,0.f,1.f);
	tNetPacket.r_float_q8			(State.quaternion.w,0.f,1.f);	

	State.enabled					= check(num_items.mask,inventory_item_state_enabled);

	if (!check(num_items.mask,inventory_item_angular_null)) {
		tNetPacket.r_float_q8		(State.angular_vel.x,0.f,10*PI_MUL_2);
		tNetPacket.r_float_q8		(State.angular_vel.y,0.f,10*PI_MUL_2);
		tNetPacket.r_float_q8		(State.angular_vel.z,0.f,10*PI_MUL_2);
	}
	else
		State.angular_vel.set		(0.f,0.f,0.f);

	if (!check(num_items.mask,inventory_item_linear_null)) {
		tNetPacket.r_float_q8		(State.linear_vel.x,-32.f,32.f);
		tNetPacket.r_float_q8		(State.linear_vel.y,-32.f,32.f);
		tNetPacket.r_float_q8		(State.linear_vel.z,-32.f,32.f);
	}
	else
		State.linear_vel.set		(0.f,0.f,0.f);
};

bool CSE_ALifeInventoryItem::bfUseful		()
{
	return						(true);
}

u32 CSE_ALifeInventoryItem::update_rate		() const
{
	return						(1000);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItem::CSE_ALifeItem				(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeInventoryItem(caSection)
{
	m_physics_disabled			= false;
}

CSE_ALifeItem::~CSE_ALifeItem				()
{
}

CSE_Abstract *CSE_ALifeItem::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeItem::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeItem::base		() const
{
	return						(inherited1::base());
}

void CSE_ALifeItem::STATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeItem::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if ((m_tClassID == CLSID_OBJECT_W_BINOCULAR) && (m_wVersion < 37)) {
		tNetPacket.r_u16		();
		tNetPacket.r_u16		();
		tNetPacket.r_u8			();
	}
	inherited2::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeItem::UPDATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);

#ifdef XRGAME_EXPORTS
	m_last_update_time			= Device.dwTimeGlobal;
#endif // XRGAME_EXPORTS
};

void CSE_ALifeItem::UPDATE_Read				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);

	m_physics_disabled			= false;
};

BOOL CSE_ALifeItem::Net_Relevant			()
{
	if (attached())
		return					(false);

	if (!m_physics_disabled && !fis_zero(State.linear_vel.square_magnitude(),EPS_L))
		return					(true);

#ifdef XRGAME_EXPORTS
	if (Device.dwTimeGlobal < (m_last_update_time + update_rate()))
		return					(false);
#endif // XRGAME_EXPORTS

	return						(true);
}

void CSE_ALifeItem::OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	inherited1::OnEvent			(tNetPacket,type,time,sender);

	if (type != GE_FREEZE_OBJECT)
		return;

//	R_ASSERT					(!m_physics_disabled);
	m_physics_disabled			= true;
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemTorch
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemTorch::CSE_ALifeItemTorch		(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	m_active					= false;
	m_nightvision_active		= false;
	m_attached					= false;
}

CSE_ALifeItemTorch::~CSE_ALifeItemTorch		()
{
}

BOOL	CSE_ALifeItemTorch::Net_Relevant			()
{
	if (m_attached) return true;
	return inherited::Net_Relevant();
}


void CSE_ALifeItemTorch::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);

}

void CSE_ALifeItemTorch::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemTorch::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	
	BYTE F = tNetPacket.r_u8();
	m_active					= !!(F & eTorchActive);
	m_nightvision_active		= !!(F & eNightVisionActive);
	m_attached					= !!(F & eAttached);
}

void CSE_ALifeItemTorch::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);

	BYTE F = 0;
	F |= (m_active ? eTorchActive : 0);
	F |= (m_nightvision_active ? eNightVisionActive : 0);
	F |= (m_attached ? eAttached : 0);
	tNetPacket.w_u8(F);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeapon
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeapon::CSE_ALifeItemWeapon	(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	a_current					= 90;
	a_elapsed					= 0;
	wpn_flags					= 0;
	wpn_state					= 0;
	ammo_type					= 0;

	m_fHitPower					= pSettings->r_float(caSection,"hit_power");
	m_tHitType					= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	m_caAmmoSections			= pSettings->r_string(caSection,"ammo_class");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));

	m_addon_flags.zero			();

	m_scope_status				=	(EWeaponAddonStatus)pSettings->r_s32(s_name,"scope_status");
	m_silencer_status			=	(EWeaponAddonStatus)pSettings->r_s32(s_name,"silencer_status");
	m_grenade_launcher_status	=	(EWeaponAddonStatus)pSettings->r_s32(s_name,"grenade_launcher_status");
	m_ef_main_weapon_type		= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_main_weapon_type",u32(-1));
	m_ef_weapon_type			= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_weapon_type",u32(-1));
}

CSE_ALifeItemWeapon::~CSE_ALifeItemWeapon	()
{
}

u32	CSE_ALifeItemWeapon::ef_main_weapon_type() const
{
	VERIFY	(m_ef_main_weapon_type != u32(-1));
	return	(m_ef_main_weapon_type);
}

u32	CSE_ALifeItemWeapon::ef_weapon_type() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CSE_ALifeItemWeapon::UPDATE_Read(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);

	tNetPacket.r_float_q8		(m_fCondition,0.0f,1.0f);
	tNetPacket.r_u8				(wpn_flags);
	tNetPacket.r_u16			(a_elapsed);
	tNetPacket.r_u8				(m_addon_flags.flags);
	tNetPacket.r_u8				(ammo_type);
	tNetPacket.r_u8				(wpn_state);
	tNetPacket.r_u8				(m_bZoom);
}

void CSE_ALifeItemWeapon::UPDATE_Write(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);

	tNetPacket.w_float_q8		(m_fCondition,0.0f,1.0f);
	tNetPacket.w_u8				(wpn_flags);
	tNetPacket.w_u16			(a_elapsed);
	tNetPacket.w_u8				(m_addon_flags.get());
	tNetPacket.w_u8				(ammo_type);
	tNetPacket.w_u8				(wpn_state);
	tNetPacket.w_u8				(m_bZoom);
}

void CSE_ALifeItemWeapon::STATE_Read(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u16			(a_current);
	tNetPacket.r_u16			(a_elapsed);
	tNetPacket.r_u8				(wpn_state);
	
	if (m_wVersion > 40)
		tNetPacket.r_u8			(m_addon_flags.flags);

	if (m_wVersion > 46)
		tNetPacket.r_u8			(ammo_type);
}

void CSE_ALifeItemWeapon::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_current);
	tNetPacket.w_u16			(a_elapsed);
	tNetPacket.w_u8				(wpn_state);
	tNetPacket.w_u8				(m_addon_flags.get());
	tNetPacket.w_u8				(ammo_type);
}

void CSE_ALifeItemWeapon::OnEvent			(NET_Packet	&tNetPacket, u16 type, u32 time, ClientID sender )
{
	inherited::OnEvent			(tNetPacket,type,time,sender);
	switch (type) {
		case GE_WPN_STATE_CHANGE:
			{			
				tNetPacket.r_u8	(wpn_state);			
//				u8 sub_state = 
					tNetPacket.r_u8();		
//				u8 NewAmmoType = 
					tNetPacket.r_u8();
//				u8 AmmoElapsed = 
					tNetPacket.r_u8();	
			}break;
	}
}

u8	 CSE_ALifeItemWeapon::get_slot			()
{
	return						((u8)pSettings->r_u8(s_name,"slot"));
}

u16	 CSE_ALifeItemWeapon::get_ammo_limit	()
{
	return						(u16) pSettings->r_u16(s_name,"ammo_limit");
}

u16	 CSE_ALifeItemWeapon::get_ammo_total	()
{
	return						((u16)a_current);
}

u16	 CSE_ALifeItemWeapon::get_ammo_elapsed	()
{
	return						((u16)a_elapsed);
}

u16 CSE_ALifeItemWeapon::get_ammo_magsize()
{
	if (Core.Features.test(xrCore::Feature::hard_ammo_reload))
	{
		const char* S = pSettings->r_string(s_name, "ammo_class");
		string128 _ammoItem{};
		_GetItem(S, 0, _ammoItem);
		return std::min(pSettings->r_u16(_ammoItem, "box_size"), pSettings->r_u16(s_name, "ammo_mag_size"));
	}
	else {
		return pSettings->r_u16(s_name, "ammo_mag_size");
	}
}

BOOL CSE_ALifeItemWeapon::Net_Relevant()
{
	return (wpn_flags==1);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponShotGun
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponShotGun::CSE_ALifeItemWeaponShotGun	(LPCSTR caSection) : CSE_ALifeItemWeaponMagazined(caSection)
{
	m_AmmoIDs.clear();
}

CSE_ALifeItemWeaponShotGun::~CSE_ALifeItemWeaponShotGun	()
{
}

void CSE_ALifeItemWeaponShotGun::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);

	m_AmmoIDs.clear();
	u8 AmmoCount = P.r_u8();
	for (u8 i=0; i<AmmoCount; i++)
	{
		m_AmmoIDs.push_back(P.r_u8());
	}
}
void CSE_ALifeItemWeaponShotGun::UPDATE_Write	(NET_Packet& P)
{
	inherited::UPDATE_Write(P);

	P.w_u8(u8(m_AmmoIDs.size()));
	for (u32 i=0; i<m_AmmoIDs.size(); i++)
	{
		P.w_u8(u8(m_AmmoIDs[i]));
	}
}
void CSE_ALifeItemWeaponShotGun::STATE_Read		(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);
}
void CSE_ALifeItemWeaponShotGun::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponMagazined
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponMagazined::CSE_ALifeItemWeaponMagazined(const char* caSection) : CSE_ALifeItemWeapon(caSection)
{
	auto FireModesList = READ_IF_EXISTS(pSettings, r_string, caSection, "fire_modes", nullptr);
	if (FireModesList) {
		int ModesCount = _GetItemCount(FireModesList);
		m_u8CurFireMode = u8(ModesCount - 1);
	}
	else {
		m_u8CurFireMode = 0;
	}
}

CSE_ALifeItemWeaponMagazined::~CSE_ALifeItemWeaponMagazined	()
{
}

void CSE_ALifeItemWeaponMagazined::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);

	m_u8CurFireMode = P.r_u8();
}
void CSE_ALifeItemWeaponMagazined::UPDATE_Write	(NET_Packet& P)
{
	inherited::UPDATE_Write(P);

	P.w_u8(m_u8CurFireMode);	
}
void CSE_ALifeItemWeaponMagazined::STATE_Read		(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);

	if (!P.r_eof()) {
		m_u8CurFireMode = P.r_u8();
	}
}
void CSE_ALifeItemWeaponMagazined::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);

	P.w_u8(m_u8CurFireMode);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeaponMagazinedWGL
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeaponMagazinedWGL::CSE_ALifeItemWeaponMagazinedWGL	(LPCSTR caSection) : CSE_ALifeItemWeaponMagazined(caSection)
{
	m_bGrenadeMode = false;
	ammo_type2 = 0;
	a_elapsed2 = 0;
}

CSE_ALifeItemWeaponMagazinedWGL::~CSE_ALifeItemWeaponMagazinedWGL	()
{
}

void CSE_ALifeItemWeaponMagazinedWGL::UPDATE_Read(NET_Packet& P)
{
	u8 _data = P.r_u8();
	m_bGrenadeMode = !!(_data & 0x1);

	inherited::UPDATE_Read(P);

	if ( !P.r_eof() ) {
	  ammo_type2 = P.r_u8();
	  a_elapsed2 = P.r_u16();
	  //Msg( "~~[%s][%s] update_read: m_bGrenadeMode: [%u], iAmmoElapsed2: [%u], m_ammoType2: [%u]", __FUNCTION__, this->name(), m_bGrenadeMode, a_elapsed2, ammo_type2 );
	}
}

void CSE_ALifeItemWeaponMagazinedWGL::UPDATE_Write(NET_Packet& P)
{
	P.w_u8( m_bGrenadeMode ? 1 : 0 );

	inherited::UPDATE_Write(P);

	P.w_u8( ammo_type2 );
	P.w_u16( a_elapsed2 );
	//Msg( "~~[%s][%s] update_write: m_bGrenadeMode: [%u], iAmmoElapsed2: [%u], m_ammoType2: [%u]", __FUNCTION__, this->name(), m_bGrenadeMode, a_elapsed2, ammo_type2 );
}

void CSE_ALifeItemWeaponMagazinedWGL::STATE_Read		(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);

	if (!P.r_eof()) {
		ammo_type2 = P.r_u8();
		a_elapsed2 = P.r_u16();
		//Msg( "~~[%s][%s] update_read: m_bGrenadeMode: [%u], iAmmoElapsed2: [%u], m_ammoType2: [%u]", __FUNCTION__, this->name(), m_bGrenadeMode, a_elapsed2, ammo_type2 );
	}
	if ( !P.r_eof() ) {
          u8 _data = P.r_u8();
          m_bGrenadeMode = !!( _data & 0x1 );
        }
}

void CSE_ALifeItemWeaponMagazinedWGL::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);

	P.w_u8(ammo_type2);
	P.w_u16(a_elapsed2);
	P.w_u8( m_bGrenadeMode ? 1 : 0 );
	//Msg( "~~[%s][%s] update_write: m_bGrenadeMode: [%u], iAmmoElapsed2: [%u], m_ammoType2: [%u]", __FUNCTION__, this->name(), m_bGrenadeMode, a_elapsed2, ammo_type2 );
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemAmmo
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemAmmo::CSE_ALifeItemAmmo		(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	a_elapsed					= m_boxSize = (u16)pSettings->r_s32(caSection, "box_size");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));
}

CSE_ALifeItemAmmo::~CSE_ALifeItemAmmo		()
{
}

void CSE_ALifeItemAmmo::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);

	tNetPacket.r_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);

	tNetPacket.w_u16			(a_elapsed);
}

bool CSE_ALifeItemAmmo::can_switch_online	() const
{
	return inherited::can_switch_online();
}

bool CSE_ALifeItemAmmo::can_switch_offline	() const
{
	return ( inherited::can_switch_offline() && a_elapsed!=0 );
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDetector::CSE_ALifeItemDetector(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	m_ef_detector_type	= pSettings->r_u32(caSection,"ef_detector_type");
}

CSE_ALifeItemDetector::~CSE_ALifeItemDetector()
{
}

u32	 CSE_ALifeItemDetector::ef_detector_type	() const
{
	return	(m_ef_detector_type);
}

void CSE_ALifeItemDetector::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
}

void CSE_ALifeItemDetector::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemArtefact::CSE_ALifeItemArtefact(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	m_fAnomalyValue				= 100.f;
}

CSE_ALifeItemArtefact::~CSE_ALifeItemArtefact()
{
}

void CSE_ALifeItemArtefact::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemArtefact::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

BOOL CSE_ALifeItemArtefact::Net_Relevant	()
{
	return							(inherited::Net_Relevant());
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemPDA
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemPDA::CSE_ALifeItemPDA		(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	m_original_owner		= 0xffff;
	m_specific_character	= NULL;
	m_info_portion			= NULL;
}


CSE_ALifeItemPDA::~CSE_ALifeItemPDA		()
{
}

void CSE_ALifeItemPDA::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	if (m_wVersion > 58)
		tNetPacket.r			(&m_original_owner,sizeof(m_original_owner));

	if (m_wVersion > 89)

	if ( (m_wVersion > 89)&&(m_wVersion < 98)  )
	{
		int tmp,tmp2;
		tNetPacket.r			(&tmp,		sizeof(int));
		tNetPacket.r			(&tmp2,		sizeof(int));
		m_info_portion			=	NULL;
		m_specific_character	= NULL;
	}else{
		tNetPacket.r_stringZ	(m_specific_character);
		tNetPacket.r_stringZ	(m_info_portion);
	
	}
}

void CSE_ALifeItemPDA::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w				(&m_original_owner,sizeof(m_original_owner));
#ifdef XRGAME_EXPORTS
	tNetPacket.w_stringZ		(m_specific_character);
	tNetPacket.w_stringZ		(m_info_portion);
#else
	shared_str		tmp_1	= NULL;
	shared_str						tmp_2	= NULL;

	tNetPacket.w_stringZ		(tmp_1);
	tNetPacket.w_stringZ		(tmp_2);
#endif

}

void CSE_ALifeItemPDA::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemPDA::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDocument
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDocument::CSE_ALifeItemDocument(LPCSTR caSection): CSE_ALifeItem(caSection)
{
	m_wDoc					= NULL;
}

CSE_ALifeItemDocument::~CSE_ALifeItemDocument()
{
}

void CSE_ALifeItemDocument::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);

	if ( m_wVersion < 98  ){
		u16 tmp;
		tNetPacket.r_u16			(tmp);
		m_wDoc = NULL;
	}else
		tNetPacket.r_stringZ		(m_wDoc);
}

void CSE_ALifeItemDocument::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_stringZ		(m_wDoc);
}

void CSE_ALifeItemDocument::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemDocument::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemGrenade
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemGrenade::CSE_ALifeItemGrenade	(LPCSTR caSection): CSE_ALifeItem(caSection)
{
	m_ef_weapon_type	= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_weapon_type",u32(-1));
}

CSE_ALifeItemGrenade::~CSE_ALifeItemGrenade	()
{
}

u32	CSE_ALifeItemGrenade::ef_weapon_type() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CSE_ALifeItemGrenade::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemGrenade::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemExplosive
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemExplosive::CSE_ALifeItemExplosive	(LPCSTR caSection): CSE_ALifeItem(caSection)
{
}

CSE_ALifeItemExplosive::~CSE_ALifeItemExplosive	()
{
}

void CSE_ALifeItemExplosive::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemExplosive::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemBolt
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemBolt::CSE_ALifeItemBolt		(LPCSTR caSection) : CSE_ALifeItem(caSection)
{
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
	m_ef_weapon_type			= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_weapon_type",u32(-1));
}

CSE_ALifeItemBolt::~CSE_ALifeItemBolt		()
{
}

u32	CSE_ALifeItemBolt::ef_weapon_type() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CSE_ALifeItemBolt::STATE_Write			(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemBolt::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeItemBolt::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeItemBolt::UPDATE_Read			(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

bool CSE_ALifeItemBolt::can_save			() const
{
	return						(false);//!attached());
}
bool CSE_ALifeItemBolt::used_ai_locations		() const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemCustomOutfit
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemCustomOutfit::CSE_ALifeItemCustomOutfit	(LPCSTR caSection): CSE_ALifeItem(caSection)
{
	m_ef_equipment_type		= pSettings->r_u32(caSection,"ef_equipment_type");
}

CSE_ALifeItemCustomOutfit::~CSE_ALifeItemCustomOutfit	()
{
}

u32	CSE_ALifeItemCustomOutfit::ef_equipment_type		() const
{
	return			(m_ef_equipment_type);
}

void CSE_ALifeItemCustomOutfit::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemCustomOutfit::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemCustomOutfit::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read			(tNetPacket);
	tNetPacket.r_float_q8			(m_fCondition,0.0f,1.0f);
}

void CSE_ALifeItemCustomOutfit::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write			(tNetPacket);
	tNetPacket.w_float_q8			(m_fCondition,0.0f,1.0f);
}

BOOL CSE_ALifeItemCustomOutfit::Net_Relevant		()
{
	return							(true);
}
