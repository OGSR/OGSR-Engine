#pragma once

#include "WeaponMagazined.h"

#define SND_RIC_COUNT 5
 
class CWeaponCustomPistol: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
					CWeaponCustomPistol	(LPCSTR name);
	virtual			~CWeaponCustomPistol();
	virtual	int		GetCurrentFireMode	() { return 1; };
protected:
	virtual void	FireEnd				();
	virtual void	switch2_Fire		();

	virtual void	net_Destroy();
	virtual void	OnDrawUI();
	virtual void	net_Relcase(CObject *object);
};
