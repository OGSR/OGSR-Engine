////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_script.cpp
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_client_classes.h"
#include "base_client_classes_wrappers.h"
#include "script_game_object.h"
#include "exported_classes_def.h"

struct CGlobalFlags { };

using namespace luabind;

#pragma optimize("s",on)

#pragma todo("KRodin: так и хочется порезать четыре говнофункции ниже. Какой-то недоэкспорт непонятно для чего нужный.")

void DLL_PureScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<DLL_Pure,CDLL_PureWrapper>("DLL_Pure")
			.def(constructor<>())
			.def("_construct",&DLL_Pure::_construct,&CDLL_PureWrapper::_construct_static)
	];
}

void ISheduledScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISheduled,CISheduledWrapper>("ISheduled")
//			.def(constructor<>())
	];
}

void IRenderableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<IRenderable,CIRenderableWrapper>("IRenderable")
//			.def(constructor<>())
	];
}

void ICollidableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ICollidable>("ICollidable")
			.def(constructor<>())
	];
}


Fvector rotation_get_dir(SRotation *R, bool v_inverse)
{
	Fvector result;
	if (v_inverse)
		result.setHP(R->yaw, -R->pitch);
	else
		result.setHP(R->yaw, R->pitch);
	return result;
}

void	rotation_set_dir(SRotation *R, const Fvector &dir, bool v_inverse)
{
	R->yaw   = dir.getH(); 
	if (v_inverse)
		R->pitch = -dir.getP();
	else
		R->pitch =  dir.getP();
	R->roll  = 0;
}

void    rotation_copy(SRotation *R, SRotation *src) { memcpy(R, src, sizeof(SRotation)); }
void	rotation_init(SRotation *R, float y, float p, float r)
{
	R->pitch = p;
	R->roll = r;
	R->yaw = y;
}

void CRotationScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<SRotation>("SRotation")
			.def ( constructor<>() )
			.def ( constructor<float, float, float>() )
			.def_readwrite("pitch"				,				&SRotation::pitch)	
			.def_readwrite("yaw"				,				&SRotation::yaw)	
			.def_readwrite("roll"				,				&SRotation::roll)				
			.def("get_dir"						,				&rotation_get_dir)
			.def("set_dir"						,				&rotation_set_dir)
			.def("set"							,				&rotation_copy)
			.def("set"							,				&rotation_init)
		];
}

