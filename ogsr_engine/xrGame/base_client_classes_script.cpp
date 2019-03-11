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
#include "../xr_3da/feel_sound.h"
#include "../xr_3da/fbasicvisual.h"
#include "../xr_3da/skeletonanimated.h"
#include "ai/stalker/ai_stalker.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "Actor.h"
#include "Explosive.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "xrServer_Space.h"
#include "xrServer_Objects_ALife.h"
#include "Weapon.h"
#include "space_restriction.h"
#include "../COMMON_AI/PATH/patrol_path.h"
#include "../COMMON_AI/PATH/patrol_point.h"
#include "../xr_3da/ResourceManager.h"
#include "../xr_3da/device.h"
#include "../xr_3da/Render.h"

#include "exported_classes_def.h"

struct CGlobalFlags { };

using namespace luabind;

#pragma optimize("s",on)
void DLL_PureScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<DLL_Pure,CDLL_PureWrapper>("DLL_Pure")
			.def(constructor<>())
			.def("_construct",&DLL_Pure::_construct,&CDLL_PureWrapper::_construct_static)
	];
}

/*
void ISpatialScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISpatial,CISpatialWrapper>("ISpatial")
			.def(constructor<>())
			.def("spatial_register",	&ISpatial::spatial_register,	&CISpatialWrapper::spatial_register_static)
			.def("spatial_unregister",	&ISpatial::spatial_unregister,	&CISpatialWrapper::spatial_unregister_static)
			.def("spatial_move",		&ISpatial::spatial_move,		&CISpatialWrapper::spatial_move_static)
			.def("spatial_sector_point",&ISpatial::spatial_sector_point,&CISpatialWrapper::spatial_sector_point_static)
			.def("dcast_CObject",		&ISpatial::dcast_CObject,		&CISpatialWrapper::dcast_CObject_static)
			.def("dcast_FeelSound",		&ISpatial::dcast_FeelSound,		&CISpatialWrapper::dcast_FeelSound_static)
			.def("dcast_Renderable",	&ISpatial::dcast_Renderable,	&CISpatialWrapper::dcast_Renderable_static)
			.def("dcast_Light",			&ISpatial::dcast_Light,			&CISpatialWrapper::dcast_Light_static)
	];
}
*/

void ISheduledScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISheduled,CISheduledWrapper>("ISheduled")
//			.def(constructor<>())
//			.def("shedule_Scale",		&ISheduled::shedule_Scale,		&CISheduledWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&ISheduled::shedule_Update,		&CISheduledWrapper::shedule_Update_static)
	];
}

void IRenderableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<IRenderable,CIRenderableWrapper>("IRenderable")
//			.def(constructor<>())
//			.def("renderable_Render",&IRenderable::renderable_Render,&CIRenderableWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&IRenderable::renderable_ShadowGenerate,&CIRenderableWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&IRenderable::renderable_ShadowReceive,&CIRenderableWrapper::renderable_ShadowReceive_static)
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
//		class_<CObject,bases<DLL_Pure,ISheduled,ICollidable,IRenderable>,CObjectWrapper>("CObject")
//			.def(constructor<>())
//			.def("_construct",			&CObject::_construct,&CObjectWrapper::_construct_static)
/*			
			.def("spatial_register",	&CObject::spatial_register,	&CObjectWrapper::spatial_register_static)
			.def("spatial_unregister",	&CObject::spatial_unregister,	&CObjectWrapper::spatial_unregister_static)
			.def("spatial_move",		&CObject::spatial_move,		&CObjectWrapper::spatial_move_static)
			.def("spatial_sector_point",&CObject::spatial_sector_point,&CObjectWrapper::spatial_sector_point_static)
			.def("dcast_FeelSound",		&CObject::dcast_FeelSound,		&CObjectWrapper::dcast_FeelSound_static)
			.def("dcast_Light",			&CObject::dcast_Light,			&CObjectWrapper::dcast_Light_static)
*/			
//			.def("shedule_Scale",		&CObject::shedule_Scale,		&CObjectWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&CObject::shedule_Update,		&CObjectWrapper::shedule_Update_static)

