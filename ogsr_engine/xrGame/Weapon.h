// Weapon.h: interface for the CWeapon class.
#pragma once

#include "WeaponAmmo.h"
#include "PhysicsShell.h"
#include "PHShellCreator.h"
#include "ShootingObject.h"
#include "hud_item_object.h"
#include "Actor_Flags.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "game_cl_single.h"
#include "xrServer_Objects_ALife.h"
#include "xrServer_Objects_ALife_Items.h"
#include "actor.h"
#include "firedeps.h"

// refs
class CEntity;
class ENGINE_API CMotionDef;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemWeaponAmmo;
class CWeaponMagazined;
class CParticlesObject;
class CUIStaticItem;

ENGINE_API extern float psHUD_FOV_def;

constexpr float def_min_zoom_k = 0.3f;
constexpr float def_zoom_step_count = 4.0f;

class CWeapon : public CHudItemObject,
				public CShootingObject
{
	friend class CWeaponScript;
private:
	typedef CHudItemObject inherited;

public:
							CWeapon				(LPCSTR name);
	virtual					~CWeapon			();

	// Generic
	virtual void			Load				(LPCSTR section);

	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);
	virtual void			net_Import			(NET_Packet& P);
	
	virtual CWeapon			*cast_weapon			()					{return this;}
	virtual CWeaponMagazined*cast_weapon_magazined	()					{return 0;}


	//serialization
	virtual void			save				(NET_Packet &output_packet);
	virtual void			load				(IReader &input_packet);
	virtual BOOL			net_SaveRelevant	()								{return inherited::net_SaveRelevant();}

	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32 dt);

	virtual void			renderable_Render	();
	virtual void			OnDrawUI			();
	virtual bool			need_renderable		();

	virtual void			OnH_B_Chield		();
	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	(bool just_before_destroy);
	virtual void			OnH_A_Independent	();
	virtual void			OnEvent				(NET_Packet& P, u16 type);// {inherited::OnEvent(P,type);}

	virtual	void			Hit					(SHit* pHDS);

	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();

	virtual void			SwitchState			(u32 S);
	virtual bool			Activate( bool = false );

	virtual void			Hide( bool = false );
	virtual void			Show( bool = false );

	//инициализация если вещь в активном слоте или спрятана на OnH_B_Chield
	virtual void			OnActiveItem		();
	virtual void			OnHiddenItem		();

	// Callback function added by Cribbledirge.
	virtual IC void	StateSwitchCallback(GameObject::ECallbackType actor_type, GameObject::ECallbackType npc_type);

//////////////////////////////////////////////////////////////////////////
//  Network
//////////////////////////////////////////////////////////////////////////

public:
	virtual bool			can_kill			() const;
	virtual CInventoryItem	*can_kill			(CInventory *inventory) const;
	virtual const CInventoryItem *can_kill		(const xr_vector<const CGameObject*> &items) const;
	virtual bool			ready_to_kill		() const;

//////////////////////////////////////////////////////////////////////////
//  Animation 
//////////////////////////////////////////////////////////////////////////
public:

	void					signal_HideComplete	();

//////////////////////////////////////////////////////////////////////////
//  InventoryItem methods
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool			Action(s32 cmd, u32 flags);

	//////////////////////////////////////////////////////////////////////////
//  Weapon state
//////////////////////////////////////////////////////////////////////////
public:
	enum EWeaponStates {
		eFire = eLastBaseState + 1,
		eFire2,
		eReload,
		eMisfire,
		eMagEmpty,
		eSwitch,
	};
	enum EWeaponSubStates{
		eSubstateReloadBegin		=0,
		eSubstateReloadInProcess,
		eSubstateReloadEnd,
		eSubstateIdleMoving,
		eSubstateIdleSprint,
	};

	virtual bool			IsHidden			()	const		{	return GetState() == eHidden;}						// Does weapon is in hidden state
	virtual bool			IsHiding			()	const		{	return GetState() == eHiding;}
	virtual bool			IsShowing			()	const		{	return GetState() == eShowing;}

	IC BOOL					IsValid				()	const		{	return iAmmoElapsed;						}
	// Does weapon need's update?
	BOOL					IsUpdating			();


	BOOL					IsMisfire			() const;
	BOOL					CheckForMisfire		();

	bool					IsTriStateReload	() const		{ return m_bTriStateReload;}
	EWeaponSubStates		GetReloadState		() const		{ return (EWeaponSubStates)m_sub_state;}
	u8 idle_state();
