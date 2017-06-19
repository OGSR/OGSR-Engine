#include "stdafx.h"
#include "weaponammo.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "Actor_Flags.h"
#include "inventory.h"
#include "weapon.h"
#include "level_bullet_manager.h"
#include "ai_space.h"
#include "gamemtllib.h"
#include "level.h"
#include "string_table.h"

#define BULLET_MANAGER_SECTION "bullet_manager"

CCartridge::CCartridge() 
{
	m_flags.assign			(cfTracer | cfRicochet);
	m_ammoSect = NULL;
	m_kDist = m_kDisp = m_kHit = m_kImpulse = m_kPierce = 1.f;
	m_kAP = 0.0f;
	m_kAirRes = 0.0f;
	m_buckShot = 1;
	m_impair = 1.f;

	bullet_material_idx = u16(-1);
}

void CCartridge::Load(LPCSTR section, u8 LocalAmmoType) 
{
	m_ammoSect				= section;
	m_LocalAmmoType			= LocalAmmoType;
	m_kDist					= pSettings->r_float(section, "k_dist");
	m_kDisp					= pSettings->r_float(section, "k_disp");
	m_kHit					= pSettings->r_float(section, "k_hit");
	m_kImpulse				= pSettings->r_float(section, "k_impulse");
	m_kPierce				= pSettings->r_float(section, "k_pierce");
	m_kAP					= READ_IF_EXISTS(pSettings, r_float, section, "k_ap", 0.0f);
	m_u8ColorID				= READ_IF_EXISTS(pSettings, r_u8, section, "tracer_color_ID", 0);
	
	if (pSettings->line_exist(section, "k_air_resistance"))
		m_kAirRes				=  pSettings->r_float(section, "k_air_resistance");
	else
		m_kAirRes				= pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");

	m_flags.set				(cfTracer, pSettings->r_bool(section, "tracer"));
	m_buckShot				= pSettings->r_s32(section, "buck_shot");
	m_impair				= pSettings->r_float(section, "impair");
	fWallmarkSize			= pSettings->r_float(section, "wm_size");

	m_flags.set				(cfCanBeUnlimited | cfRicochet, TRUE);
	if(pSettings->line_exist(section,"can_be_unlimited"))
		m_flags.set(cfCanBeUnlimited, pSettings->r_bool(section, "can_be_unlimited"));

	if(pSettings->line_exist(section,"explosive"))
		m_flags.set			(cfExplosive, pSettings->r_bool(section, "explosive"));

	bullet_material_idx		=  GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);
	VERIFY	(u16(-1)!=bullet_material_idx);
	VERIFY	(fWallmarkSize>0);

	m_InvShortName			= CStringTable().translate( pSettings->r_string(section, "inv_name_short"));
}

CWeaponAmmo::CWeaponAmmo(void) 
{
	m_weight				= .2f;
	m_flags.set				(Fbelt, TRUE);
}

CWeaponAmmo::~CWeaponAmmo(void)
{
}

void CWeaponAmmo::Load(LPCSTR section) 
{
	inherited::Load			(section);

	m_kDist					= pSettings->r_float(section, "k_dist");
	m_kDisp					= pSettings->r_float(section, "k_disp");
	m_kHit					= pSettings->r_float(section, "k_hit");
	m_kImpulse				= pSettings->r_float(section, "k_impulse");
	m_kPierce				= pSettings->r_float(section, "k_pierce");
	m_kAP					= READ_IF_EXISTS(pSettings, r_float, section, "k_ap", 0.0f);
	m_u8ColorID				= READ_IF_EXISTS(pSettings, r_u8, section, "tracer_color_ID", 0);

	if (pSettings->line_exist(section, "k_air_resistance"))
		m_kAirRes				=  pSettings->r_float(section, "k_air_resistance");
	else
		m_kAirRes				= pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");
	m_tracer				= !!pSettings->r_bool(section, "tracer");
	m_buckShot				= pSettings->r_s32(section, "buck_shot");
	m_impair				= pSettings->r_float(section, "impair");
	fWallmarkSize			= pSettings->r_float(section,"wm_size");
	R_ASSERT				(fWallmarkSize>0);

	m_boxSize				= (u16)pSettings->r_s32(section, "box_size");
	m_boxCurr				= m_boxSize;	
}