//			.def("renderable_Render"		,&CObject::renderable_Render,&CObjectWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&CObject::renderable_ShadowGenerate,&CObjectWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&CObject::renderable_ShadowReceive,&CObjectWrapper::renderable_ShadowReceive_static)
//			.def("Visual",					&CObject::Visual)

		class_<CGameObject,bases<DLL_Pure,ISheduled,ICollidable,IRenderable>,CGameObjectWrapper>("CGameObject")
			.def(constructor<>())
			.def("_construct",			&CGameObject::_construct,&CGameObjectWrapper::_construct_static)
			.def("Visual",				&CGameObject::Visual)
/*
			.def("spatial_register",	&CGameObject::spatial_register,	&CGameObjectWrapper::spatial_register_static)
			.def("spatial_unregister",	&CGameObject::spatial_unregister,	&CGameObjectWrapper::spatial_unregister_static)
			.def("spatial_move",		&CGameObject::spatial_move,		&CGameObjectWrapper::spatial_move_static)
			.def("spatial_sector_point",&CGameObject::spatial_sector_point,&CGameObjectWrapper::spatial_sector_point_static)
			.def("dcast_FeelSound",		&CGameObject::dcast_FeelSound,		&CGameObjectWrapper::dcast_FeelSound_static)
			.def("dcast_Light",			&CGameObject::dcast_Light,			&CGameObjectWrapper::dcast_Light_static)
*/
//			.def("shedule_Scale",		&CGameObject::shedule_Scale,		&CGameObjectWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&CGameObject::shedule_Update,		&CGameObjectWrapper::shedule_Update_static)

//			.def("renderable_Render"		,&CGameObject::renderable_Render,&CGameObjectWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&CGameObject::renderable_ShadowGenerate,&CGameObjectWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&CGameObject::renderable_ShadowReceive,&CGameObjectWrapper::renderable_ShadowReceive_static)

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
				value("eIdle",		int(CWeapon::EWeaponStates::eIdle)),
				value("eFire",		int(CWeapon::EWeaponStates::eFire)),
				value("eFire2",		int(CWeapon::EWeaponStates::eFire2)),
				value("eReload",	int(CWeapon::EWeaponStates::eReload)),
				value("eShowing",	int(CWeapon::EWeaponStates::eShowing)),
				value("eHiding",	int(CWeapon::EWeaponStates::eHiding)),
				value("eHidden",	int(CWeapon::EWeaponStates::eHidden)),
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

//		,class_<CPhysicsShellHolder,CGameObject>("CPhysicsShellHolder")
//			.def(constructor<>())

//		,class_<CEntity,CPhysicsShellHolder,CEntityWrapper>("CEntity")
//			.def(constructor<>())
//			.def("HitSignal",&CEntity::HitSignal,&CEntityWrapper::HitSignal_static)
//			.def("HitImpulse",&CEntity::HitImpulse,&CEntityWrapper::HitImpulse_static)

//		,class_<CEntityAlive,CEntity>("CEntityAlive")
//			.def(constructor<>())

//		,class_<CCustomMonster,CEntityAlive>("CCustomMonster")
//			.def(constructor<>())

//		,class_<CAI_Stalker,CCustomMonster>("CAI_Stalker")
	];
}

// alpet ======================== SCRIPT_TEXTURE_CONTROL BEGIN =========== 

IRender_Visual* visual_get_child(IRender_Visual	*v, u32 n_child)
{
	if (!v) return NULL; // not have visual
	CKinematics *k = smart_cast<CKinematics*> (v);
	if (!k) return NULL;
	if (n_child >= k->children.size()) return NULL;
	return k->children.at(n_child);
}

