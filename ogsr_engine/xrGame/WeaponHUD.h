#pragma once

#include "../Include/xrRender/KinematicsAnimated.h"
class CHudItem;

#define BOBBING_SECT "wpn_bobbing_effector"

#define CROUCH_FACTOR	0.75f
#define SPEED_REMINDER	5.f 

class CWeaponBobbing
{
	//родительский объект HUD
	CHudItem*			m_pParentWeapon;		
	public:
		CWeaponBobbing(CHudItem* pHudItem);
		virtual ~CWeaponBobbing();
		void Load();
		void Update(Fmatrix &m);
		void CheckState();

	private:
		float	fTime;
		Fvector	vAngleAmplitude;
		float	fYAmplitude;
		float	fSpeed;

		u32		dwMState;
		float	fReminderFactor;
		bool	is_limping;
		bool	m_bZoomMode;

		float	m_fAmplitudeRun;
		float	m_fAmplitudeWalk;
		float	m_fAmplitudeLimp;

		float	m_fSpeedRun;
		float	m_fSpeedWalk;
		float	m_fSpeedLimp;

		float m_fCrouchFactor;
		float m_fZoomFactor;
		float m_fScopeZoomFactor;
};


struct weapon_hud_value: public shared_value
{
	IKinematicsAnimated*	m_animations;
public:
	int					m_fire_bone;
	Fvector				m_fp_offset;
	Fvector				m_fp2_offset;
	Fvector				m_sp_offset;

	Fmatrix				m_offset;
	bool				m_bAllowBobbing;
public:
	virtual				~weapon_hud_value		();
	BOOL				load					(const shared_str& section, CHudItem* owner);
};

typedef shared_container<weapon_hud_value>		weapon_hud_container;
extern weapon_hud_container*					g_pWeaponHUDContainer;


class MotionIDEx {
public:
  MotionID m_MotionID;
  float    stop_k;

  MotionIDEx();
  MotionIDEx( MotionID );
};


class shared_weapon_hud: public shared_item<weapon_hud_value>
{
protected:
	struct	on_new_pred{
		CHudItem*		owner;
						on_new_pred				(CHudItem* _owner):owner(_owner){}
		BOOL			operator()				(const shared_str& key, weapon_hud_value* val) const {return val->load(key,owner);}
	};
public:
	void				create					(shared_str key, CHudItem* owner)
	{	
		shared_item<weapon_hud_value>::create	(key,g_pWeaponHUDContainer,on_new_pred(owner));	
	}
	IKinematicsAnimated*	animations				(){return p_->m_animations;}
	u32 motion_length ( MotionIDEx& M );
	MotionID			motion_id				(LPCSTR name);
};
//---------------------------------------------------------------------------

class CWeaponHUD
{
	friend class CWeaponScript;
	//родительский объект HUD
	CHudItem*			m_pParentWeapon;		
	//флаг, если hud спрятан не показывается
	bool				m_bHidden;
	bool				m_bVisible;

	Fmatrix				m_Transform;

	//shared HUD data
	shared_weapon_hud	m_shared_data;

	//таймеры для проигрывания анимаций
	u32					m_dwAnimTime{};
	u32					m_dwAnimEndTime{};
	bool				m_bStopAtEndAnimIsRunning;
	u32					m_startedAnimState{};
//	CInventoryItem*		m_pCallbackItem;
	CHudItem*			m_pCallbackItem;

	//остановление таймера текущей анимации, и вызов callback
	void				StopCurrentAnim	();

	//поворот и смещение для режима приближения
	float				m_fZoomRotateX{};
	float				m_fZoomRotateY{};
	Fvector				m_fZoomOffset;
public: 
						CWeaponHUD		(CHudItem* pHudItem);
						~CWeaponHUD		();

	// misc
	void				Load			(LPCSTR section);
	void				net_DestroyHud	();
	void				Init			();

	IC IRenderVisual*	Visual			()	{ return m_shared_data.animations()->dcast_RenderVisual();			}
	IC Fmatrix&			Transform		()	{ return m_Transform;							}

	int					FireBone		()	{return m_shared_data.get_value()->m_fire_bone;	}
	const Fvector&		FirePoint		()	{return m_shared_data.get_value()->m_fp_offset;	}
	const Fvector&		FirePoint2		()	{return m_shared_data.get_value()->m_fp2_offset;}
	const Fvector&		ShellPoint		()	{return m_shared_data.get_value()->m_sp_offset;	}

	const Fvector&		ZoomOffset		()	const {return m_fZoomOffset;}
	float				ZoomRotateX		()	const {return m_fZoomRotateX;}
	float				ZoomRotateY		()	const {return m_fZoomRotateY;}
	void				SetZoomOffset	(const Fvector& zoom_offset)  { m_fZoomOffset = zoom_offset;}
	void				SetZoomRotateX	(float zoom_rotate_x)		  { m_fZoomRotateX = zoom_rotate_x;}
	void				SetZoomRotateY	(float zoom_rotate_y)		  { m_fZoomRotateY = zoom_rotate_y;}
	

	// Animations
	void				animPlay( MotionIDEx& M, BOOL bMixIn/*=TRUE*/, CHudItem*  W /*=0*/, u32 state );
	void				animDisplay		(MotionID M, BOOL bMixIn);
	void				animDisplay( MotionIDEx M, BOOL bMixIn );
	MotionID			animGet			(LPCSTR name);
	
	void				UpdatePosition	(const Fmatrix& transform);

	bool				IsHidden		() {return m_bHidden;}
	void				Hide			() {m_bHidden = true;}
	void				Show			() {m_bHidden = false;}

	void				Visible			(bool val){m_bVisible=val;}
	
	//обновление HUD должно вызываться на каждом кадре
	void				Update			();

	void				StopCurrentAnimWithoutCallback	();

public:
	static void			CreateSharedContainer	();
	static void			DestroySharedContainer	();
	static void			CleanSharedContainer	();

public:
	void				dbg_SetFirePoint	(const Fvector &fp)			{((weapon_hud_value*)m_shared_data.get_value())->m_fp_offset.set(fp);}
	void				dbg_SetFirePoint2	(const Fvector &fp)			{((weapon_hud_value*)m_shared_data.get_value())->m_fp2_offset.set(fp);}
	void				dbg_SetShellPoint	(const Fvector &sp)			{((weapon_hud_value*)m_shared_data.get_value())->m_sp_offset.set(sp);}


private:
	CWeaponBobbing *m_bobbing;
};


#define		MAX_ANIM_COUNT							8
typedef		svector<MotionIDEx,MAX_ANIM_COUNT>		MotionSVec;
MotionIDEx& random_anim( MotionSVec& v ); 