protected:
	bool					m_bTriStateReload;
	u8						m_sub_state;
	u8						m_idle_state;
	// Weapon fires now
	bool					bWorking2;
	// a misfire happens, you'll need to rearm weapon
	bool					bMisfire;				

//////////////////////////////////////////////////////////////////////////
//  Weapon Addons
//////////////////////////////////////////////////////////////////////////
public:
	///////////////////////////////////////////
	// работа с аддонами к оружию
	//////////////////////////////////////////


			bool IsGrenadeLauncherAttached	() const;
			bool IsScopeAttached			() const;
			bool IsSilencerAttached			() const;

	bool			IsGrenadeMode() const;

	virtual bool GrenadeLauncherAttachable() const;
	virtual bool ScopeAttachable() const;
	virtual bool SilencerAttachable() const;
	virtual bool UseScopeTexture();

	//обновление видимости для косточек аддонов
			void UpdateAddonsVisibility();
			void UpdateHUDAddonsVisibility();
	//инициализация свойств присоединенных аддонов
	virtual void InitAddons();

	//для отоброажения иконок апгрейдов в интерфейсе
	int	GetScopeX() {return m_iScopeX;}
	int	GetScopeY() {return m_iScopeY;}
	int	GetSilencerX() {return m_iSilencerX;}
	int	GetSilencerY() {return m_iSilencerY;}
	int	GetGrenadeLauncherX() {return m_iGrenadeLauncherX;}
	int	GetGrenadeLauncherY() {return m_iGrenadeLauncherY;}

	const shared_str& GetGrenadeLauncherName	() const		{return m_sGrenadeLauncherName;}
	const shared_str& GetScopeName				() const		{return m_sScopeName;}
	const shared_str& GetSilencerName			() const		{return m_sSilencerName;}

	u8		GetAddonsState						()		const		{return m_flagsAddOnState;};
	void	SetAddonsState						(u8 st)	{m_flagsAddOnState=st;}//dont use!!! for buy menu only!!!

                                                                               //названия секций подключаемых аддонов
    shared_str		m_sScopeName;
    std::vector<shared_str> m_allScopeNames;
    shared_str		m_sSilencerName;
    shared_str		m_sGrenadeLauncherName;

	shared_str m_sWpn_scope_bone;
	shared_str m_sWpn_silencer_bone;
	shared_str m_sWpn_launcher_bone;
	shared_str m_sHud_wpn_scope_bone;
	shared_str m_sHud_wpn_silencer_bone;
	shared_str m_sHud_wpn_launcher_bone;

private:
	std::vector<shared_str> hidden_bones;
	std::vector<shared_str> hud_hidden_bones;

protected:
	//состояние подключенных аддонов
	u8 m_flagsAddOnState;

	//возможность подключения различных аддонов
	ALife::EWeaponAddonStatus	m_eScopeStatus;
	ALife::EWeaponAddonStatus	m_eSilencerStatus;
	ALife::EWeaponAddonStatus	m_eGrenadeLauncherStatus;


	//смещение иконов апгрейдов в инвентаре
	int	m_iScopeX, m_iScopeY;
	int	m_iSilencerX, m_iSilencerY;
	int	m_iGrenadeLauncherX, m_iGrenadeLauncherY;
		