void CObjectScript::script_register		(lua_State *L)
{
	module(L)
	[
		class_<CGameObject,bases<DLL_Pure,ISheduled,ICollidable,IRenderable>,CGameObjectWrapper>("CGameObject")
			.def(constructor<>())
			.def("_construct",			&CGameObject::_construct,&CGameObjectWrapper::_construct_static)
			.def("Visual",				&CGameObject::Visual)

			.def("net_Export",			&CGameObject::net_Export,		&CGameObjectWrapper::net_Export_static)
			.def("net_Import",			&CGameObject::net_Import,		&CGameObjectWrapper::net_Import_static)
			.def("net_Spawn",			&CGameObject::net_Spawn,	&CGameObjectWrapper::net_Spawn_static)

			.def("use",					&CGameObject::use,	&CGameObjectWrapper::use_static)

//			.def("setVisible",			&CGameObject::setVisible)
			.def("getVisible",			&CGameObject::getVisible)
			.def("getEnabled",			&CGameObject::getEnabled)
//			.def("setEnabled",			&CGameObject::setEnabled)
			,
			class_<CGlobalFlags>("global_flags")  // для оптимальности доступа, предполагается в скриптах скопировать элементы этого "класса" в пространство имен _G 
			.enum_("inventory_item")
			[
				value("FdropManual"				,				int(CInventoryItem::EIIFlags::FdropManual)),
				value("FCanTake"				,				int(CInventoryItem::EIIFlags::FCanTake)),
				value("FCanTrade"				,				int(CInventoryItem::EIIFlags::FCanTrade)),
				value("Fbelt"					,				int(CInventoryItem::EIIFlags::Fbelt)),
				value("Fruck"					,				int(CInventoryItem::EIIFlags::Fruck)),
				value("FRuckDefault"			,				int(CInventoryItem::EIIFlags::FRuckDefault)),
				value("FUsingCondition"			,				int(CInventoryItem::EIIFlags::FUsingCondition)),
				value("FAllowSprint"			,				int(CInventoryItem::EIIFlags::FAllowSprint)),
				value("Fuseful_for_NPC"			,				int(CInventoryItem::EIIFlags::Fuseful_for_NPC)),
				value("FInInterpolation"		,				int(CInventoryItem::EIIFlags::FInInterpolation)),
				value("FInInterpolate"			,				int(CInventoryItem::EIIFlags::FInInterpolate)),
				value("FIsQuestItem"			,				int(CInventoryItem::EIIFlags::FIsQuestItem)),
				value("FIAlwaysUntradable"		,				int(CInventoryItem::EIIFlags::FIAlwaysUntradable)),
				value("FIUngroupable"			,				int(CInventoryItem::EIIFlags::FIUngroupable)),
				value("FIHiddenForInventory"	,				int(CInventoryItem::EIIFlags::FIHiddenForInventory))
			]
			.enum_("se_object_flags")
			[
				value("flUseSwitches"			,				int(CSE_ALifeObject:: flUseSwitches)),
				value("flSwitchOnline"			,				int(CSE_ALifeObject:: flSwitchOnline)),
				value("flSwitchOffline"			,				int(CSE_ALifeObject:: flSwitchOffline)),
				value("flInteractive"			,				int(CSE_ALifeObject:: flInteractive)),
				value("flVisibleForAI"			,				int(CSE_ALifeObject:: flVisibleForAI)),
				value("flUsefulForAI"			,				int(CSE_ALifeObject:: flUsefulForAI)),
				value("flOfflineNoMove"			,				int(CSE_ALifeObject:: flOfflineNoMove)),
				value("flUsedAI_Locations"		,				int(CSE_ALifeObject:: flUsedAI_Locations)),
				value("flGroupBehaviour"		,				int(CSE_ALifeObject:: flGroupBehaviour)),
				value("flCanSave"				,				int(CSE_ALifeObject:: flCanSave)),
				value("flVisibleForMap"			,				int(CSE_ALifeObject:: flVisibleForMap)),
				value("flUseSmartTerrains"		,				int(CSE_ALifeObject:: flUseSmartTerrains)),
				value("flCheckForSeparator"		,				int(CSE_ALifeObject:: flCheckForSeparator))
			]
			.enum_("weapon_states")
			[
				value("eIdle",		int(CHudItem::EHudStates::eIdle)),
				value("eFire",		int(CWeapon::EWeaponStates::eFire)),
				value("eFire2",		int(CWeapon::EWeaponStates::eFire2)),
				value("eReload",	int(CWeapon::EWeaponStates::eReload)),
				value("eShowing",	int(CHudItem::EHudStates::eShowing)),
				value("eHiding",	int(CHudItem::EHudStates::eHiding)),
				value("eHidden",	int(CHudItem::EHudStates::eHidden)),
				value("eMisfire",	int(CWeapon::EWeaponStates::eMisfire)),
				value("eMagEmpty",	int(CWeapon::EWeaponStates::eMagEmpty)),
				value("eSwitch",	int(CWeapon::EWeaponStates::eSwitch))
			]
			.enum_("RestrictionSpace")
			[
				value("eDefaultRestrictorTypeNone", int(RestrictionSpace::eDefaultRestrictorTypeNone)),
				value("eDefaultRestrictorTypeOut",  int(RestrictionSpace::eDefaultRestrictorTypeOut)),
				value("eDefaultRestrictorTypeIn",   int(RestrictionSpace::eDefaultRestrictorTypeIn)),
				value("eRestrictorTypeNone",        int(RestrictionSpace::eRestrictorTypeNone)),
				value("eRestrictorTypeIn",          int(RestrictionSpace::eRestrictorTypeIn)),
				value("eRestrictorTypeOut",         int(RestrictionSpace::eRestrictorTypeOut))
			]
	];
}


CCameraBase* actor_camera(u16 index)
{
	auto pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return nullptr;

	return pA->cam_ByIndex(index);
}

void CCameraScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<CCameraBase>("CCameraBase")
			.def_readwrite("aspect",		&CCameraBase::f_aspect)
			.def_readonly ("direction",		&CCameraBase::vDirection)
			.def_readwrite("fov",			&CCameraBase::f_fov)
			.def_readwrite("position",		&CCameraBase::vPosition)

			.def_readwrite("lim_yaw",		&CCameraBase::lim_yaw)
			.def_readwrite("lim_pitch",		&CCameraBase::lim_pitch)
			.def_readwrite("lim_roll",		&CCameraBase::lim_roll)

			.def_readwrite("yaw",			&CCameraBase::yaw)
			.def_readwrite("pitch",			&CCameraBase::pitch)
			.def_readwrite("roll",			&CCameraBase::roll),


			def("actor_camera",				&actor_camera)
		];
}


