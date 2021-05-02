#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponShotgun :	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponShotgun(void);
	virtual ~CWeaponShotgun(void);

	virtual void	Load			(LPCSTR section);
	
	virtual void	net_Destroy			();
	virtual void	net_Export			(NET_Packet& P);
	virtual void	net_Import			(NET_Packet& P);

	virtual void	Reload				();
	void TryReload();
	virtual void	Fire2Start			();
	virtual void	Fire2End			();
	virtual void	OnShot				();
	virtual void	OnShotBoth			();
	virtual void	switch2_Fire		();
	virtual void	switch2_Fire2		();
	void			switch2_StartReload ();
	void			switch2_AddCartgidge();
	void			switch2_EndReload	();
	virtual void StopHUDSounds();

	virtual void	UpdateSounds		();
	virtual void	PlayAnimOpenWeapon	();
	virtual void	PlayAnimAddOneCartridgeWeapon();
	void			PlayAnimCloseWeapon	();

	virtual	void	UpdateCL();

	virtual bool	Action(s32 cmd, u32 flags);

#ifdef DUPLET_STATE_SWITCH
	bool is_duplet_enabled = false;
	void SwitchDuplet();
#endif // !DUPLET_STATE_SWITCH

protected:
	virtual void	OnAnimationEnd		(u32 state);
	void			TriStateReload		();
	virtual void	OnStateSwitch		(u32 S, u32 oldState);

	bool			HaveCartridgeInInventory(u8 cnt);
	virtual u8		AddCartridge		(u8 cnt);
	virtual void	ReloadMagazine	();

	HUD_SOUND		sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;

	ESoundTypes		m_eSoundOpen{};
	ESoundTypes		m_eSoundAddCartridge;
	ESoundTypes		m_eSoundClose;
	HUD_SOUND		m_sndOpen;
	HUD_SOUND		m_sndAddCartridge;
	HUD_SOUND		m_sndClose;

	bool			m_stop_triStateReload;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponShotgun)
#undef script_type_list
#define script_type_list save_type_list(CWeaponShotgun)