BOOL CWeaponAmmo::net_Spawn(CSE_Abstract* DC) 
{
	BOOL bResult			= inherited::net_Spawn	(DC);
	CSE_Abstract	*e		= (CSE_Abstract*)(DC);
	CSE_ALifeItemAmmo* l_pW	= smart_cast<CSE_ALifeItemAmmo*>(e);
	m_boxCurr				= l_pW->a_elapsed;
	
	if(m_boxCurr > m_boxSize)
		l_pW->a_elapsed		= m_boxCurr = m_boxSize;

	return					bResult;
}

void CWeaponAmmo::net_Destroy() 
{
	inherited::net_Destroy	();
}

void CWeaponAmmo::OnH_B_Chield() 
{
	inherited::OnH_B_Chield	();
}

void CWeaponAmmo::OnH_B_Independent(bool just_before_destroy) 
{
	if(!Useful()) {
		
		if (Local()){
			DestroyObject	();
		}
		m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent(just_before_destroy);
}


bool CWeaponAmmo::Useful() const
{
	// Если IItem еще не полностью использованый, вернуть true
	return !!m_boxCurr;
}
/*
s32 CWeaponAmmo::Sort(PIItem pIItem) 
{
	// Если нужно разместить IItem после this - вернуть 1, если
	// перед - -1. Если пофиг то 0.
	CWeaponAmmo *l_pA = smart_cast<CWeaponAmmo*>(pIItem);
	if(!l_pA) return 0;
	if(xr_strcmp(cNameSect(), l_pA->cNameSect())) return 0;
	if(m_boxCurr <= l_pA->m_boxCurr) return 1;
	else return -1;
}
*/
bool CWeaponAmmo::Get(CCartridge &cartridge) 
{
	if(!m_boxCurr) return false;
	cartridge.m_ammoSect = cNameSect();
	cartridge.m_kDist = m_kDist;
	cartridge.m_kDisp = m_kDisp;
	cartridge.m_kHit = m_kHit;
	cartridge.m_kImpulse = m_kImpulse;
	cartridge.m_kPierce = m_kPierce;
	cartridge.m_kAP = m_kAP;
	cartridge.m_kAirRes = m_kAirRes;
	cartridge.m_u8ColorID = m_u8ColorID;
	cartridge.m_flags.set(CCartridge::cfTracer ,m_tracer);
	cartridge.m_buckShot = m_buckShot;
	cartridge.m_impair = m_impair;
	cartridge.fWallmarkSize = fWallmarkSize;
	cartridge.bullet_material_idx = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);
	cartridge.m_InvShortName = NameShort();
	--m_boxCurr;
	if(m_pCurrentInventory)
		m_pCurrentInventory->InvalidateState();
	return true;
}

void CWeaponAmmo::renderable_Render() 
{
	if(!m_ready_to_destroy)
		inherited::renderable_Render();
}

void CWeaponAmmo::UpdateCL() 
{
	VERIFY2								(_valid(renderable.xform),*cName());
	inherited::UpdateCL	();
	VERIFY2								(_valid(renderable.xform),*cName());
	
	if(!IsGameTypeSingle())
		make_Interpolation	();

	VERIFY2								(_valid(renderable.xform),*cName());

}

void CWeaponAmmo::net_Export(NET_Packet& P) 
{
	inherited::net_Export	(P);
	
	P.w_u16					(m_boxCurr);
}

void CWeaponAmmo::net_Import(NET_Packet& P) 
{
	inherited::net_Import	(P);

	P.r_u16					(m_boxCurr);
}

CInventoryItem *CWeaponAmmo::can_make_killing	(const CInventory *inventory) const
{
	VERIFY					(inventory);

	TIItemContainer::const_iterator	I = inventory->m_all.begin();
	TIItemContainer::const_iterator	E = inventory->m_all.end();
	for ( ; I != E; ++I) {
		CWeapon		*weapon = smart_cast<CWeapon*>(*I);
		if (!weapon)
			continue;
		xr_vector<shared_str>::const_iterator	i = std::find(weapon->m_ammoTypes.begin(),weapon->m_ammoTypes.end(),cNameSect());
		if (i != weapon->m_ammoTypes.end())
			return			(weapon);
	}

	return					(0);
}

float CWeaponAmmo::Weight()
{
	float res = inherited::Weight();

	res *= (float)m_boxCurr/(float)m_boxSize;

	return res;
}