///////////////////////////////////////////////////
//	для режима приближения и снайперского прицела
///////////////////////////////////////////////////
protected:
	//разрешение регулирования приближения. Real Wolf.
	bool			m_bScopeDynamicZoom;
	//run-time zoom factor
	float			m_fRTZoomFactor;
	//разрешение режима приближения
	bool			m_bZoomEnabled;
	//текущий фактор приближения
	float			m_fZoomFactor;
	//время приближения
	float			m_fZoomRotateTime;
	//текстура для снайперского прицела, в режиме приближения
	CUIStaticItem*	m_UIScope;
	//коэффициент увеличения прицеливания
	float			m_fIronSightZoomFactor;
	//коэффициент увеличения прицела
	float			m_fScopeZoomFactor;
	//коэффициент увеличения второго прицела
	float			m_fSecondScopeZoomFactor;
	//когда режим приближения включен
	bool			m_bZoomMode;
	//от 0 до 1, показывает насколько процентов
	//мы перемещаем HUD  
	float			m_fZoomRotationFactor;
	//коэффициент увеличения во втором вьюпорте при зуме
	float			m_fSecondVPZoomFactor;
	//прятать перекрестие в режиме прицеливания
	bool			m_bHideCrosshairInZoom;
	//разрешить инерцию оружия в режиме прицеливания
	bool			m_bZoomInertionAllow;
	// или в режиме прицеливания через оптику
	bool			m_bScopeZoomInertionAllow;
	//Целевой HUD FOV при зуме
	float			m_fZoomHudFov;
	//Целевой HUD FOV для линзы
	float			m_fSecondVPHudFov;

	bool m_bUseScopeZoom			= false;
	bool m_bUseScopeGrenadeZoom		= false;
	bool m_bUseScopeDOF = true;
	bool m_bForceScopeDOF = false;
	bool m_bScopeShowIndicators = true;
	bool m_bIgnoreScopeTexture = false;

	float m_fMinZoomK			= def_min_zoom_k;
	float m_fZoomStepCount		= def_zoom_step_count;

	float			m_fScopeInertionFactor;
public:

	IC bool					IsZoomEnabled		()	const	{return m_bZoomEnabled;}
	void					GetZoomData			(float scope_factor, float& delta, float& min_zoom_factor);
	virtual	void			ZoomChange			(bool inc);
	virtual void			OnZoomIn			();
	virtual void			OnZoomOut			();
			bool			IsZoomed			()	const	{return m_bZoomMode;};
	CUIStaticItem*			ZoomTexture			();	
	bool ZoomHideCrosshair()
	{
		auto* pA = smart_cast<CActor*>(H_Parent());
		if (pA && pA->active_cam() == eacLookAt)
			return false;

		return (m_bHideCrosshairInZoom || ZoomTexture()) && !psActorFlags.test(AF_CROSSHAIR_DBG);
	}

	virtual void			OnZoomChanged		() {}

	IC float				GetZoomFactor		() const		{	return m_fZoomFactor;	}
	virtual	float			CurrentZoomFactor	();
	//показывает, что оружие находится в соостоянии поворота для приближенного прицеливания
			bool			IsRotatingToZoom	() const		{	return (m_fZoomRotationFactor<1.f);}

			void			LoadZoomOffset		(LPCSTR section, LPCSTR prefix);

	virtual float			Weight				() const;		
	virtual u32				Cost				() const;
	virtual float			GetControlInertionFactor() const;

public:
    virtual EHandDependence		HandDependence		()	const		{	return eHandDependence;}
			bool				IsSingleHanded		()	const		{	return m_bIsSingleHanded; }

public:
	IC		LPCSTR			strap_bone0			() const {return m_strap_bone0;}
	IC		LPCSTR			strap_bone1			() const {return m_strap_bone1;}
	IC		void			strapped_mode		(bool value) {m_strapped_mode = value;}
	IC		bool			strapped_mode		() const {return m_strapped_mode;}

protected:
	LPCSTR					m_strap_bone0;
	LPCSTR					m_strap_bone1;
	Fmatrix					m_StrapOffset;
	bool					m_strapped_mode;
	bool					m_can_be_strapped;

	Fmatrix					m_Offset;
	// 0-используется без участия рук, 1-одна рука, 2-две руки
	EHandDependence			eHandDependence;
	bool					m_bIsSingleHanded;

public:
	//загружаемые параметры
	Fvector					vLoadedFirePoint;
	Fvector					vLoadedFirePoint2;
private:
	firedeps				m_current_firedeps{};

protected:
	virtual void			UpdateFireDependencies_internal	();
	virtual void			UpdatePosition			(const Fmatrix& transform);	//.
	virtual void			UpdateXForm				();

	float					m_fLR_MovingFactor; // !!!!
	Fvector					m_strafe_offset[3][2]; //pos,rot,data/ normal,aim-GL --#SM+#--

	virtual	u8				GetCurrentHudOffsetIdx	();
	virtual bool			MovingAnimAllowedNow	();
	virtual void			UpdateHudAdditonal		(Fmatrix&);

	IC		void			UpdateFireDependencies	()			{ if (dwFP_Frame==Device.dwFrame) return; UpdateFireDependencies_internal(); };

	virtual void			LoadFireParams		(LPCSTR section, LPCSTR prefix);
