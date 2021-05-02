// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Weapon.h"
#include "ParticlesObject.h"
#include "HUDManager.h"
#include "entity_alive.h"
#include "player_hud.h"
#include "inventory_item_impl.h"

#include "inventory.h"
#include "xrserver_objects_alife_items.h"

#include "actor.h"
#include "actoreffector.h"
#include "level.h"

#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "../Include/xrRender/Kinematics.h"
#include "ai_object_location.h"
#include "clsid_game.h"
#include "mathutils.h"
#include "object_broker.h"
#include "..\xr_3da\IGame_Persistent.h"

#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"

#include "WeaponMagazinedWGrenade.h"
#include "GamePersistent.h"

#define ROTATION_TIME			0.25f

extern ENGINE_API Fvector4 w_states;
extern ENGINE_API Fvector3 w_timers;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeapon::CWeapon(LPCSTR name) : m_fLR_MovingFactor(0.f), m_strafe_offset{}
{
	SetState				(eHidden);
	SetNextState			(eHidden);
	m_sub_state				= eSubstateReloadBegin;
	m_idle_state				= eIdle;
	m_bTriStateReload		= false;
	SetDefaults				();

	m_Offset.identity		();
	m_StrapOffset.identity	();

	iAmmoCurrent			= -1;
	m_dwAmmoCurrentCalcFrame= 0;

	iAmmoElapsed			= -1;
	iMagazineSize			= -1;
	m_ammoType				= 0;

	eHandDependence			= hdNone;

	m_fZoomFactor = Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system) ? 1.f : g_fov;

	m_fZoomRotationFactor	= 0.f;


	m_pAmmo					= nullptr;


	m_pFlameParticles2		= nullptr;
	m_sFlameParticles2		= nullptr;


	m_fCurrentCartirdgeDisp = 1.f;

	m_strap_bone0			= nullptr;
	m_strap_bone1			= nullptr;
	m_StrapOffset.identity	();
	m_strapped_mode			= false;
	m_can_be_strapped		= false;
	m_ef_main_weapon_type	= u32(-1);
	m_ef_weapon_type		= u32(-1);
	m_UIScope				= nullptr;
	m_set_next_ammoType_on_reload = u32(-1);

	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

CWeapon::~CWeapon		()
{
	xr_delete	(m_UIScope);
}

//void CWeapon::Hit(float P, Fvector &dir,	
//		    CObject* who, s16 element,
//		    Fvector position_in_object_space, 
//		    float impulse, 
//		    ALife::EHitType hit_type)
void CWeapon::Hit					(SHit* pHDS)
{
//	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
	inherited::Hit(pHDS);
}



void CWeapon::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*	E		= smart_cast<CEntityAlive*>(H_Parent());
		
		if(!E) 
			return;

		const CInventoryOwner	*parent = smart_cast<const CInventoryOwner*>(E);
		if (!parent || parent && parent->use_simplified_visual())
			return;

		if (parent->attached(this))
			return;

		R_ASSERT		(E);
		IKinematics*	V		= smart_cast<IKinematics*>	(E->Visual());
		VERIFY			(V);

		// Get matrices
		int				boneL,boneR,boneR2;
		E->g_WeaponBones(boneL,boneR,boneR2);
		if ((HandDependence() == hd1Hand) || (GetState() == eReload) || (!E->g_Alive()))
			boneL = boneR2;
#pragma todo("TO ALL: serious performance problem")
		// от mortan:
		// https://www.gameru.net/forum/index.php?s=&showtopic=23443&view=findpost&p=1677678
		V->CalculateBones_Invalidate();
		V->CalculateBones( true ); //V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));
		// Calculate
		Fmatrix				mRes;
		Fvector				R,D,N;
		D.sub				(mL.c,mR.c);	

		if(fis_zero(D.magnitude()))
		{
			mRes.set(E->XFORM());
			mRes.c.set(mR.c);
		}
		else
		{		
			D.normalize();
			R.crossproduct	(mR.j,D);

			N.crossproduct	(D,R);			
			N.normalize();

			mRes.set		(R,N,D,mR.c);
			mRes.mulA_43	(E->XFORM());
		}

		UpdatePosition	(mRes);
	}
}

void CWeapon::UpdateFireDependencies_internal()
{
	if (Device.dwFrame != dwFP_Frame)
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm();

		if (GetHUDmode())
		{
			HudItemData()->setup_firedeps(m_current_firedeps);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		}
		else
		{
			// 3rd person or no parent
			Fmatrix& parent = XFORM();
			Fvector& fp = vLoadedFirePoint;
			Fvector& fp2 = vLoadedFirePoint2;
			Fvector& sp = vLoadedShellPoint;

			parent.transform_tiny(m_current_firedeps.vLastFP, fp);
			parent.transform_tiny(m_current_firedeps.vLastFP2, fp2);
			parent.transform_tiny(m_current_firedeps.vLastSP, sp);

			m_current_firedeps.vLastFD.set(0.f, 0.f, 1.f);
			parent.transform_dir(m_current_firedeps.vLastFD);

			m_current_firedeps.m_FireParticlesXForm.set(parent);
			VERIFY(_valid(m_current_firedeps.m_FireParticlesXForm));
		}
	}
}

void CWeapon::ForceUpdateFireParticles()
{
	if (!GetHUDmode())
	{ // update particlesXFORM real bullet direction

		if (!H_Parent())
			return;

		Fvector p, d;
		smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p, d);

		Fmatrix _pxf;
		_pxf.k = d;
		_pxf.i.crossproduct(Fvector().set(0.0f, 1.0f, 0.0f), _pxf.k);
		_pxf.j.crossproduct(_pxf.k, _pxf.i);
		_pxf.c = XFORM().c;

		m_current_firedeps.m_FireParticlesXForm.set(_pxf);
	}
}

LPCSTR wpn_scope_def_bone = "wpn_scope";
LPCSTR wpn_silencer_def_bone = "wpn_silencer";
LPCSTR wpn_launcher_def_bone_shoc = "wpn_launcher";
LPCSTR wpn_launcher_def_bone_cop = "wpn_grenade_launcher";