void CAnomalyDetectorScript::script_register( lua_State *L ) {
  module( L ) [
    class_<CAnomalyDetector>( "CAnomalyDetector" )
    .def_readwrite( "Anomaly_Detect_Radius", &CAnomalyDetector::m_radius )
    .def_readwrite( "Anomaly_Detect_Time_Remember", &CAnomalyDetector::m_time_to_rememeber )
    .def_readwrite( "Anomaly_Detect_Probability", &CAnomalyDetector::m_detect_probability )
    .def_readonly( "is_active", &CAnomalyDetector::m_active )
    .def( "activate", &CAnomalyDetector::activate )
    .def( "deactivate", &CAnomalyDetector::deactivate )
    .def( "remove_all_restrictions", &CAnomalyDetector::remove_all_restrictions )
    .def( "remove_restriction", &CAnomalyDetector::remove_restriction )
  ];
}


LPCSTR CPatrolPointScript::getName( CPatrolPoint *pp ) {
  return pp->m_name.c_str();
}

void CPatrolPointScript::setName( CPatrolPoint *pp, LPCSTR str ) {
  pp->m_name = shared_str( str );
}

void CPatrolPointScript::script_register( lua_State *L ) {
  module( L ) [
    class_<CPatrolPoint>( "CPatrolPoint" )
    .def( constructor<>() )
    .def_readwrite( "m_position",        &CPatrolPoint::m_position )
    .def_readwrite( "m_flags",           &CPatrolPoint::m_flags )
    .def_readwrite( "m_level_vertex_id", &CPatrolPoint::m_level_vertex_id )
    .def_readwrite( "m_game_vertex_id",  &CPatrolPoint::m_game_vertex_id )
    .property( "m_name", &CPatrolPointScript::getName, &CPatrolPointScript::setName )
    .def( "position", ( CPatrolPoint& ( CPatrolPoint::* ) ( Fvector ) ) ( &CPatrolPoint::position ) )
  ];
}


void CPatrolPathScript::script_register( lua_State *L ) {
  module( L ) [
    class_<CPatrolPath>( "CPatrolPath" )
    .def( constructor<>() )
    .def( "add_point", &CPatrolPath::add_point )
    .def( "point", ( CPatrolPoint ( CPatrolPath::* ) ( u32 ) ) ( &CPatrolPath::point ) )
    .def( "add_vertex", &CPatrolPath::add_vertex )
  ];
}


decltype(auto) script_texture_find(const char* name)
{
	auto textures = Device.m_pRender->GetResourceManager()->FindTexture(name);
	auto table = luabind::newtable(ai().script_engine().lua());

	for (const auto& tex : textures)
		table[tex->GetName()] = tex; // key - texture name, value - texture object

	return table;
}

void script_texture_load(ITexture* t, const char* name)
{
	t->Unload();
	t->Load(name);
}

void ITextureScript::script_register(lua_State *L)
{
	module(L)
	[
		def("texture_find", &script_texture_find)
		,
		class_<ITexture>("ITexture")
		.def("load", &script_texture_load)
		.def("get_name", &ITexture::GetName)
	];
}


void CPHCaptureScript::script_register( lua_State *L ) {
  module( L ) [
    class_<CPHCapture>( "CPHCapture" )
    .def_readonly( "e_state", &CPHCapture::e_state )
    .def_readwrite( "capture_force", &CPHCapture::m_capture_force )
    .def_readwrite( "distance",      &CPHCapture::m_capture_distance )
    .def_readwrite( "hard_mode",     &CPHCapture::m_hard_mode )
    .def_readwrite( "pull_distance", &CPHCapture::m_pull_distance )
    .def_readwrite( "pull_force",    &CPHCapture::m_pull_force )
    .def_readwrite( "time_limit",    &CPHCapture::m_capture_time ),

    class_<enum_exporter<EPHCaptureState>>( "ph_capture" )
    .enum_( "ph_capture" ) [
      value( "pulling",  int( EPHCaptureState::cstPulling  ) ),
      value( "captured", int( EPHCaptureState::cstCaptured ) ),
      value( "released", int( EPHCaptureState::cstReleased ) )
    ]
  ];
}