public:	
	IC		const Fvector&	get_LastFP				()			{ UpdateFireDependencies(); return m_current_firedeps.vLastFP;	}
	IC		const Fvector&	get_LastFP2				()			{ UpdateFireDependencies(); return m_current_firedeps.vLastFP2;	}
	IC		const Fvector&	get_LastFD				()			{ UpdateFireDependencies(); return m_current_firedeps.vLastFD;	}
	IC		const Fvector&	get_LastSP				()			{ UpdateFireDependencies(); return m_current_firedeps.vLastSP;	}

	virtual const Fvector&	get_CurrentFirePoint	()			{ return get_LastFP();				}
	virtual const Fvector&	get_CurrentFirePoint2	()			{ return get_LastFP2();				}
	virtual const Fmatrix&	get_ParticlesXFORM		()			{ UpdateFireDependencies(); return m_current_firedeps.m_FireParticlesXForm;	}
	virtual void			ForceUpdateFireParticles();

	//////////////////////////////////////////////////////////////////////////
	// Weapon fire
	//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			SetDefaults			();

	//трассирование полета пули
	virtual void			FireTrace			(const Fvector& P, const Fvector& D);
	virtual float			GetWeaponDeterioration	();

	virtual void			FireStart			() {CShootingObject::FireStart();}
	virtual void			FireEnd				();// {CShootingObject::FireEnd();}

	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual void			Reload				();
			void			StopShooting		();
    

	// обработка визуализации выстрела
	virtual void			OnShot				(){};
	virtual void			AddShotEffector		();
	virtual void			RemoveShotEffector	();
	virtual	void			ClearShotEffector	();

public:
	//текущая дисперсия (в радианах) оружия с учетом используемого патрона
	float					GetFireDispersion	(bool with_cartridge)			;
	float					GetFireDispersion	(float cartridge_k)				;
//	const Fvector&			GetRecoilDeltaAngle	();
	virtual	int				ShotsFired			() { return 0; }

	//параметы оружия в зависимоти от его состояния исправности
	float					GetConditionDispersionFactor	() const;
	float					GetConditionMisfireProbability	() const;
	virtual	float			GetConditionToShow				() const;

public:
	//отдача при стрельбе 
	float					camMaxAngle;
	float					camRelaxSpeed;
	float					camRelaxSpeed_AI;
	float					camDispersion;
	float					camDispersionInc;
	float					camDispertionFrac;
	float					camMaxAngleHorz;
	float					camStepAngleHorz;

protected:
	//фактор увеличения дисперсии при максимальной изношености 
	//(на сколько процентов увеличится дисперсия)
	float					fireDispersionConditionFactor;
	//вероятность осечки при максимальной изношености
	float					misfireProbability;
	float					misfireConditionK;
	//увеличение изношености при выстреле
	float					conditionDecreasePerShot;
	float					conditionDecreasePerShotOnHit;
	float					conditionDecreasePerShotSilencer;

	//  [8/2/2005]
	float					m_fPDM_disp_base			;
	float					m_fPDM_disp_vel_factor		;
	float					m_fPDM_disp_accel_factor	;
	float					m_fPDM_disp_crouch			;
	float					m_fPDM_disp_crouch_no_acc	;
	//  [8/2/2005]

protected:
	//для отдачи оружия
	Fvector					m_vRecoilDeltaAngle;

	//для сталкеров, чтоб они знали эффективные границы использования 
	//оружия
	float					m_fMinRadius;
	float					m_fMaxRadius;

//////////////////////////////////////////////////////////////////////////
// партиклы
//////////////////////////////////////////////////////////////////////////

protected:	
	//для второго ствола
			void			StartFlameParticles2();
			void			StopFlameParticles2	();
			void			UpdateFlameParticles2();
protected:
	shared_str					m_sFlameParticles2;
	//объект партиклов для стрельбы из 2-го ствола
	CParticlesObject*		m_pFlameParticles2;

