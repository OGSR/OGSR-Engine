#pragma once

#include "weaponShotgun.h"
#include "script_export_space.h"

class CWeaponBM16 :public CWeaponShotgun
{
	typedef CWeaponShotgun inherited;
protected:
	MotionSVec		mhud_reload1;
	MotionSVec		mhud_shot1;
	MotionSVec		mhud_shot2;
	MotionSVec		mhud_idle1;
	MotionSVec		mhud_idle2;
	MotionSVec		mhud_zoomed_idle1;
	MotionSVec		mhud_zoomed_idle2;
	MotionSVec		mhud_idle_sprint_1;
	MotionSVec		mhud_idle_sprint_2;
	MotionSVec		mhud_idle_moving_1;
	MotionSVec		mhud_idle_moving_2;

	MotionSVec		mhud_show1;
	MotionSVec		mhud_show2;

	MotionSVec		mhud_hide1;
	MotionSVec		mhud_hide2;

	HUD_SOUND		m_sndReload1;

public:
	virtual			~CWeaponBM16					();
	virtual void	Load							(LPCSTR section);

protected:
	virtual bool	TryPlayAnimIdle					( u8 );
	virtual void	PlayAnimShoot					();
	virtual void	PlayAnimReload					();
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayReloadSound					();
	virtual void UpdateSounds() override;
	virtual void	PlayAnimIdle					( u8 );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponBM16)
#undef script_type_list
#define script_type_list save_type_list(CWeaponBM16)
