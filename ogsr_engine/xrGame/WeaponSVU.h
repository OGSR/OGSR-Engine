#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponSVU : public CWeaponCustomPistol
{
    typedef CWeaponCustomPistol inherited;

public:
    CWeaponSVU(void);
    virtual ~CWeaponSVU(void);

protected:
    virtual size_t GetWeaponTypeForCollision() const override { return SniperRifle; } //Вообще СВУ короткая, мб и не надо её относить к длинным снайперкам типа СВД

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponSVU)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVU)
