#include "stdafx.h"
#include "script_game_object.h"
#include "GameObject.h"
#include "..\xr_3da\xr_collide_defs.h"
#include "GameMtlLib.h"
#include "Level.h"
#include "script_rq_result.h"

void script_rq_result::set_result(collide::rq_result _res)
{
	range		= _res.range;
	element		= _res.element;
	result		= true;
	if (_res.O)
	{
		CGameObject *obj = smart_cast<CGameObject *>(_res.O);
		if (obj)
			object = obj->lua_game_object();
	}
	else {
	  CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + element;
	  mtl = GMLib.GetMaterialByIdx( T->material );
	}
}
using namespace luabind;

#pragma optimize("s",on)
void script_rq_result::script_register( lua_State *L ) {
  module( L ) [
    class_<script_rq_result>( "rq_result" )
    .def_readonly( "range",   &script_rq_result::range )
    .def_readonly( "object",  &script_rq_result::object )
    .def_readonly( "element", &script_rq_result::element )
    .def_readonly( "result",  &script_rq_result::result ),

    class_<SGameMtl>( "SGameMtl" )
    .def_readonly( "m_Name",       &SGameMtl::m_Name )
    .def_readonly( "m_Desc",       &SGameMtl::m_Desc )
    .def_readonly( "Flags",        &SGameMtl::Flags )
    .def_readonly( "fPHFriction",  &SGameMtl::fPHFriction )
    .def_readonly( "fPHDamping",   &SGameMtl::fPHDamping )
    .def_readonly( "fPHSpring",    &SGameMtl::fPHSpring )
    .def_readonly( "fPHBounceStartVelocity", &SGameMtl::fPHBounceStartVelocity )
    .def_readonly( "fPHBouncing",  &SGameMtl::fPHBouncing )
    .def_readonly( "fFlotationFactor", &SGameMtl::fFlotationFactor )
    .def_readonly( "fShootFactor", &SGameMtl::fShootFactor )
    .def_readonly( "fBounceDamageFactor", &SGameMtl::fBounceDamageFactor )
    .def_readonly( "fInjuriousSpeed", &SGameMtl::fInjuriousSpeed )
    .def_readonly( "fVisTransparencyFactor", &SGameMtl::fVisTransparencyFactor )
    .def_readonly( "fSndOcclusionFactor", &SGameMtl::fVisTransparencyFactor )
  ];
}