void CWeapon::Load		(LPCSTR section)
{
	inherited::Load					(section);
	CShootingObject::Load			(section);

	
	if(pSettings->line_exist(section, "flame_particles_2"))
		m_sFlameParticles2 = pSettings->r_string(section, "flame_particles_2");

#ifdef DEBUG
	{
		Fvector				pos,ypr;
		pos					= pSettings->r_fvector3		(section,"position");
		ypr					= pSettings->r_fvector3		(section,"orientation");
		ypr.mul				(PI/180.f);

		m_Offset.setHPB			(ypr.x,ypr.y,ypr.z);
		m_Offset.translate_over	(pos);
	}

	m_StrapOffset			= m_Offset;
	if (pSettings->line_exist(section,"strap_position") && pSettings->line_exist(section,"strap_orientation")) {
		Fvector				pos,ypr;
		pos					= pSettings->r_fvector3		(section,"strap_position");
		ypr					= pSettings->r_fvector3		(section,"strap_orientation");
		ypr.mul				(PI/180.f);

		m_StrapOffset.setHPB			(ypr.x,ypr.y,ypr.z);
		m_StrapOffset.translate_over	(pos);
	}
#endif

	// load ammo classes
	m_ammoTypes.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"ammo_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes.push_back	(_ammoItem);
		}
	}

	iAmmoElapsed		= pSettings->r_s32		(section,"ammo_elapsed"		);
	iMagazineSize		= pSettings->r_s32		(section,"ammo_mag_size"	);
	
	////////////////////////////////////////////////////
	// дисперсия стрельбы

	//подбрасывание камеры во время отдачи
	camMaxAngle			= pSettings->r_float		(section,"cam_max_angle"	); 
	camMaxAngle			= deg2rad					(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float		(section,"cam_relax_speed"	); 
	camRelaxSpeed		= deg2rad					(camRelaxSpeed);
	if (pSettings->line_exist(section, "cam_relax_speed_ai"))
	{
		camRelaxSpeed_AI		= pSettings->r_float		(section,"cam_relax_speed_ai"	); 
		camRelaxSpeed_AI		= deg2rad					(camRelaxSpeed_AI);
	}
	else
	{
		camRelaxSpeed_AI	= camRelaxSpeed;
	}
	
//	camDispersion		= pSettings->r_float		(section,"cam_dispersion"	); 
//	camDispersion		= deg2rad					(camDispersion);

	camMaxAngleHorz		= pSettings->r_float		(section,"cam_max_angle_horz"	); 
	camMaxAngleHorz		= deg2rad					(camMaxAngleHorz);
	camStepAngleHorz	= pSettings->r_float		(section,"cam_step_angle_horz"	); 
	camStepAngleHorz	= deg2rad					(camStepAngleHorz);	
	camDispertionFrac			= READ_IF_EXISTS(pSettings, r_float, section, "cam_dispertion_frac",	0.7f);
	//  [8/2/2005]
	//m_fParentDispersionModifier = READ_IF_EXISTS(pSettings, r_float, section, "parent_dispersion_modifier",1.0f);
	m_fPDM_disp_base			= READ_IF_EXISTS(pSettings, r_float, section, "PDM_disp_base",	1.0f);
	m_fPDM_disp_vel_factor		= READ_IF_EXISTS(pSettings, r_float, section, "PDM_disp_vel_factor",	1.0f);
	m_fPDM_disp_accel_factor	= READ_IF_EXISTS(pSettings, r_float, section, "PDM_disp_accel_factor",	1.0f);
	m_fPDM_disp_crouch			= READ_IF_EXISTS(pSettings, r_float, section, "PDM_disp_crouch",	1.0f);
	m_fPDM_disp_crouch_no_acc	= READ_IF_EXISTS(pSettings, r_float, section, "PDM_disp_crouch_no_acc",	1.0f);
	//  [8/2/2005]

	fireDispersionConditionFactor = pSettings->r_float(section,"fire_dispersion_condition_factor"); 
	misfireProbability			  = pSettings->r_float(section,"misfire_probability"); 
	misfireConditionK			  = READ_IF_EXISTS(pSettings, r_float, section, "misfire_condition_k",	1.0f);
	conditionDecreasePerShot	  = pSettings->r_float(section,"condition_shot_dec"); 
	conditionDecreasePerShotOnHit = READ_IF_EXISTS( pSettings, r_float, section, "condition_shot_dec_on_hit", 0.f );
	conditionDecreasePerShotSilencer = READ_IF_EXISTS( pSettings, r_float, section, "condition_shot_dec_silencer", conditionDecreasePerShot );
		
	vLoadedFirePoint	= pSettings->r_fvector3		(section,"fire_point"		);
	
	if(pSettings->line_exist(section,"fire_point2")) 
		vLoadedFirePoint2= pSettings->r_fvector3	(section,"fire_point2");
	else 
		vLoadedFirePoint2= vLoadedFirePoint;

	// hands
	eHandDependence		= EHandDependence(pSettings->r_s32(section,"hand_dependence"));
	m_bIsSingleHanded	= true;
	if (pSettings->line_exist(section, "single_handed"))
		m_bIsSingleHanded	= !!pSettings->r_bool(section, "single_handed");
	// 
	m_fMinRadius		= pSettings->r_float		(section,"min_radius");
	m_fMaxRadius		= pSettings->r_float		(section,"max_radius");


	// информация о возможных апгрейдах и их визуализации в инвентаре
	m_eScopeStatus			 = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"scope_status");
	m_eSilencerStatus		 = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"silencer_status");
	m_eGrenadeLauncherStatus = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"grenade_launcher_status");

	m_bZoomEnabled = !!pSettings->r_bool(section,"zoom_enabled");
	m_bUseScopeZoom = !!READ_IF_EXISTS(pSettings, r_bool, section, "use_scope_zoom", false);
	m_bUseScopeGrenadeZoom = !!READ_IF_EXISTS(pSettings, r_bool, section, "use_scope_grenade_zoom", false);
	m_bUseScopeDOF = !!READ_IF_EXISTS(pSettings, r_bool, section, "use_scope_dof", true);
	m_bForceScopeDOF = !!READ_IF_EXISTS(pSettings, r_bool, section, "force_scope_dof", false);
	m_bScopeShowIndicators = !!READ_IF_EXISTS(pSettings, r_bool, section, "scope_show_indicators", true);
	m_bIgnoreScopeTexture = !!READ_IF_EXISTS(pSettings, r_bool, section, "ignore_scope_texture", false);

	m_fZoomRotateTime = ROTATION_TIME;
	m_bScopeDynamicZoom = false;
	m_fScopeZoomFactor = 0;
	m_fRTZoomFactor = 0;

	UpdateZoomOffset();

        m_allScopeNames.clear();
	m_highlightAddons.clear();
	if(m_eScopeStatus == ALife::eAddonAttachable)
	{
		m_sScopeName = pSettings->r_string(section,"scope_name");
		m_iScopeX = pSettings->r_s32(section,"scope_x");
		m_iScopeY = pSettings->r_s32(section,"scope_y");

                m_allScopeNames.push_back( m_sScopeName );
                if ( pSettings->line_exist( section, "scope_names" ) ) {
                  LPCSTR S = pSettings->r_string( section, "scope_names" );
                  if ( S && S[ 0 ] ) {
                    string128 _scopeItem;
                    int count = _GetItemCount( S );
                    for ( int it = 0; it < count; ++it ) {
                      _GetItem( S, it, _scopeItem );
                      m_allScopeNames.push_back( _scopeItem );
                      m_highlightAddons.push_back( _scopeItem );
                    }
                  }
                }
	}

	if(m_eSilencerStatus == ALife::eAddonAttachable)
	{
		m_sSilencerName = pSettings->r_string(section,"silencer_name");
		m_iSilencerX = pSettings->r_s32(section,"silencer_x");
		m_iSilencerY = pSettings->r_s32(section,"silencer_y");
	}

	if(m_eGrenadeLauncherStatus == ALife::eAddonAttachable)
	{
		m_sGrenadeLauncherName = pSettings->r_string(section,"grenade_launcher_name");
		m_iGrenadeLauncherX = pSettings->r_s32(section,"grenade_launcher_x");
		m_iGrenadeLauncherY = pSettings->r_s32(section,"grenade_launcher_y");
	}

	// Кости мировой модели оружия
	if (pSettings->line_exist(section, "scope_bone"))
		m_sWpn_scope_bone = pSettings->r_string(section, "scope_bone");
	else
		m_sWpn_scope_bone = wpn_scope_def_bone;

	if (pSettings->line_exist(section, "silencer_bone"))
		m_sWpn_silencer_bone = pSettings->r_string(section, "silencer_bone");
	else
		m_sWpn_silencer_bone = wpn_silencer_def_bone;

	if (pSettings->line_exist(section, "launcher_bone"))
		m_sWpn_launcher_bone = pSettings->r_string(section, "launcher_bone");
	else
		m_sWpn_launcher_bone = wpn_launcher_def_bone_shoc;

	if (pSettings->line_exist(section, "hidden_bones"))
	{
		const char* S = pSettings->r_string(section, "hidden_bones");
		if (S && strlen(S))
		{
			const int count = _GetItemCount(S);
			string128 _hidden_bone{};
			for (int it = 0; it < count; ++it)
			{
				_GetItem(S, it, _hidden_bone);
				hidden_bones.push_back(_hidden_bone);
			}
		}
	}

	// Кости худовой модели оружия
	if (pSettings->line_exist(hud_sect, "scope_bone"))
		m_sHud_wpn_scope_bone = pSettings->r_string(hud_sect, "scope_bone");
	else
		m_sHud_wpn_scope_bone = wpn_scope_def_bone;

	if (pSettings->line_exist(hud_sect, "silencer_bone"))
		m_sHud_wpn_silencer_bone = pSettings->r_string(hud_sect, "silencer_bone");
	else
		m_sHud_wpn_silencer_bone = wpn_silencer_def_bone;

	if (pSettings->line_exist(hud_sect, "launcher_bone"))
		m_sHud_wpn_launcher_bone = pSettings->r_string(hud_sect, "launcher_bone");
	else
		m_sHud_wpn_launcher_bone = wpn_launcher_def_bone_shoc;

	if (pSettings->line_exist(hud_sect, "hidden_bones")) 
	{
		const char* S = pSettings->r_string(hud_sect, "hidden_bones");
		if (S && strlen(S))
		{
			const int count = _GetItemCount(S);
			string128 _hidden_bone{};
			for (int it = 0; it < count; ++it) 
			{
				_GetItem(S, it, _hidden_bone);
				hud_hidden_bones.push_back(_hidden_bone);
			}
		}
	}

	//Можно и из конфига прицела читать и наоборот! Пока так.
	m_fSecondVPZoomFactor = 0.0f;
	m_fZoomHudFov = 0.0f;
	m_fSecondVPHudFov = 0.0f;
	m_fScopeInertionFactor = m_fControlInertionFactor;

	InitAddons();

	m_bHideCrosshairInZoom = true;
	if(pSettings->line_exist(hud_sect, "zoom_hide_crosshair"))
		m_bHideCrosshairInZoom = !!pSettings->r_bool(hud_sect, "zoom_hide_crosshair");

	m_bZoomInertionAllow = READ_IF_EXISTS(pSettings, r_bool, hud_sect, "allow_zoom_inertion", READ_IF_EXISTS(pSettings, r_bool, "features", "default_allow_zoom_inertion", true));
	m_bScopeZoomInertionAllow = READ_IF_EXISTS(pSettings, r_bool, hud_sect, "allow_scope_zoom_inertion", false);

	//////////////////////////////////////////////////////////

	m_bHasTracers = READ_IF_EXISTS(pSettings, r_bool, section, "tracers", true);
	m_u8TracerColorID = READ_IF_EXISTS(pSettings, r_u8, section, "tracers_color_ID", u8(-1));

	string256						temp;
	for (int i=egdNovice; i<egdCount; ++i) {
		strconcat					(sizeof(temp),temp,"hit_probability_",get_token_name(difficulty_type_token,i));
		m_hit_probability[i]		= READ_IF_EXISTS(pSettings,r_float,section,temp,1.f);
	}
	
	if (pSettings->line_exist(section, "highlight_addons")) {
		LPCSTR S = pSettings->r_string(section, "highlight_addons");
		if (S && S[0]) {
			string128 _addonItem;
			int count = _GetItemCount(S);
			for (int it = 0; it < count; ++it) {
				_GetItem(S, it, _addonItem);
				ASSERT_FMT(pSettings->section_exist(_addonItem), "Section [%s] not found!", _addonItem);
				m_highlightAddons.emplace_back(_addonItem);
#ifdef OGSR_MOD
				if (pSettings->line_exist(_addonItem, "real_item_section")) //KRodin: Костыль для огсе-шной системы аддонов, т.к. мне лень по конфигам лазить.
					m_highlightAddons.emplace_back(pSettings->r_string(_addonItem, "real_item_section"));
#endif
			}
		}
	}

	m_nearwall_on = READ_IF_EXISTS(pSettings, r_bool, section, "nearwall_on", READ_IF_EXISTS(pSettings, r_bool, "features", "default_nearwall_on", true));
	if (m_nearwall_on)
	{
		// Параметры изменения HUD FOV когда игрок стоит вплотную к стене
		m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", 0.27f);
		m_nearwall_dist_min = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", 0.5f);
		m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", 1.f);
		m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);
	}

	////////////////////////////////////////////
	//--#SM+# Begin--
	string16 _prefix = { "" };
	//xr_sprintf(_prefix, "%s", UI()->is_widescreen() ? "_16x9" : ""); //KRodin: на мой взгляд это лишнее.

	string128 val_name;

	// Смещение в стрейфе
	m_strafe_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, xr_strconcat(val_name, "strafe_hud_offset_pos", _prefix), Fvector().set(0.015f, 0.f, 0.f));
	m_strafe_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, xr_strconcat(val_name, "strafe_hud_offset_rot", _prefix), Fvector().set(0.f, 0.f, 4.5f));

	// Поворот в стрейфе
	m_strafe_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, xr_strconcat(val_name, "strafe_aim_hud_offset_pos", _prefix), Fvector().set(0.005f, 0.f, 0.f));
	m_strafe_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, xr_strconcat(val_name, "strafe_aim_hud_offset_rot", _prefix), Fvector().set(0.f, 0.f, 2.5f));

	// Параметры стрейфа
	float fFullStrafeTime     = READ_IF_EXISTS(pSettings, r_float, section, "strafe_transition_time", 0.25f);
	float fFullStrafeTime_aim = READ_IF_EXISTS(pSettings, r_float, section, "strafe_aim_transition_time", 0.15f);
	bool bStrafeEnabled       = READ_IF_EXISTS(pSettings, r_bool, section, "strafe_enabled", READ_IF_EXISTS(pSettings, r_bool, "features", "default_strafe_enabled", true));
	bool bStrafeEnabled_aim   = READ_IF_EXISTS(pSettings, r_bool, section, "strafe_aim_enabled", false);

	m_strafe_offset[2][0].set(bStrafeEnabled, fFullStrafeTime, 0.f); // normal
	m_strafe_offset[2][1].set(bStrafeEnabled_aim, fFullStrafeTime_aim, 0.f); // aim-GL
	//--#SM+# End--
	////////////////////////////////////////////
}

