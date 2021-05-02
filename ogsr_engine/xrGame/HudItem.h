//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок для всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

class CSE_Abstract;
class CPhysicItem;
class NET_Packet;
struct HUD_SOUND;
class CInventoryItem;
struct attachable_hud_item;
class motion_marks;
class CMotionDef;

#include "actor_defs.h"

class CHUDState
{
public:
	enum EHudStates
	{
		eIdle = 0,
		eShowing,
		eHiding,
		eHidden,
		eBore,
		eLastBaseState = eBore,
	};

private:
	u32 m_hud_item_state;
	u32 m_nextState;
	u32 m_dw_curr_state_time;

protected:
	u32 m_dw_curr_substate_time;

public:
	CHUDState() { SetState(eHidden); }
	IC u32 GetNextState() const { return m_nextState; }
	IC u32 GetState() const { return m_hud_item_state; }
	IC void SetState(u32 v)
	{
		m_hud_item_state = v;
		m_dw_curr_state_time = Device.dwTimeGlobal;
		ResetSubStateTime();
	}
	IC void SetNextState(u32 v) { m_nextState = v; }
	IC u32 CurrStateTime() const { return Device.dwTimeGlobal - m_dw_curr_state_time; }
	IC void ResetSubStateTime() { m_dw_curr_substate_time = Device.dwTimeGlobal; }
	virtual void SwitchState(u32 S) = 0;
	virtual void OnStateSwitch(u32 S, u32 oldState) = 0;
};

class CHudItem : public CHUDState
{
protected: //чтоб нельзя было вызвать на прямую
	CHudItem(void);
	virtual ~CHudItem(void);
	virtual DLL_Pure*_construct			();

	Flags16			m_huditem_flags;
	enum {
		fl_pending			= (1 << 0),
		fl_renderhud		= (1 << 1),
		fl_inertion_enable	= (1 << 2),
		fl_inertion_allow	= (1 << 3),
		fl_aiming			= (1 << 4),
		fl_collision_hud	= (1 << 5),
	};

	// Motion data
	const CMotionDef* m_current_motion_def;
	shared_str		m_current_motion;
	u32				m_dwMotionCurrTm;
	u32				m_dwMotionStartTm;
	u32				m_dwMotionEndTm;
	u32				m_startedMotionState;
	u8				m_started_rnd_anim_idx;
	bool			m_bStopAtEndAnimIsRunning;

	u32				m_dwStateTime;
public:
	virtual void	Load				(LPCSTR section);
	virtual CHudItem*cast_hud_item		()	 { return this; }


	virtual void	PlaySound( HUD_SOUND& snd, const Fvector& position, bool overlap = false );
										
	///////////////////////////////////////////////
	// общие функции HUD
	///////////////////////////////////////////////
	virtual void	StopHUDSounds		()				{};
	
	//для предачи команд владельцем
	virtual bool	Action				(s32 cmd, u32 flags);

	virtual void	OnDrawUI			()				{};
	
	virtual bool	IsHidden			() const { return GetState() == eHidden; } // Does weapon is in hidden state
	virtual bool	IsHiding			() const { return GetState() == eHiding; }
	virtual bool	IsShowing			() const { return GetState() == eShowing; }
	//посылка сообщения на сервер о смене состояния оружия 
	virtual void	SwitchState			(u32 S);
	//прием сообщения с сервера и его обработка
	virtual void	OnStateSwitch		(u32 S, u32 oldState);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	(bool just_before_destroy);
	virtual void	OnH_A_Independent	();
	
	virtual	BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();
	
	virtual bool	Activate( bool = false );
	virtual void	Deactivate( bool = false );
	
	virtual void	OnActiveItem		() {};
	virtual void	OnHiddenItem		() {};

	virtual void	OnAnimationEnd		(u32 state) {};
	virtual void	OnMotionMark		(u32 state, const motion_marks&) {};
	virtual void	OnMovementChanged	(ACTOR_DEFS::EMoveCommand cmd);

