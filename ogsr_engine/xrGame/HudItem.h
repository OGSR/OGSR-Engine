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

#include "actor_defs.h"
#include "weaponHUD.h"

class CHudItem {
protected: //чтоб нельзя было вызвать на прямую
	CHudItem(void);
	virtual ~CHudItem(void);
	virtual DLL_Pure*_construct			();
private:
	u32				m_state;
	u32				m_nextState;
public:
	virtual void	Load				(LPCSTR section);
	virtual CHudItem*cast_hud_item		()	 { return this; }


	virtual void	PlaySound			(HUD_SOUND& snd, const Fvector& position);
										
	///////////////////////////////////////////////
	// общие функции HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	virtual void	StopHUDSounds		()				{};
	
	//для предачи команд владельцем
	virtual bool	Action				(s32 cmd, u32 flags);
	virtual void	onMovementChanged	(ACTOR_DEFS::EMoveCommand cmd)				{};

	virtual void	OnDrawUI			()				{};
	
	IC		u32		GetNextState		() const			{return		m_nextState;}
	IC		u32		GetState			() const			{return		m_state;}

	IC		void	SetState			(u32 v)				{m_state = v;}
	IC		void	SetNextState		(u32 v)				{m_nextState = v;}
	//посылка сообщения на сервер о смене состояния оружия 
	virtual void	SwitchState			(u32 S);
	//прием сообщения с сервера и его обработка
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	(bool just_before_destroy);
	virtual void	OnH_A_Independent	();
	
	virtual	BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();

	virtual void	StartIdleAnim		() {};

	
	virtual bool	Activate			();
	virtual void	Deactivate			();
	
	virtual void	OnActiveItem		() {};
	virtual void	OnHiddenItem		() {};

	virtual void	OnAnimationEnd		(u32 state)				{};

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();
	
	//просчет инерции для HUD 
	virtual void	UpdateHudInertion		(Fmatrix& hud_trans);
	//просчет дополнительных вычислений (переопределяется в потомках)
	virtual void	UpdateHudAdditonal		(Fmatrix&);


	virtual	void	UpdateXForm			() = 0;
	void					animGet		(MotionSVec& lst, LPCSTR prefix);

	CWeaponHUD*		GetHUD				() {return m_pHUD;}

protected:
	//TRUE - оружие занято, выполнением некоторого действия
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	shared_str				hud_sect;
	bool					m_bRenderHud;

	//время нахождения в текущем состоянии
	u32						m_dwStateTime;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

	//вкл/выкл инерции (временное, с плавным возвращением оружия в состояние без инерции)
	void					StartHudInertion();
	void					StopHudInertion();
private:
	bool					m_bInertionEnable;
	bool					m_bInertionAllow;
protected:
	u32						m_animation_slot;
public:
	IC		u32				animation_slot			()	{	return m_animation_slot;}

private:
	CPhysicItem				*m_object;
	CInventoryItem			*m_item;

public:
	IC CPhysicItem&			object					() const {	VERIFY(m_object); return(*m_object);}
	IC CInventoryItem&		item					() const {	VERIFY(m_item);	return(*m_item);}

	virtual void			on_renderable_Render	() = 0;
};