void CWeapon::LoadFireParams		(LPCSTR section, LPCSTR prefix)
{
	camDispersion		= pSettings->r_float		(section,"cam_dispersion"	); 
	camDispersion		= deg2rad					(camDispersion);

	if (pSettings->line_exist(section,"cam_dispersion_inc"))
	{
		camDispersionInc		= pSettings->r_float		(section,"cam_dispersion_inc"	); 
		camDispersionInc		= deg2rad					(camDispersionInc);
	}
	else
		camDispersionInc = 0;

	CShootingObject::LoadFireParams(section, prefix);
};

void CWeapon::LoadZoomOffset(LPCSTR section, LPCSTR prefix)
{
	string256 full_name;
	/*if (Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system)
		&& is_second_zoom_offset_enabled //Если включен режим второго прицеливания
		&& !READ_IF_EXISTS(pSettings, r_bool, *cNameSect(), "disable_second_scope", false) //И второй прицел не запрещён (нужно для поддержки замороченной системы ogse_addons)
		&& pSettings->line_exist(hud_sect, strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_offset")) //И в секциии худа есть настройки для второго режима прицеливания
		&& pSettings->line_exist(hud_sect, strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_rotate_x"))
		&& pSettings->line_exist(hud_sect, strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_rotate_y"))
	) { //Используем настройки для второго режима прицеливания
		m_pHUD->SetZoomOffset(pSettings->r_fvector3(hud_sect, strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_offset")));
		m_pHUD->SetZoomRotateX(pSettings->r_float(hud_sect,   strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_rotate_x")));
		m_pHUD->SetZoomRotateY(pSettings->r_float(hud_sect,   strconcat(sizeof(full_name), full_name, "second_", prefix, "zoom_rotate_y")));
		//
		is_second_zoom_offset_enabled = true;
		//Msg("--Second scope enabled!");
	}
	else //В противном случае используем стандартные настройки
	{
		m_pHUD->SetZoomOffset(pSettings->r_fvector3(hud_sect, strconcat(sizeof(full_name), full_name, prefix, "zoom_offset")));
		m_pHUD->SetZoomRotateX(pSettings->r_float(hud_sect,   strconcat(sizeof(full_name), full_name, prefix, "zoom_rotate_x")));
		m_pHUD->SetZoomRotateY(pSettings->r_float(hud_sect,   strconcat(sizeof(full_name), full_name, prefix, "zoom_rotate_y")));
		//
		is_second_zoom_offset_enabled = false;
		//Msg("~~Second scope disabled!");
	}*/

	//Зум фактор обновлять здесь необходимо. second_soom_factor поддерживается.
	m_fZoomFactor = this->CurrentZoomFactor();
	//

	if(pSettings->line_exist(hud_sect, "zoom_rotate_time"))
		m_fZoomRotateTime = pSettings->r_float(hud_sect,"zoom_rotate_time");

	callback(GameObject::eOnSecondScopeSwitch)(is_second_zoom_offset_enabled); //Для нормальной поддержки скриптовых оружейных наворотов ОГСЕ
}

void CWeapon::UpdateZoomOffset() //Собрал все манипуляции с зум оффсетом сюда, чтоб были в одном месте.
{
	if (m_bZoomEnabled && HudItemData())
	{
		const bool has_gl = GrenadeLauncherAttachable() && IsGrenadeLauncherAttached();
		const bool has_scope = ScopeAttachable() && IsScopeAttached();

		if (IsGrenadeMode())
		{
			if (m_bUseScopeGrenadeZoom && has_scope)
				LoadZoomOffset(*hud_sect, "scope_grenade_");
			else
				LoadZoomOffset(*hud_sect, "grenade_");
		}
		else if (has_gl)
		{
			if (m_bUseScopeZoom && has_scope)
				LoadZoomOffset(*hud_sect, "scope_grenade_normal_");
			else
				LoadZoomOffset(*hud_sect, "grenade_normal_");
		}
		else
		{
			if (m_bUseScopeZoom && has_scope)
				LoadZoomOffset(*hud_sect, "scope_");
			else
				LoadZoomOffset(*hud_sect, "");
		}
	}
}

void CWeapon::SwitchScope()
{
	is_second_zoom_offset_enabled = !is_second_zoom_offset_enabled;
	UpdateZoomOffset();
}

BOOL CWeapon::net_Spawn		(CSE_Abstract* DC)
{
	BOOL bResult					= inherited::net_Spawn(DC);
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemWeapon			    *E	= smart_cast<CSE_ALifeItemWeapon*>(e);

	//iAmmoCurrent					= E->a_current;
	iAmmoElapsed					= E->a_elapsed;
	m_flagsAddOnState				= E->m_addon_flags.get();
	m_ammoType						= E->ammo_type;
	SetState						(E->wpn_state);
	SetNextState					(E->wpn_state);

	if ( m_ammoType >= m_ammoTypes.size() ) {
	  Msg( "! [%s]: %s: wrong m_ammoType[%u/%u]", __FUNCTION__, cName().c_str(), m_ammoType, m_ammoTypes.size() - 1 );
	  m_ammoType = 0;
	  auto se_obj = alife_object();
	  if ( se_obj ) {
	    auto W = smart_cast<CSE_ALifeItemWeapon*>( se_obj );
	    W->ammo_type = m_ammoType;
	  }
	}

	m_DefaultCartridge.Load(*m_ammoTypes[m_ammoType], u8(m_ammoType));	
	if(iAmmoElapsed) 
	{
		// нож автоматически заряжается двумя патронами, хотя
		// размер магазина у него 0. Что бы зря не ругаться, проверим
		// что в конфиге размер магазина не нулевой.
		if ( iMagazineSize && iAmmoElapsed > iMagazineSize ) {
		  Msg( "! [%s]: %s: wrong iAmmoElapsed[%u/%u]", __FUNCTION__, cName().c_str(), iAmmoElapsed, iMagazineSize );
		  iAmmoElapsed = iMagazineSize;
		  auto se_obj = alife_object();
		  if ( se_obj ) {
		    auto W = smart_cast<CSE_ALifeItemWeapon*>( se_obj );
		    W->a_elapsed = iAmmoElapsed;
		  }
		}
		m_fCurrentCartirdgeDisp = m_DefaultCartridge.m_kDisp;
		for(int i = 0; i < iAmmoElapsed; ++i) 
			m_magazine.push_back(m_DefaultCartridge);
	}


	UpdateAddonsVisibility();
	InitAddons();

	VERIFY((u32)iAmmoElapsed == m_magazine.size());

	return bResult;
}

void CWeapon::net_Destroy	()
{
	inherited::net_Destroy	();

	//удалить объекты партиклов
	StopFlameParticles	();
	StopFlameParticles2	();
	StopLight			();
	Light_Destroy		();

	m_magazine.clear();
	m_magazine.shrink_to_fit();
}

BOOL CWeapon::IsUpdating()
{	
	bool bIsActiveItem = m_pCurrentInventory && m_pCurrentInventory->ActiveItem()==this;
	return bIsActiveItem || bWorking || IsPending() || getVisible();
}

void CWeapon::net_Export(NET_Packet& P)
{
	inherited::net_Export	(P);

	P.w_float_q8			(m_fCondition,0.0f,1.0f);

	u8 need_upd				= IsUpdating() ? 1 : 0;
	P.w_u8					(need_upd);
	P.w_u16					(u16(iAmmoElapsed));
	P.w_u8					(m_flagsAddOnState);
	P.w_u8					((u8)m_ammoType);
	P.w_u8					((u8)GetState());
	P.w_u8					((u8)m_bZoomMode);
}