//////////////////////////////////////////////////////////////////////////
// Weapon and ammo
//////////////////////////////////////////////////////////////////////////
protected:
	int GetAmmoCount_forType( shared_str const& ammo_type, u32 = 0 ) const;
	int GetAmmoCount( u8 ammo_type, u32 = 0 ) const;

public:
	IC int					GetAmmoElapsed		()	const		{	return /*int(m_magazine.size())*/iAmmoElapsed;}
	IC int					GetAmmoMagSize		()	const		{	return iMagazineSize;						}
	int						GetAmmoCurrent		(bool use_item_to_spawn = false)  const;
	IC void					SetAmmoMagSize		(int _size) { iMagazineSize = _size; }

	void					SetAmmoElapsed		(int ammo_count);

	virtual void			OnMagazineEmpty		();
			void			SpawnAmmo			(u32 boxCurr = 0xffffffff, 
													LPCSTR ammoSect = NULL, 
													u32 ParentID = 0xffffffff);

	//  [8/3/2005]
	virtual	float			Get_PDM_Base		()	const	{ return m_fPDM_disp_base			; };
	virtual	float			Get_PDM_Vel_F		()	const	{ return m_fPDM_disp_vel_factor		; };
	virtual	float			Get_PDM_Accel_F		()	const	{ return m_fPDM_disp_accel_factor	; };
	virtual	float			Get_PDM_Crouch		()	const	{ return m_fPDM_disp_crouch			; };
	virtual	float			Get_PDM_Crouch_NA	()	const	{ return m_fPDM_disp_crouch_no_acc	; };
	//  [8/3/2005]
protected:
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine

	//для подсчета в GetAmmoCurrent
	mutable int				iAmmoCurrent;
	mutable u32				m_dwAmmoCurrentCalcFrame;	//кадр на котором просчитали кол-во патронов

	virtual bool			IsNecessaryItem	    (const shared_str& item_sect);

public:
	xr_vector<shared_str>	m_ammoTypes;
	xr_vector<shared_str>	m_highlightAddons;

	CWeaponAmmo*			m_pAmmo;
	u32						m_ammoType;
	BOOL					m_bHasTracers;
	u8						m_u8TracerColorID;
	u32						m_set_next_ammoType_on_reload;
	// Multitype ammo support
	xr_vector<CCartridge>	m_magazine;
	CCartridge				m_DefaultCartridge;
	float					m_fCurrentCartirdgeDisp;

		bool				unlimited_ammo				();
	IC	bool				can_be_strapped				() const {return m_can_be_strapped;};

	LPCSTR					GetCurrentAmmo_ShortName	();
	float					GetMagazineWeight(const decltype(m_magazine)& mag) const;


protected:
	u32						m_ef_main_weapon_type;
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_main_weapon_type	() const;
	virtual u32				ef_weapon_type		() const;

protected:
	// This is because when scope is attached we can't ask scope for these params
	// therefore we should hold them by ourself :-((
	float					m_addon_holder_range_modifier;
	float					m_addon_holder_fov_modifier;

	float m_nearwall_last_hud_fov;

	float m_nearwall_target_hud_fov = 0.f;
	float m_nearwall_dist_max = 0.f;
	float m_nearwall_dist_min = 0.f;
	float m_nearwall_speed_mod = 0.f;

	bool m_nearwall_on = false;

public:
	virtual	void			modify_holder_params		(float &range, float &fov) const;
	virtual bool			use_crosshair				()	const {return true;}
			bool			show_crosshair				();
			bool			show_indicators				();
	virtual BOOL			ParentMayHaveAimBullet		();
	virtual BOOL			ParentIsActor				();

private:
	float					m_hit_probability[egdCount];

public:
	const float				&hit_probability			() const;
	void					UpdateWeaponParams();
	//
	bool is_second_zoom_offset_enabled = false;
	void UpdateZoomOffset();
	//
	void UpdateSecondVP(); //
	float GetZRotatingFactor() const { return m_fZoomRotationFactor; } //--#SM+#--
	float GetSecondVPFov() const; //--#SM+#--
	bool SecondVPEnabled() const;
	float GetHudFov();

	void SwitchScope();

	virtual void OnBulletHit();
	bool IsPartlyReloading();
};
