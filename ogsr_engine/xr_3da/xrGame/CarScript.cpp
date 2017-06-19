#include "pch_script.h"
#include "alife_space.h"
#include "Car.h"
#include "CarWeapon.h"
#include "script_game_object.h"

using namespace luabind;

u8 CCar__IsLightsOn(CCar *self)
{
	return self->IsLightsOn() ? 1 : 0;
}
u8 CCar__IsEngineOn(CCar *self)
{
	return self->IsEngineOn() ? 1 : 0;
}

#pragma optimize("s",on)
void CCar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCar,bases<CGameObject,CHolderCustom> >("CCar")
			.enum_("wpn_action")
				[
					value("eWpnDesiredDir",							int(CCarWeapon::eWpnDesiredDir)),
					value("eWpnDesiredPos",							int(CCarWeapon::eWpnDesiredPos)),
					value("eWpnActivate",							int(CCarWeapon::eWpnActivate)),
					value("eWpnFire",								int(CCarWeapon::eWpnFire)),
					value("eWpnAutoFire",							int(CCarWeapon::eWpnAutoFire)),
					value("eWpnToDefaultDir",						int(CCarWeapon::eWpnToDefaultDir))
				]
		.def("Action",			&CCar::Action)
//		.def("SetParam",		(void (CCar::*)(int,Fvector2)) &CCar::SetParam)
		.def("SetParam",		(void (CCar::*)(int,Fvector)) &CCar::SetParam)
		.def("CanHit",			&CCar::WpnCanHit)
		.def("FireDirDiff",		&CCar::FireDirDiff)
		.def("IsObjectVisible",	&CCar::isObjectVisible)
		.def("HasWeapon",		&CCar::HasWeapon)
		.def("CurrentVel",		&CCar::CurrentVel)
		.def("GetfHealth",		&CCar::GetfHealth)
		.def("SetfHealth",		&CCar::SetfHealth)
		.def("SetExplodeTime",	&CCar::SetExplodeTime)
		.def("ExplodeTime",		&CCar::ExplodeTime)
		.def("CarExplode",		&CCar::CarExplode)

		.def("GetFuelTank", &CCar::GetFuelTank)
		.def("GetFuel", &CCar::GetFuel)
		.def("SetFuel", &CCar::SetFuel)
		.def("IsLightsOn", &CCar__IsLightsOn)
		.def("IsEngineOn", &CCar__IsEngineOn)
		.def("SwitchEngine", &CCar::SwitchEngine)
		.def("SwitchLights", &CCar::SwitchLights)
		.def(constructor<>())
	];
}