void CWeapon::net_Import(NET_Packet& P)
{
	inherited::net_Import (P);

	P.r_float_q8			(m_fCondition,0.0f,1.0f);

	u8 flags				= 0;
	P.r_u8					(flags);

	u16 ammo_elapsed = 0;
	P.r_u16					(ammo_elapsed);

	u8						NewAddonState;
	P.r_u8					(NewAddonState);

	m_flagsAddOnState		= NewAddonState;
	UpdateAddonsVisibility	();

	u8 ammoType, wstate;
	P.r_u8					(ammoType);
	P.r_u8					(wstate);

	u8 Zoom;
	P.r_u8					(Zoom);

	if (H_Parent() && H_Parent()->Remote())
	{
		if (Zoom) 
			OnZoomIn();
		else 
			OnZoomOut();
	};
	switch (wstate)
	{	
	case eFire:
	case eFire2:
	case eSwitch:
	case eReload:
		{
		}break;	
	default:
		{
			if (ammoType >= m_ammoTypes.size())
				Msg("!! Weapon [%d], State - [%d]", ID(), wstate);
			else
			{
				m_ammoType = ammoType;
				SetAmmoElapsed((ammo_elapsed));
			}
		}break;
	}
	
	VERIFY((u32)iAmmoElapsed == m_magazine.size());
}

void CWeapon::save(NET_Packet &output_packet)
{
	inherited::save	(output_packet);
	save_data		(iAmmoElapsed,		output_packet);
	save_data		(m_flagsAddOnState, output_packet);
	save_data		(m_ammoType,		output_packet);
	save_data		(m_bZoomMode,		output_packet);
}

void CWeapon::load(IReader &input_packet)
{
	inherited::load	(input_packet);
	load_data		(iAmmoElapsed,		input_packet);
	load_data		(m_flagsAddOnState, input_packet);
	UpdateAddonsVisibility	();
	load_data		(m_ammoType,		input_packet);
	load_data		(m_bZoomMode,		input_packet);

	if (m_bZoomMode)	OnZoomIn();
		else			OnZoomOut();
}


void CWeapon::OnEvent(NET_Packet& P, u16 type) 
{
	switch (type)
	{
	case GE_ADDON_CHANGE:
		{
			P.r_u8					(m_flagsAddOnState);
			InitAddons();
			UpdateAddonsVisibility();
		}break;

	case GE_WPN_STATE_CHANGE:
		{
			u8				state;
			P.r_u8			(state);
			P.r_u8			(m_sub_state);		
//			u8 NewAmmoType = 
				P.r_u8();
			u8 AmmoElapsed = P.r_u8();
			u8 NextAmmo = P.r_u8();
			if (NextAmmo == u8(-1))
				m_set_next_ammoType_on_reload = u32(-1);
			else
				m_set_next_ammoType_on_reload = u8(NextAmmo);

			if (OnClient())
				SetAmmoElapsed(int(AmmoElapsed));			
			OnStateSwitch(u32(state), GetState());
		}
		break;
	default:
		{
			inherited::OnEvent(P,type);
		}break;
	}
};

void CWeapon::shedule_Update	(u32 dT)
{
	// Queue shrink
//	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
//	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();	

	// Inherited
	inherited::shedule_Update	(dT);
}

void CWeapon::OnH_B_Independent	(bool just_before_destroy)
{
	RemoveShotEffector			();

	inherited::OnH_B_Independent(just_before_destroy);

	//завершить принудительно все процессы что шли
	FireEnd						();
	SetPending					(FALSE);
	SwitchState					(eIdle);

	m_strapped_mode				= false;
	OnZoomOut					();
	m_fZoomRotationFactor	= 0.f;
	UpdateXForm					();

	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

void CWeapon::OnH_A_Independent	()
{
	inherited::OnH_A_Independent();
	Light_Destroy				();
};

void CWeapon::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();

	UpdateAddonsVisibility		();
};

void CWeapon::OnActiveItem ()
{
	inherited::OnActiveItem		();
	//если мы занружаемся и оружие было в руках
	SetState					(eIdle);
	SetNextState				(eIdle);
}

void CWeapon::OnHiddenItem ()
{
	inherited::OnHiddenItem();
	SetState					(eHidden);
	SetNextState				(eHidden);
	m_set_next_ammoType_on_reload	= u32(-1);
}


void CWeapon::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();

	OnZoomOut					();
	m_set_next_ammoType_on_reload	= u32(-1);

	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

static float state_time = 0;				// таймер нахождения оружия в текущем состоянии
static float state_time_heat = 0;			// таймер нагрева оружия
static float previous_heating = 0;		// "нагретость" оружия в предыдущем состоянии

#include "WeaponBinoculars.h"

void CWeapon::UpdateWeaponParams()
{
#pragma todo("KRodin: адаптировать тепловизор и тп. под новый рендер, если это возможно.")

	if (!IsHidden()) {
		w_states.x = m_fZoomRotationFactor;			//x = zoom mode, y - текущее состояние, z - старое состояние
		if (psActorFlags.test(AF_DOF_SCOPE) && !(IsZoomed() && !IsRotatingToZoom() && (IsScopeAttached() || m_bForceScopeDOF) && !IsGrenadeMode() && m_bUseScopeDOF))
			w_states.x = 0.f;
		if (w_states.y != GetState())	// первый апдейт или стейт изменился
		{
			w_states.z = w_states.y;						// записываем старое состояние
			state_time_heat = state_time = Device.fTimeGlobal;	// инитим счетчики времени
			previous_heating = w_timers.z;				// сохраняем "нагретость" оружия
			w_timers.y = w_timers.x;						// записываем время нахождения в предыдущем состоянии
			w_states.y = (float)GetState();				// обновляем состояние
		}
		// флаг бинокля в руках (в этом режиме не нужно размытие)
		if (smart_cast<CWeaponBinoculars*>(this))
			w_states.w = 0;
		else
			w_states.w = 1;
		if ((w_states.y == eFire) || (w_states.y == eFire2))	//стреляем, значит оружие греется
		{
			w_timers.z = Device.fTimeGlobal - state_time_heat + previous_heating;
		}
		else		// не стреляем - оружие охлаждается
		{
			if (w_timers.z > EPS)		// оружие все еще нагрето
			{
				float tm = state_time_heat + previous_heating - Device.fTimeGlobal;
				w_timers.z = (tm<EPS) ? 0.f : tm;
			}
		}
		w_timers.x = Device.fTimeGlobal - state_time;		// обновляем таймер текущего состояния
	}
}


u8 CWeapon::idle_state() {
	auto* actor = smart_cast<CActor*>(H_Parent());

	if (actor) {
		u32 st = actor->get_state();
		if (st & mcSprint)
			return eSubstateIdleSprint;
		else if (st & mcAnyAction && !(st & mcJump) && !(st & mcFall))
			return eSubstateIdleMoving;
	}

	return eIdle;
}


void CWeapon::UpdateCL		()
{
	inherited::UpdateCL		();

	UpdateHUDAddonsVisibility();

	//подсветка от выстрела
	UpdateLight				();

	if (ParentIsActor())
		UpdateWeaponParams();	// параметры для рендера оружия в режиме тепловидения

	//нарисовать партиклы
	UpdateFlameParticles	();
	UpdateFlameParticles2	();
	
	VERIFY(smart_cast<IKinematics*>(Visual()));

        if ( GetState() == eIdle ) {
          auto state = idle_state();
          if ( m_idle_state != state ) {
            m_idle_state = state;
			if (GetNextState() != eMagEmpty && GetNextState() != eReload)
			{
				SwitchState(eIdle);
			}
          }
        }
        else
          m_idle_state = eIdle;
}


void CWeapon::renderable_Render		()
{
	//KRodin: чтоб ствол в руках актора не был виден внутри 3D прицела.
	if (Device.m_SecondViewport.IsSVPFrame() && this->m_fZoomRotationFactor > 0.05f)
		return;

	UpdateXForm				();

	//нарисовать подсветку
	RenderLight				();	

	//если мы в режиме снайперки, то сам HUD рисовать не надо
	if(IsZoomed() && !IsRotatingToZoom() && ZoomTexture())
		RenderHud(FALSE);
	else
		RenderHud(TRUE);

	inherited::renderable_Render		();
}

bool CWeapon::need_renderable() 
{
	return !(IsZoomed() && ZoomTexture() && !IsRotatingToZoom()); 
}

bool CWeapon::MovingAnimAllowedNow()
{ 
	return !IsZoomed(); 
}

void CWeapon::signal_HideComplete()
{
	if(H_Parent())
		setVisible(FALSE);
	SetPending(FALSE);
}

void CWeapon::SetDefaults()
{
	bWorking2			= false;
	SetPending			(FALSE);

	m_flags.set			(FUsingCondition, TRUE);
	bMisfire			= false;
	m_flagsAddOnState	= 0;
	m_bZoomMode			= false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans)
{
	Position().set		(trans.c);
	XFORM().mul			(trans,m_strapped_mode ? m_StrapOffset : m_Offset);
	VERIFY				(!fis_zero(DET(renderable.xform)));
}