CTexture* visual_get_texture(IRender_Visual *child_v, int n_texture)
{
	if (!child_v) return NULL; // not have visual

	const int max_tex_number = 255;

	n_texture = (n_texture > max_tex_number) ? max_tex_number : n_texture;

	// визуал выстраивается иерархически - есть потомки и предки

	Shader* s = child_v->shader._get();

	if (s && s->E[0]._get()) // обычно в первом элементе находится исчерпывающий список текстур 
	{
		ShaderElement* E = s->E[0]._get();

		int tex_count = 0;

		for (u32 p = 0; p < E->passes.size(); p++)
		{
			if (E->passes[p]._get())
			{
				SPass* pass = E->passes[p]._get();

				STextureList* tlist = pass->T._get();
				if (!tlist)
					continue;

				for (u32 t = 0; t < tlist->size() && tex_count <= n_texture; t++, tex_count++)
					if (tex_count == n_texture)
					{
						return tlist->at(t).second._get();
					}
			}
		}
	}

	return NULL;
}

void IRender_VisualScript::script_register(lua_State *L)
{
	module(L)
	[
		class_<IRender_Visual>("IRender_Visual")
			.def(constructor<>())
			.def("dcast_PKinematicsAnimated",&IRender_Visual::dcast_PKinematicsAnimated)
			.def("child", &visual_get_child)

			.def("get_texture", &visual_get_texture)
	];
}

void CKinematicsAnimated_PlayCycle(CKinematicsAnimated* sa, LPCSTR anim)
{
	sa->PlayCycle(anim);
}

void CKinematicsAnimatedScript::script_register		(lua_State *L)
{
	module(L)
	[
		class_<CKinematicsAnimated>("CKinematicsAnimated")
			.def("PlayCycle",		&CKinematicsAnimated_PlayCycle)
	];
}

void CBlendScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<CBlend>("CBlend")
			//			.def(constructor<>())
		];
}

// alpet ======================== CAMERA SCRIPT OBJECT =================

CCameraBase* actor_camera(u16 index)
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (!pA) return NULL;
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
// alpet ======================== CAMERA SCRIPT OBJECT =================

/*
void CKinematicsScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<CKinematics, FHierrarhyVisual>("CKinematics")
			//			.def(constructor<>())
		];
}

void FHierrarhyVisualScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<FHierrarhyVisual, IRender_Visual>("FHierrarhyVisual")
			//			.def(constructor<>())
		];
}
*/

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

// alpet ======================== SCRIPT_TEXTURE_CONTROL EXPORTS 2 =========== 

CTexture* script_object_get_texture(CScriptGameObject *script_obj, u32 n_child, u32 n_texture)
{
	IRender_Visual* v = script_obj->object().Visual();
	IRender_Visual* child_v = visual_get_child(v, n_child);
	return visual_get_texture(child_v, n_texture);
}

decltype(auto) script_texture_find(const char* name)
{
	auto textures = Device.Resources->_FindTexture(name);
	auto table = luabind::newtable(ai().script_engine().lua());

	for (auto& tex : textures)
		table[tex->cName.c_str()] = tex; // key - texture name, value - texture object

	return table;
}

LPCSTR script_texture_getname(CTexture *t)
{
	return t->cName.c_str();
}

void script_texture_load(CTexture *t, LPCSTR name)
{
	t->Unload();
	t->Preload(name);
	t->Load(name);
}

void CTextureScript::script_register(lua_State *L)
{
	// added by alpet 10.07.14
	module(L)
		[
			class_<CTexture>("CTexture")
			.def("load", &script_texture_load)
			.def("get_name", &script_texture_getname)
			.def_readonly("ref_count", &CTexture::dwReference)
		];
}

using namespace luabind;

void CResourceManagerScript::script_register(lua_State *L)
{
	// added by alpet 10.07.14
	module(L)[
		// added by alpet
		def("texture_find", &script_texture_find),
		def("texture_load", &script_texture_load),
		def("texture_from_object", &script_object_get_texture),
		def("texture_from_visual", &visual_get_texture),
		def("texture_get_name", &script_texture_getname)
	];
}
// alpet ======================== SCRIPT_TEXTURE_CONTROL END =========== 


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
