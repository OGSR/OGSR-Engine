#include "stdafx.h"
#include "WeaponShotgun.h"

using namespace luabind;

#pragma optimize("s", on)
void CWeaponShotgun::script_register(lua_State* L)
{
    module(L)[class_<CWeaponShotgun, CGameObject>("CWeaponShotgun")
                  .def(constructor<>())
#ifdef DUPLET_STATE_SWITCH
                  .def_readonly("is_duplet_enabled", &CWeaponShotgun::is_duplet_enabled)
                  .def("switch_duplet", &CWeaponShotgun::SwitchDuplet)
#endif // !DUPLET_STATE_SWITCH
    ];
}