bool CWeapon::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	
	switch(cmd) 
	{
		case kWPN_FIRE: 
			{
				//если оружие чем-то занято, то ничего не делать
				{				
					if(flags&CMD_START) 
					{
						if(IsPending())		return false;
						FireStart			();
					}else 
						FireEnd();
				};

			} 
			return true;
		case kWPN_NEXT: 
			{
				if(IsPending() || OnClient()) 
				{
					return false;
				}
									
				if ( Core.Features.test(xrCore::Feature::lock_reload_in_sprint) && ParentIsActor() && g_actor->get_state() & mcSprint )
				  return true;

				if(flags&CMD_START) 
				{
					u32 l_newType = m_ammoType;
					bool b1, b2;
					do
					{
						l_newType = (l_newType + 1) % m_ammoTypes.size();
						b1 = l_newType != m_ammoType;
						b2 = unlimited_ammo() ? false : (!m_pCurrentInventory->GetAmmo(*m_ammoTypes[l_newType], ParentIsActor()));
					} while (b1 && b2);

					if (l_newType != m_ammoType)
					{
						m_set_next_ammoType_on_reload = l_newType;
						if (OnServer()) Reload();
					}
				}
			} 
            return true;

		case kWPN_ZOOM:
		{
			if (IsZoomEnabled())
			{
				if (flags & CMD_START && !IsPending())
				{
					if (psActorFlags.is(AF_WPN_AIM_TOGGLE) && IsZoomed())
					{
						OnZoomOut();
					}
					else
						OnZoomIn();
				}
				else if (IsZoomed() && !psActorFlags.is(AF_WPN_AIM_TOGGLE))
				{
					OnZoomOut();
				}
				return true;
			}
			else
				return false;
		}

		case kWPN_ZOOM_INC:
		case kWPN_ZOOM_DEC:
		{
			if (IsZoomEnabled() && IsZoomed() && m_bScopeDynamicZoom && IsScopeAttached() && !is_second_zoom_offset_enabled && (flags&CMD_START))
			{
				// если в режиме ПГ - не будем давать использовать динамический зум
				if (IsGrenadeMode())
					return false;

				ZoomChange(cmd == kWPN_ZOOM_INC);

				return true;
			}
			else
				return false;
		}
		case kSWITCH_SCOPE: //KRodin: заюзаем эту кнопку, один хрен она только в мультиплеере нужна, а он вырезан.
		{
			if (flags&CMD_START)
			{
				this->SwitchScope();
			}
		}
	}
	return false;
}

void CWeapon::GetZoomData(const float scope_factor, float& delta, float& min_zoom_factor)
{
	float def_fov = Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system) ? 1.f : g_fov;
	float delta_factor_total = def_fov-scope_factor;
	VERIFY(delta_factor_total>0);
	min_zoom_factor = def_fov-delta_factor_total*m_fMinZoomK;
	delta = (delta_factor_total*(1-m_fMinZoomK) )/m_fZoomStepCount;
}

void CWeapon::ZoomChange(bool inc)
{
	bool wasChanged = false;

	if (SecondVPEnabled())
	{
		float delta, min_zoom_factor;
		GetZoomData(m_fSecondVPZoomFactor, delta, min_zoom_factor);

		const float currentZoomFactor = m_fRTZoomFactor;

		if (Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system)) {
			m_fRTZoomFactor += delta * (inc ? 1 : -1);
			clamp(m_fRTZoomFactor, min_zoom_factor, m_fSecondVPZoomFactor);
		}
		else {
			m_fRTZoomFactor += delta * (inc ? 1 : -1);
			clamp(m_fRTZoomFactor, m_fSecondVPZoomFactor, min_zoom_factor);
		}

		wasChanged = !fsimilar(currentZoomFactor, m_fRTZoomFactor);
	}
	else
	{
		float delta, min_zoom_factor;
		GetZoomData(m_fScopeZoomFactor, delta, min_zoom_factor);

		const float currentZoomFactor = m_fZoomFactor;

		if (Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system)) {
			m_fZoomFactor += delta * (inc ? 1 : -1);
			clamp(m_fZoomFactor, min_zoom_factor, m_fScopeZoomFactor);
		}
		else {
			m_fZoomFactor -= delta * (inc ? 1 : -1);
			clamp(m_fZoomFactor, m_fScopeZoomFactor, min_zoom_factor);
		}

		wasChanged = !fsimilar(currentZoomFactor, m_fZoomFactor);

		if (H_Parent() && !IsRotatingToZoom() && !SecondVPEnabled())
			m_fRTZoomFactor = m_fZoomFactor; //store current
	}

	if (wasChanged)
	{
		OnZoomChanged();
	}
}

void CWeapon::SpawnAmmo(u32 boxCurr, LPCSTR ammoSect, u32 ParentID) 
{
	if(!m_ammoTypes.size())			return;
	if (OnClient())					return;
	
	if (!ammoSect) ammoSect = m_ammoTypes.front().c_str();
	
	CSE_Abstract *D					= F_entity_Create(ammoSect);

	if (D->m_tClassID==CLSID_OBJECT_AMMO	||
		D->m_tClassID==CLSID_OBJECT_A_M209	||
		D->m_tClassID==CLSID_OBJECT_A_VOG25	||
		D->m_tClassID==CLSID_OBJECT_A_OG7B)
	{	
		CSE_ALifeItemAmmo *l_pA		= smart_cast<CSE_ALifeItemAmmo*>(D);
		R_ASSERT					(l_pA);
		l_pA->m_boxSize				= (u16)pSettings->r_s32(ammoSect, "box_size");
		D->s_name					= ammoSect;
		D->set_name_replace			("");
		D->s_gameid					= u8(GameID());
		D->s_RP						= 0xff;
		D->ID						= 0xffff;
		if (ParentID == 0xffffffff)	
			D->ID_Parent			= (u16)H_Parent()->ID();
		else
			D->ID_Parent			= (u16)ParentID;

		D->ID_Phantom				= 0xffff;
		D->s_flags.assign			(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime				= 0;
		l_pA->m_tNodeID				= ai_location().level_vertex_id();

		if(boxCurr == 0xffffffff) 	
			boxCurr					= l_pA->m_boxSize;

		while(boxCurr) 
		{
			l_pA->a_elapsed			= (u16)(boxCurr > l_pA->m_boxSize ? l_pA->m_boxSize : boxCurr);
			NET_Packet				P;
			D->Spawn_Write			(P, TRUE);
			Level().Send			(P,net_flags(TRUE));

			if(boxCurr > l_pA->m_boxSize) 
				boxCurr				-= l_pA->m_boxSize;
			else 
				boxCurr				= 0;
		}
	};
	F_entity_Destroy				(D);
}

int CWeapon::GetAmmoCurrent(bool use_item_to_spawn) const
{
	int l_count = iAmmoElapsed;
	if(!m_pCurrentInventory) return l_count;

	//чтоб не делать лишних пересчетов
	if(m_pCurrentInventory->ModifyFrame()<=m_dwAmmoCurrentCalcFrame)
		return l_count + iAmmoCurrent;

 	m_dwAmmoCurrentCalcFrame = Device.dwFrame;
	iAmmoCurrent = 0;

	for(int i = 0; i < (int)m_ammoTypes.size(); ++i) 
	{
		iAmmoCurrent += GetAmmoCount_forType( m_ammoTypes[i] );

		if (!use_item_to_spawn)
			continue;

		if (!inventory_owner().item_to_spawn())
			continue;

		iAmmoCurrent += inventory_owner().ammo_in_box_to_spawn();
	}
	return l_count + iAmmoCurrent;
}

int CWeapon::GetAmmoCount( u8 ammo_type, u32 max ) const {
  VERIFY( m_pInventory );
  R_ASSERT( ammo_type < m_ammoTypes.size() );

  return GetAmmoCount_forType( m_ammoTypes[ ammo_type ], max );
}

int CWeapon::GetAmmoCount_forType( shared_str const& ammo_type, u32 max ) const {
  u32 res = 0;
  auto callback = [&]( const auto pIItem ) -> bool {
    auto* ammo = smart_cast<CWeaponAmmo*>( pIItem );
    if ( ammo->cNameSect() == ammo_type )
      res += ammo->m_boxCurr;
    return ( max > 0 && res >= max );
  };

  m_pCurrentInventory->IterateAmmo( false, callback );
  if ( max == 0 || res < max )
    if ( !smart_cast<const CActor*>( H_Parent() ) || !psActorFlags.test( AF_AMMO_ON_BELT ) )
      m_pCurrentInventory->IterateAmmo( true, callback );

  return res;
}

float CWeapon::GetConditionMisfireProbability() const
{
	if( GetCondition()>0.95f ) return 0.0f;

	float mis = misfireProbability+powf(1.f-GetCondition(), 3.f)*misfireConditionK;
	clamp(mis,0.0f,0.99f);
	return mis;
}

BOOL CWeapon::CheckForMisfire	()
{
	if (OnClient()) return FALSE;

	if ( Core.Features.test( xrCore::Feature::npc_simplified_shooting ) ) {
	  CActor *actor = smart_cast<CActor*>( H_Parent() );
	  if ( !actor ) return FALSE;
	}

	float rnd = ::Random.randF(0.f,1.f);
	float mp = GetConditionMisfireProbability();
	if(rnd < mp)
	{
		FireEnd();

		bMisfire = true;
		SwitchState(eMisfire);		
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CWeapon::IsMisfire() const
{	
	return bMisfire;
}
void CWeapon::Reload()
{
	OnZoomOut();
}


bool CWeapon::IsGrenadeLauncherAttached() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eGrenadeLauncherStatus;
}

bool CWeapon::IsScopeAttached() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eScopeStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eScopeStatus;

}

bool CWeapon::IsSilencerAttached() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eSilencerStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eSilencerStatus;
}

bool CWeapon::GrenadeLauncherAttachable() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus);
}
bool CWeapon::ScopeAttachable() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eScopeStatus);
}
bool CWeapon::SilencerAttachable() const
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eSilencerStatus);
}

