#pragma once

#include "weaponShotgun.h"
#include "script_export_space.h"

class CWeaponBM16 :public CWeaponShotgun
{
	typedef CWeaponShotgun inherited;
protected:
	HUD_SOUND		m_sndReload1;

public:
	virtual			~CWeaponBM16					();
	virtual void	Load							(LPCSTR section);

protected:
	virtual void	PlayAnimShoot					();
	virtual void	PlayAnimReload					();
	virtual void	PlayAnimShow					();
	virtual void	PlayAnimHide					();
	virtual void	PlayAnimIdleMoving				();
	virtual void	PlayAnimIdleMovingSlow() override;
	virtual void	PlayAnimIdleMovingCrouch() override;
	virtual void	PlayAnimIdleMovingCrouchSlow() override;
	virtual void	PlayAnimIdleSprint				();
	virtual void	PlayAnimIdle					();
	virtual void	PlayReloadSound					();
	virtual void	UpdateSounds() override;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponBM16)
#undef script_type_list
#define script_type_list save_type_list(CWeaponBM16)