	virtual void	PlayAnimIdle		();
	bool			TryPlayAnimIdle		();
	virtual bool	MovingAnimAllowedNow() { return true; }
	virtual void	PlayAnimIdleMoving	();
	virtual void	PlayAnimIdleSprint	();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide( bool = false ) = 0;
	virtual void	Show( bool = false ) = 0;

	virtual void	UpdateHudAdditonal		(Fmatrix&) {};
	virtual	void	UpdateXForm				() = 0;

	u32				PlayHUDMotion			(const shared_str& M, BOOL bMixIn, CHudItem* W, u32 state, bool randomAnim = true);
	u32				PlayHUDMotion			(const shared_str& M, const shared_str& M2, BOOL bMixIn, CHudItem* W, u32 state, bool randomAnim = true);
	u32				PlayHUDMotion_noCB		(const shared_str& M, BOOL bMixIn, bool randomAnim = true);
	bool			AnimationExist			(const shared_str& M) const;
	void			StopCurrentAnimWithoutCallback();

	attachable_hud_item* HudItemData		() const;
	virtual void	on_a_hud_attach			();
	virtual void	on_b_hud_detach			();
	virtual u8		GetCurrentHudOffsetIdx	() { return 0; }
	const shared_str& HudSection			() const { return hud_sect; }

	BOOL			GetHUDmode				();
	IC void			SetPending				(BOOL H) { m_huditem_flags.set(fl_pending, H); }
	IC BOOL			IsPending				() const { return !!m_huditem_flags.test(fl_pending); }
	IC void			RenderHud				(BOOL B) { m_huditem_flags.set(fl_renderhud, B); }
	IC BOOL			RenderHud				() { return m_huditem_flags.test(fl_renderhud); }
	IC bool			HudInertionEnabled		() const { return m_huditem_flags.test(fl_inertion_enable); }
	IC bool			HudInertionAllowed		() const { return m_huditem_flags.test(fl_inertion_allow); }
	IC bool			HudCollisionEnabled		() const { return m_huditem_flags.test(fl_collision_hud); }
	IC void			EnableHudInertion		(BOOL B) { m_huditem_flags.set(fl_inertion_enable, B); }
	IC void			AllowHudInertion		(BOOL B) { m_huditem_flags.set(fl_inertion_allow, B); }
	IC void			EnableHudCollision		(BOOL B) { m_huditem_flags.set(fl_collision_hud, B); }
	virtual void	render_hud_mode			() {};
	virtual bool	need_renderable			() { return true; };
	virtual void	render_item_3d_ui		() {}
	virtual bool	render_item_3d_ui_query	() { return false; }
	virtual float	GetInertionFactor		() { return 1.f; }; //--#SM+#--
	virtual float	GetInertionPowerFactor	() { return 1.f; }; //--#SM+#--
	virtual bool	CheckCompatibility		(CHudItem*) { return true; }
protected:
	BOOL					hud_mode;
	shared_str				hud_sect;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

protected:
	u32						m_animation_slot;

	// отклонение модели от "курса" из за инерции во время движения
	float m_origin_offset;
	// скорость возврата худ модели в нужное положение
	float m_tendto_speed;

	// отклонение модели от "курса" из за инерции во время движения с прицеливанием
	float m_zoom_origin_offset;
	// скорость возврата худ модели в нужное положение во время прицеливания
	float m_zoom_tendto_speed;

public:
	IC		u32				animation_slot			()	{	return m_animation_slot;}

private:
	CPhysicItem*			m_object;
	CInventoryItem*			m_item;

public:
	IC CPhysicItem&			object					() const {	VERIFY(m_object); return(*m_object);}
	IC CInventoryItem&		item					() const {	VERIFY(m_item);	return(*m_item);}

	virtual void			on_renderable_Render	() = 0;
};