void CWeapon::UpdateHUDAddonsVisibility()
{
	if (H_Parent() != Level().CurrentEntity()) //actor only
		return;

	if (!GetHUDmode())
		return;

	if (ScopeAttachable())
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_scope_bone, IsScopeAttached());
	}

	if (m_eScopeStatus == ALife::eAddonDisabled)
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_scope_bone, FALSE, TRUE);
	}
	else if (m_eScopeStatus == ALife::eAddonPermanent)
		HudItemData()->set_bone_visible(m_sHud_wpn_scope_bone, TRUE, TRUE);

	if (SilencerAttachable())
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_silencer_bone, IsSilencerAttached());
	}
	if (m_eSilencerStatus == ALife::eAddonDisabled)
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_silencer_bone, FALSE, TRUE);
	}
	else if (m_eSilencerStatus == ALife::eAddonPermanent)
		HudItemData()->set_bone_visible(m_sHud_wpn_silencer_bone, TRUE, TRUE);

	if (!HudItemData()->has_bone(m_sHud_wpn_launcher_bone) && HudItemData()->has_bone(wpn_launcher_def_bone_cop))
		m_sHud_wpn_launcher_bone = wpn_launcher_def_bone_cop;

	if (GrenadeLauncherAttachable())
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_launcher_bone, IsGrenadeLauncherAttached());
	}
	if (m_eGrenadeLauncherStatus == ALife::eAddonDisabled)
	{
		HudItemData()->set_bone_visible(m_sHud_wpn_launcher_bone, FALSE, TRUE);
	}
	else if (m_eGrenadeLauncherStatus == ALife::eAddonPermanent)
		HudItemData()->set_bone_visible(m_sHud_wpn_launcher_bone, TRUE, TRUE);

	///////////////////////////////////////////////////////////////////

	for (const shared_str& bone_name : hud_hidden_bones)
	{
		HudItemData()->set_bone_visible(bone_name, FALSE, TRUE);
	}

	callback(GameObject::eOnUpdateHUDAddonsVisibiility)();
}

