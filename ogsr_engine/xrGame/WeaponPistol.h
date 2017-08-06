#pragma once
#include "weaponcustompistol.h"

class CWeaponPistol :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponPistol	(LPCSTR name);
	virtual ~CWeaponPistol(void);

	virtual void	Load			(LPCSTR section);
	
	virtual void	switch2_Reload	();

	virtual void	OnShot			();
	virtual void	OnAnimationEnd	(u32 state);
	virtual void	net_Destroy		();
	virtual void	OnH_B_Chield	();

	virtual void	OnDrawUI();
	virtual void	net_Relcase(CObject *object);

	//анимации
	virtual void	PlayAnimShow	();
	virtual void	PlayAnimIdle	();
	virtual void	PlayAnimHide	();
	virtual void	PlayAnimReload	();
	virtual void	PlayAnimShoot	();

	virtual void	UpdateSounds	();
protected:	
	virtual bool	AllowFireWhileWorking() {return true;}

	HUD_SOUND			sndClose;
	ESoundTypes			m_eSoundClose;
	struct WWPMotions{
		MotionSVec		mhud_show_empty;
		MotionSVec		mhud_empty;
		MotionSVec		mhud_shot_l;
		MotionSVec		mhud_close;
		MotionSVec		mhud_reload_empty;
	};
	WWPMotions			mhud_pistol,mhud_pistol_r;
	SWMmotions			wm_mhud_r;
	
	WWPMotions&			wwpm_current	();
	SWMmotions&			swm_current	();

	bool m_opened;
};