void CWeapon::UpdateAddonsVisibility()
{
	auto pWeaponVisual = smart_cast<IKinematics*>(Visual());
	VERIFY(pWeaponVisual);

	UpdateHUDAddonsVisibility();

	callback(GameObject::eOnUpdateAddonsVisibiility)();

	///////////////////////////////////////////////////////////////////

	u16 bone_id = pWeaponVisual->LL_BoneID(m_sWpn_scope_bone);

	if (ScopeAttachable())
	{
		if (IsScopeAttached())
		{
			if (!pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else
		{
			if (pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}

	if (m_eScopeStatus == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
	else if (m_eScopeStatus == CSE_ALifeItemWeapon::eAddonPermanent && bone_id != BI_NONE && !pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);

	///////////////////////////////////////////////////////////////////

	bone_id = pWeaponVisual->LL_BoneID(m_sWpn_silencer_bone);

	if (SilencerAttachable())
	{
		if (IsSilencerAttached())
		{
			if (!pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else
		{
			if (pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}

	if (m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
	else if (m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonPermanent && bone_id != BI_NONE && !pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);

	///////////////////////////////////////////////////////////////////

	bone_id = pWeaponVisual->LL_BoneID(m_sWpn_launcher_bone);

	if (GrenadeLauncherAttachable())
	{
		if (IsGrenadeLauncherAttached())
		{
			if (!pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
		}
		else
		{
			if (pWeaponVisual->LL_GetBoneVisible(bone_id))
				pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}
	}

	if (m_eGrenadeLauncherStatus == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
	else if (m_eGrenadeLauncherStatus == CSE_ALifeItemWeapon::eAddonPermanent && bone_id != BI_NONE && !pWeaponVisual->LL_GetBoneVisible(bone_id))
		pWeaponVisual->LL_SetBoneVisible(bone_id, TRUE, TRUE);

	///////////////////////////////////////////////////////////////////

	for (const auto& bone_name : hidden_bones)
	{
		bone_id = pWeaponVisual->LL_BoneID(bone_name);
		if (bone_id != BI_NONE && pWeaponVisual->LL_GetBoneVisible(bone_id))
			pWeaponVisual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
	}

	///////////////////////////////////////////////////////////////////

	pWeaponVisual->CalculateBones_Invalidate();
	pWeaponVisual->CalculateBones();
}


bool CWeapon::Activate( bool now ) 
{
	UpdateAddonsVisibility();
	return inherited::Activate( now );
}

void CWeapon::InitAddons()
{
}

float CWeapon::CurrentZoomFactor()
{
	if (is_second_zoom_offset_enabled)
		return m_fSecondScopeZoomFactor;
	else if (SecondVPEnabled())
		return Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system) ? 1.0f : m_fIronSightZoomFactor; // no change to main fov zoom when use second vp
	else if (IsScopeAttached())
		return m_fScopeZoomFactor;
	else
		return m_fIronSightZoomFactor;
}

void CWeapon::OnZoomIn()
{
	m_bZoomMode = true;

	// если в режиме ПГ - не будем давать включать динамический зум
	if ( m_bScopeDynamicZoom && !IsGrenadeMode() && !SecondVPEnabled())
		m_fZoomFactor = m_fRTZoomFactor;
	else
		m_fZoomFactor = CurrentZoomFactor();

	if(GetHUDmode())
		GamePersistent().SetPickableEffectorDOF(true);

	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if ( pActor )
		pActor->callback(GameObject::eOnActorWeaponZoomIn)(lua_game_object());
}

void CWeapon::OnZoomOut()
{
	m_fZoomFactor = Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system) ? 1.f : g_fov;

	if ( m_bZoomMode ) {

		m_bZoomMode = false;

		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if ( pActor ) {
			w_states.set( 0.f, 0.f, 0.f, 1.f );
			pActor->callback(GameObject::eOnActorWeaponZoomOut)(lua_game_object());
		}
	}

	if (GetHUDmode())
		GamePersistent().SetPickableEffectorDOF(false);

	ResetSubStateTime();
}

bool CWeapon::UseScopeTexture() {
	return (( GetAddonsState() & CSE_ALifeItemWeapon::eForcedNotexScope ) == 0) 
		&& !is_second_zoom_offset_enabled
		&& !SecondVPEnabled()
		&& m_UIScope; // только если есть текстура прицела - для простого создания коллиматоров
};

CUIStaticItem* CWeapon::ZoomTexture()
{
	if (UseScopeTexture())
		return m_UIScope;
	else
		return NULL;
}

void CWeapon::SwitchState(u32 S)
{
	if (OnClient()) return;

	SetNextState		( S );	// Very-very important line of code!!! :)
	if (CHudItem::object().Local() && !CHudItem::object().getDestroy()/* && (S!=NEXT_STATE)*/ 
		&& m_pCurrentInventory && OnServer())	
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		CHudItem::object().u_EventGen		(P,GE_WPN_STATE_CHANGE,CHudItem::object().ID());
		P.w_u8			(u8(S));
		P.w_u8			(u8(m_sub_state));
		P.w_u8			(u8(m_ammoType& 0xff));
		P.w_u8			(u8(iAmmoElapsed & 0xff));
		P.w_u8			(u8(m_set_next_ammoType_on_reload & 0xff));
		CHudItem::object().u_EventSend		(P, net_flags(TRUE, TRUE, FALSE, TRUE));
	}
}

void CWeapon::OnMagazineEmpty	()
{
	VERIFY((u32)iAmmoElapsed == m_magazine.size());
}


void CWeapon::reinit			()
{
	CShootingObject::reinit		();
	CHudItemObject::reinit			();
}

void CWeapon::reload			(LPCSTR section)
{
	CShootingObject::reload		(section);
	CHudItemObject::reload			(section);
	
	m_can_be_strapped			= true;
	m_strapped_mode				= false;
	
	if (pSettings->line_exist(section,"strap_bone0"))
		m_strap_bone0			= pSettings->r_string(section,"strap_bone0");
	else
		m_can_be_strapped		= false;
	
	if (pSettings->line_exist(section,"strap_bone1"))
		m_strap_bone1			= pSettings->r_string(section,"strap_bone1");
	else
		m_can_be_strapped		= false;

	if (m_eScopeStatus == ALife::eAddonAttachable) {
		m_addon_holder_range_modifier	= READ_IF_EXISTS(pSettings,r_float,m_sScopeName,"holder_range_modifier",m_holder_range_modifier);
		m_addon_holder_fov_modifier		= READ_IF_EXISTS(pSettings,r_float,m_sScopeName,"holder_fov_modifier",m_holder_fov_modifier);
	}
	else {
		m_addon_holder_range_modifier	= m_holder_range_modifier;
		m_addon_holder_fov_modifier		= m_holder_fov_modifier;
	}


	{
		Fvector				pos,ypr;
		pos					= pSettings->r_fvector3		(section,"position");
		ypr					= pSettings->r_fvector3		(section,"orientation");
		ypr.mul				(PI/180.f);

		m_Offset.setHPB			(ypr.x,ypr.y,ypr.z);
		m_Offset.translate_over	(pos);
	}

	m_StrapOffset			= m_Offset;
	if (pSettings->line_exist(section,"strap_position") && pSettings->line_exist(section,"strap_orientation")) {
		Fvector				pos,ypr;
		pos					= pSettings->r_fvector3		(section,"strap_position");
		ypr					= pSettings->r_fvector3		(section,"strap_orientation");
		ypr.mul				(PI/180.f);

		m_StrapOffset.setHPB			(ypr.x,ypr.y,ypr.z);
		m_StrapOffset.translate_over	(pos);
	}
	else
		m_can_be_strapped	= false;

	m_ef_main_weapon_type	= READ_IF_EXISTS(pSettings,r_u32,section,"ef_main_weapon_type",u32(-1));
	m_ef_weapon_type		= READ_IF_EXISTS(pSettings,r_u32,section,"ef_weapon_type",u32(-1));
}

void CWeapon::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CWeapon::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CWeapon::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

bool CWeapon::can_kill	() const
{
	if (GetAmmoCurrent(true) || m_ammoTypes.empty())
		return				(true);

	return					(false);
}

CInventoryItem *CWeapon::can_kill	(CInventory *inventory) const
{
	if (GetAmmoElapsed() || m_ammoTypes.empty())
		return				(const_cast<CWeapon*>(this));

	TIItemContainer::iterator I = inventory->m_all.begin();
	TIItemContainer::iterator E = inventory->m_all.end();
	for ( ; I != E; ++I) {
		CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(*I);
		if (!inventory_item)
			continue;
		
		xr_vector<shared_str>::const_iterator	i = std::find(m_ammoTypes.begin(),m_ammoTypes.end(),inventory_item->object().cNameSect());
		if (i != m_ammoTypes.end())
			return			(inventory_item);
	}

	return					(0);
}

const CInventoryItem *CWeapon::can_kill	(const xr_vector<const CGameObject*> &items) const
{
	if (m_ammoTypes.empty())
		return				(this);

	xr_vector<const CGameObject*>::const_iterator I = items.begin();
	xr_vector<const CGameObject*>::const_iterator E = items.end();
	for ( ; I != E; ++I) {
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item)
			continue;

		xr_vector<shared_str>::const_iterator	i = std::find(m_ammoTypes.begin(),m_ammoTypes.end(),inventory_item->object().cNameSect());
		if (i != m_ammoTypes.end())
			return			(inventory_item);
	}

	return					(0);
}

bool CWeapon::ready_to_kill	() const
{
	return					(
		!IsMisfire() && 
		((GetState() == eIdle) || (GetState() == eFire) || (GetState() == eFire2)) && 
		GetAmmoElapsed()
	);
}

// Получить индекс текущих координат худа
u8 CWeapon::GetCurrentHudOffsetIdx()
{
	auto pActor = smart_cast<const CActor*>(H_Parent());
	if (!pActor)
		return 0;

	bool b_aiming = ((IsZoomed() && /*m_zoom_params.*/m_fZoomRotationFactor <= 1.f) || (!IsZoomed() && /*m_zoom_params.*/m_fZoomRotationFactor > 0.f));

	if (!b_aiming)
		return 0;
	else if (IsGrenadeMode())
		return 2;
	else
		return 1;
}


void CWeapon::UpdateHudAdditonal		(Fmatrix& trans)
{
	auto pActor = smart_cast<const CActor*>(H_Parent());
	if(!pActor) return;

	u8 idx = GetCurrentHudOffsetIdx();

	if(		(pActor->IsZoomAimingMode() && m_fZoomRotationFactor<=1.f) ||
			(!pActor->IsZoomAimingMode() && m_fZoomRotationFactor>0.f))
	{
		u8 idx = GetCurrentHudOffsetIdx();

		attachable_hud_item* hi = HudItemData();
		R_ASSERT(hi);
		Fvector curr_offs, curr_rot;
		curr_offs = hi->m_measures.m_hands_offset[0][idx]; // pos,aim
		curr_rot = hi->m_measures.m_hands_offset[1][idx]; // rot,aim
		curr_offs.mul(m_fZoomRotationFactor);
		curr_rot.mul(m_fZoomRotationFactor);

		Fmatrix hud_rotation;
		hud_rotation.identity();
		hud_rotation.rotateX(curr_rot.x);

		Fmatrix hud_rotation_y;
		hud_rotation_y.identity();
		hud_rotation_y.rotateY(curr_rot.y);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation_y.identity();
		hud_rotation_y.rotateZ(curr_rot.z);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation.translate_over(curr_offs);
		trans.mulB_43(hud_rotation);

		if(pActor->IsZoomAimingMode())
		{
			m_fZoomRotationFactor += Device.fTimeDelta/m_fZoomRotateTime;
		}
		else
		{
			m_fZoomRotationFactor -= Device.fTimeDelta/m_fZoomRotateTime;
		}
		clamp(m_fZoomRotationFactor, 0.f, 1.f);
	}

	// Боковой стрейф с оружием
	clamp(idx, 0ui8, 1ui8);

	// Рассчитываем фактор боковой ходьбы
	float fStrafeMaxTime = /*hi->m_measures.*/m_strafe_offset[2][idx].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
	if (fStrafeMaxTime <= EPS)
		fStrafeMaxTime = 0.01f;

	float fStepPerUpd = Device.fTimeDelta / fStrafeMaxTime; // Величина изменение фактора поворота

	u32 iMovingState = pActor->MovingState();
	if ((iMovingState & mcLStrafe) != 0)
	{ // Движемся влево
		float fVal = (m_fLR_MovingFactor > 0.f ? fStepPerUpd * 3 : fStepPerUpd);
		m_fLR_MovingFactor -= fVal;
	}
	else if ((iMovingState & mcRStrafe) != 0)
	{ // Движемся вправо
		float fVal = (m_fLR_MovingFactor < 0.f ? fStepPerUpd * 3 : fStepPerUpd);
		m_fLR_MovingFactor += fVal;
	}
	else
	{ // Двигаемся в любом другом направлении
		if (m_fLR_MovingFactor < 0.0f)
		{
			m_fLR_MovingFactor += fStepPerUpd;
			clamp(m_fLR_MovingFactor, -1.0f, 0.0f);
		}
		else
		{
			m_fLR_MovingFactor -= fStepPerUpd;
			clamp(m_fLR_MovingFactor, 0.0f, 1.0f);
		}
	}

	clamp(m_fLR_MovingFactor, -1.0f, 1.0f); // Фактор боковой ходьбы не должен превышать эти лимиты

	// Производим наклон ствола для нормального режима и аима
	for (int _idx = 0; _idx <= 1; _idx++)
	{
		bool bEnabled = m_strafe_offset[2][_idx].x;
		if (!bEnabled)
			continue;

		Fvector curr_offs, curr_rot;

		// Смещение позиции худа в стрейфе
		curr_offs = m_strafe_offset[0][_idx]; //pos
		curr_offs.mul(m_fLR_MovingFactor);                   // Умножаем на фактор стрейфа

		// Поворот худа в стрейфе
		curr_rot = m_strafe_offset[1][_idx]; //rot
		curr_rot.mul(-PI / 180.f);                          // Преобразуем углы в радианы
		curr_rot.mul(m_fLR_MovingFactor);                   // Умножаем на фактор стрейфа

		if (_idx == 0)
		{ // От бедра
			curr_offs.mul(1.f - m_fZoomRotationFactor);
			curr_rot.mul(1.f - m_fZoomRotationFactor);
		}
		else
		{ // Во время аима
			curr_offs.mul(m_fZoomRotationFactor);
			curr_rot.mul(m_fZoomRotationFactor);
		}

		Fmatrix hud_rotation;
		Fmatrix hud_rotation_y;

		hud_rotation.identity();
		hud_rotation.rotateX(curr_rot.x);

		hud_rotation_y.identity();
		hud_rotation_y.rotateY(curr_rot.y);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation_y.identity();
		hud_rotation_y.rotateZ(curr_rot.z);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation.translate_over(curr_offs);
		trans.mulB_43(hud_rotation);
	}
}

void	CWeapon::SetAmmoElapsed	(int ammo_count)
{
	iAmmoElapsed				= ammo_count;

	u32 uAmmo					= u32(iAmmoElapsed);

	if (uAmmo != m_magazine.size())
	{
		if (uAmmo > m_magazine.size())
		{
			CCartridge			l_cartridge; 
			l_cartridge.Load	(*m_ammoTypes[m_ammoType], u8(m_ammoType));
			while (uAmmo > m_magazine.size())
				m_magazine.push_back(l_cartridge);
		}
		else
		{
			while (uAmmo < m_magazine.size())
				m_magazine.pop_back();
		};
	};
}

u32	CWeapon::ef_main_weapon_type	() const
{
	VERIFY	(m_ef_main_weapon_type != u32(-1));
	return	(m_ef_main_weapon_type);
}

u32	CWeapon::ef_weapon_type	() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

bool CWeapon::IsNecessaryItem	    (const shared_str& item_sect)
{
	return (std::find(m_ammoTypes.begin(), m_ammoTypes.end(), item_sect) != m_ammoTypes.end() );
}

void CWeapon::modify_holder_params		(float &range, float &fov) const
{
	if (!IsScopeAttached()) {
		inherited::modify_holder_params	(range,fov);
		return;
	}
	range	*= m_addon_holder_range_modifier;
	fov		*= m_addon_holder_fov_modifier;
}

void CWeapon::OnDrawUI()
{
	if(IsZoomed() && ZoomHideCrosshair()){
		if(ZoomTexture() && !IsRotatingToZoom()){
			ZoomTexture()->SetPos	(0,0);
			ZoomTexture()->SetRect	(0,0,UI_BASE_WIDTH, UI_BASE_HEIGHT);
			ZoomTexture()->Render	();

//			m_UILens.Draw();
		}
	}
}

bool CWeapon::unlimited_ammo() 
{ 
	if (m_pCurrentInventory)
		return inventory_owner().unlimited_ammo() && m_DefaultCartridge.m_flags.test(CCartridge::cfCanBeUnlimited);
	else
		return false;
};

LPCSTR	CWeapon::GetCurrentAmmo_ShortName	()
{
	if (m_magazine.empty()) return ("");
	CCartridge &l_cartridge = m_magazine.back();
	return *(l_cartridge.m_InvShortName);
}

float CWeapon::GetMagazineWeight(const decltype(CWeapon::m_magazine)& mag) const
{
    float res = 0;
    const char* last_type = nullptr;
    float last_ammo_weight = 0;
    for (auto& c : mag)
    {
        // Usually ammos in mag have same type, use this fact to improve performance
        if (last_type != c.m_ammoSect.c_str())
        {
            last_type = c.m_ammoSect.c_str();
            last_ammo_weight = c.Weight();
        }
        res += last_ammo_weight;
    }
    return res;
}

float CWeapon::Weight() const
{
	float res = CInventoryItemObject::Weight();
	if ( GrenadeLauncherAttachable() && IsGrenadeLauncherAttached() )
		res += pSettings->r_float(GetGrenadeLauncherName(),"inv_weight");
	if ( ScopeAttachable() && IsScopeAttached() )
		res += pSettings->r_float(GetScopeName(),"inv_weight");
	if ( SilencerAttachable() && IsSilencerAttached() )
		res += pSettings->r_float(GetSilencerName(),"inv_weight");
	res += GetMagazineWeight(m_magazine);

	return res;
}

u32 CWeapon::Cost() const
{
	u32 res = m_cost;
	
	if (Core.Features.test(xrCore::Feature::wpn_cost_include_addons)) {
		if (GrenadeLauncherAttachable() && IsGrenadeLauncherAttached())
			res += pSettings->r_u32(GetGrenadeLauncherName(), "cost");
		if (ScopeAttachable() && IsScopeAttached())
			res += pSettings->r_u32(GetScopeName(), "cost");
		if (SilencerAttachable() && IsSilencerAttached())
			res += pSettings->r_u32(GetSilencerName(), "cost");
	}
	return res;
}

void CWeapon::Hide(bool now)
{
	if (now)
	{
		OnStateSwitch(eHidden, GetState());
		SetState(eHidden);
		StopHUDSounds();
	}
	else
		SwitchState(eHiding);

	OnZoomOut();
}

void CWeapon::Show(bool now)
{
	if (now)
	{
		StopCurrentAnimWithoutCallback();
		OnStateSwitch(eIdle, GetState());
		SetState(eIdle);
		StopHUDSounds();
	}
	else
		SwitchState(eShowing);
}

bool CWeapon::show_crosshair()
{
	return ! ( IsZoomed() && ZoomHideCrosshair() );
}

bool CWeapon::show_indicators()
{
	return !(IsZoomed() && (ZoomTexture() || !m_bScopeShowIndicators));
}

float CWeapon::GetConditionToShow	() const
{
	return	(GetCondition());//powf(GetCondition(),4.0f));
}

BOOL CWeapon::ParentMayHaveAimBullet	()
{
	CObject* O=H_Parent();
	if (!O) return FALSE;
	CEntityAlive* EA=smart_cast<CEntityAlive*>(O);
	return EA->cast_actor()!=0;
}

BOOL CWeapon::ParentIsActor	()
{
	CObject* O=H_Parent();
	if (!O) return FALSE;
	CEntityAlive* EA=smart_cast<CEntityAlive*>(O);
	if (!EA) return FALSE;
	return EA->cast_actor()!=0;
}

const float &CWeapon::hit_probability	() const
{
	VERIFY					((g_SingleGameDifficulty >= egdNovice) && (g_SingleGameDifficulty <= egdMaster)); 
	return					(m_hit_probability[egdNovice]);
}

// Function for callbacks added by Cribbledirge.
void CWeapon::StateSwitchCallback(GameObject::ECallbackType actor_type, GameObject::ECallbackType npc_type)
{
	if (g_actor)
	{
		if (smart_cast<CActor*>(H_Parent()))  // This is an actor.
		{
			Actor()->callback(actor_type)(
				lua_game_object()  // The weapon as a game object.
			);
		}
		else if (smart_cast<CEntityAlive*>(H_Parent()))  // This is an NPC.
		{
			Actor()->callback(npc_type)(
				smart_cast<CEntityAlive*>(H_Parent())->lua_game_object(),       // The owner of the weapon.
				lua_game_object()                                              // The weapon itself.
			);
		}
	}
}

// Обновление необходимости включения второго вьюпорта +SecondVP+
// Вызывается только для активного оружия игрока
void CWeapon::UpdateSecondVP()
{
	// + CActor::UpdateCL();
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if (!pActor)
		return;

	CInventoryOwner* inv_owner = pActor->cast_inventory_owner();

	bool b_is_active_item = inv_owner && (inv_owner->m_inventory->ActiveItem() == this);
	R_ASSERT(b_is_active_item); // Эта функция должна вызываться только для оружия в руках нашего игрока

	bool bCond_1 = m_fZoomRotationFactor > 0.05f;    // Мы должны целиться
	bool bCond_3 = pActor->cam_Active() == pActor->cam_FirstEye(); // Мы должны быть от 1-го лица

	Device.m_SecondViewport.SetSVPActive(bCond_1 && bCond_3 && SecondVPEnabled());
}

bool CWeapon::SecondVPEnabled() const
{	
	bool bCond_2 = m_fSecondVPZoomFactor > 0.0f;     // В конфиге должен быть прописан фактор зума (scope_lense_fov_factor) больше чем 0
	bool bCond_4 = !IsGrenadeMode();     // Мы не должны быть в режиме подствольника
	bool bCond_5 = !is_second_zoom_offset_enabled; // Мы не должны быть в режиме второго прицеливания.
	bool bcond_6 = psActorFlags.test(AF_3D_SCOPES);

	return bCond_2 && bCond_4 && bCond_5 && bcond_6;
}

// Чувствительность мышкии с оружием в руках во время прицеливания
float CWeapon::GetControlInertionFactor() const
{
	float fInertionFactor = inherited::GetControlInertionFactor();

	if (IsZoomed() && SecondVPEnabled() && !IsRotatingToZoom())
	{
		if (m_bScopeDynamicZoom)
		{
			const float delta_factor_total = 1 - m_fSecondVPZoomFactor;
			float min_zoom_factor = 1 + delta_factor_total * m_fMinZoomK;
			float k = (m_fRTZoomFactor - min_zoom_factor) / (m_fSecondVPZoomFactor - min_zoom_factor);
			return (m_fScopeInertionFactor - fInertionFactor) * k + fInertionFactor;
		}
		else
			return m_fScopeInertionFactor;
	}

	return fInertionFactor;
}

float CWeapon::GetSecondVPFov() const
{
	float fov_factor = m_fSecondVPZoomFactor;
	if (m_bScopeDynamicZoom)
	{
		fov_factor = m_fRTZoomFactor;
	}
	return atanf(tanf(g_fov * (0.5f * PI / 180)) / fov_factor) / (0.5f * PI / 180);
}

bool CWeapon::IsGrenadeMode() const
{
	const auto wpn_w_gl = smart_cast<const CWeaponMagazinedWGrenade*>(this);
	return wpn_w_gl && wpn_w_gl->m_bGrenadeMode;
}

// Получить HUD FOV от текущего оружия игрока
float CWeapon::GetHudFov()
{
	// Рассчитываем HUD FOV от бедра (с учётом упирания в стены)
	if (m_nearwall_on && ParentIsActor() && Level().CurrentViewEntity() == H_Parent())
	{
		// Получаем расстояние от камеры до точки в прицеле
		collide::rq_result& RQ = HUD().GetCurrentRayQuery();
		float dist = RQ.range;

		// Интерполируем расстояние в диапазон от 0 (min) до 1 (max)
		clamp(dist, m_nearwall_dist_min, m_nearwall_dist_max);
		float fDistanceMod = ((dist - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min)); // 0.f ... 1.f

		 // Рассчитываем базовый HUD FOV от бедра
		float fBaseFov = psHUD_FOV_def;
		clamp(fBaseFov, 0.0f, FLT_MAX);

		// Плавно высчитываем итоговый FOV от бедра
		float src = m_nearwall_speed_mod * Device.fTimeDelta;
		clamp(src, 0.f, 1.f);

		float fTrgFov = m_nearwall_target_hud_fov + fDistanceMod * (fBaseFov - m_nearwall_target_hud_fov);
		m_nearwall_last_hud_fov = m_nearwall_last_hud_fov * (1 - src) + fTrgFov * src;
	}

	if (m_fZoomRotationFactor > 0.0f)
	{
		if (SecondVPEnabled() && m_fSecondVPHudFov > 0.0f)
		{
			// В линзе зума
			float fDiff = m_nearwall_last_hud_fov - m_fSecondVPHudFov;
			return m_fSecondVPHudFov + (fDiff * (1 - m_fZoomRotationFactor));
		}
		if ((m_eScopeStatus == CSE_ALifeItemWeapon::eAddonDisabled || IsScopeAttached()) && !IsGrenadeMode() && m_fZoomHudFov > 0.0f)
		{
			// В процессе зума
			float fDiff = m_nearwall_last_hud_fov - m_fZoomHudFov;
			return m_fZoomHudFov + (fDiff * (1 - m_fZoomRotationFactor));
		}
	}

	// От бедра	 
	return m_nearwall_last_hud_fov;
}


void CWeapon::OnBulletHit() {
  if ( !fis_zero( conditionDecreasePerShotOnHit ) )
    ChangeCondition( -conditionDecreasePerShotOnHit );
}


bool CWeapon::IsPartlyReloading() {
  return ( m_set_next_ammoType_on_reload == u32(-1) && GetAmmoElapsed() > 0 && !IsMisfire() );
}
