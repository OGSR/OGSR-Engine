////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_action.cpp
//	Created 	: 27.12.2003
//  Modified 	: 03.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sight_action.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "inventory.h"

//#define SIGHT_TEST

void CSightAction::initialize					()
{
	m_start_time	= Device.dwTimeGlobal;
	
	if (SightManager::eSightTypeCoverLookOver == m_sight_type)
		initialize_cover_look_over	();

	if (SightManager::eSightTypeFireObject == m_sight_type)
		initialize_fire_object		();
}

void CSightAction::execute						()
{
	switch (m_sight_type) {
		case SightManager::eSightTypeCurrentDirection : {
			execute_current_direction	();
			break;
		}
		case SightManager::eSightTypePathDirection : {
			execute_path_direction	();
			break;
		}
		case SightManager::eSightTypeDirection : {
			execute_direction			();
			break;
		}
		case SightManager::eSightTypePosition : {
			execute_position			();
			break;
		}
		case SightManager::eSightTypeObject : {
			execute_object			();
			break;
		}
		case SightManager::eSightTypeCover : {
			execute_cover				();
			break;
		}
		case SightManager::eSightTypeSearch : {
			execute_search			();
			break;
		}
		case SightManager::eSightTypeCoverLookOver : {
			execute_cover_look_over	();
			break;
		}
		case SightManager::eSightTypeFireObject : {
			execute_fire_object		();
			break;
		}
		default	: NODEFAULT;
	}
}

void CSightAction::remove_links					(CObject *object)
{
	if (!m_object_to_look)
		return;

	if (m_object_to_look->ID() != object->ID())
		return;
	
//	execute				();
	
	m_object_to_look	= 0;

	m_sight_type		= SightManager::eSightTypeDirection;
	m_vector3d.setHP	(-this->object().movement().m_head.target.yaw,this->object().movement().m_head.target.pitch);
}

bool CSightAction::target_reached				()
{
	return				(!!fsimilar(object().movement().m_head.target.yaw,object().movement().m_head.current.yaw));
}

void CSightAction::execute_current_direction	()
{
	object().movement().m_head.target	= object().movement().m_head.current;
#ifdef SIGHT_TEST
	Msg					("%6d eSightTypeCurrentDirection",Device.dwTimeGlobal);
#endif
}

void CSightAction::execute_path_direction		()
{
	object().sight().SetDirectionLook();
#ifdef SIGHT_TEST
	Msg					("%6d eSightTypePathDirection",Device.dwTimeGlobal);
#endif
}

void CSightAction::execute_direction			()
{
	m_vector3d.getHP	(object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
	object().movement().m_head.target.yaw		*= -1;
	object().movement().m_head.target.pitch	*= -1;
#ifdef SIGHT_TEST
	Msg					("%6d eSightTypeDirection",Device.dwTimeGlobal);
#endif
}

void CSightAction::execute_position				()
{
	if (m_torso_look)
		object().sight().SetFirePointLookAngles	(m_vector3d,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
	else
		object().sight().SetPointLookAngles		(m_vector3d,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
#ifdef SIGHT_TEST
	Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFirePosition" : "eSightTypePosition");
#endif
}

void CSightAction::execute_object				()
{
	Fvector					look_pos;
	m_object_to_look->Center(look_pos);

	const CEntityAlive		*entity_alive = smart_cast<const CEntityAlive*>(m_object_to_look);
	if (!entity_alive || entity_alive->g_Alive()) {
		look_pos.x			= m_object_to_look->Position().x;
		look_pos.z			= m_object_to_look->Position().z;
	}

	if (m_torso_look)
		object().sight().SetFirePointLookAngles	(look_pos,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch,m_object_to_look);
	else
		object().sight().SetPointLookAngles		(look_pos,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch,m_object_to_look);

//	Msg						("execute_object(%f)(%s)my_position[%f][%f][%f],object_position[%f][%f][%f]",object().movement().m_head.target.yaw,*m_object_to_look->cName(),VPUSH(m_object->eye_matrix.c),VPUSH(m_object_to_look->Position()));

	if (m_no_pitch)
		object().movement().m_head.target.pitch	= 0.f;

#ifdef SIGHT_TEST
	Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireObject" : "eSightTypeObject");
#endif
}

void CSightAction::execute_cover				()
{
	if (m_torso_look)
		object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),PI,m_path);
	else
		object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),m_path);
#ifdef SIGHT_TEST
	Msg					("%6d %s [%f] -> [%f]",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireCover" : "eSightTypeCover",object().movement().m_body.current.yaw,object().movement().m_body.target.yaw);
#endif
}

void CSightAction::execute_search				()
{
	m_torso_look					= false;
	if (m_torso_look)
		object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),PI,m_path);
	else
		object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),m_path);
	object().movement().m_head.target.pitch	= PI_DIV_4;
#ifdef SIGHT_TEST
	Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireSearch" : "eSightTypeSearch");
#endif
}

void CSightAction::initialize_cover_look_over	()
{
	m_internal_state	= 2;
	m_start_state_time	= Device.dwTimeGlobal;
	m_stop_state_time	= 3500;
	execute_cover		();
	m_cover_yaw			= object().movement().m_head.target.yaw;
}

void CSightAction::execute_cover_look_over		()
{
	switch (m_internal_state) {
		case 0 :
		case 2 : {
			if ((m_start_state_time + m_stop_state_time < Device.dwTimeGlobal) && target_reached()) {
				m_start_state_time	= Device.dwTimeGlobal;
				m_stop_state_time	= 3500;
				m_internal_state	= 1;
				object().movement().m_head.target.yaw = m_cover_yaw + ::Random.randF(-PI_DIV_8,PI_DIV_8);
			}
			break;
		}
		case 1 : {
			if ((m_start_state_time + m_stop_state_time < Device.dwTimeGlobal) && target_reached()) {
				execute_cover		();
				m_internal_state	= 0;
				m_start_state_time	= Device.dwTimeGlobal;
			}
			break;
		}
		default : NODEFAULT;
	}
}

bool CSightAction::change_body_speed			() const
{
	return	(false);
}

float CSightAction::body_speed					() const
{
	return	(object().movement().m_body.speed);
}

bool CSightAction::change_head_speed			() const
{
	return	((SightManager::eSightTypeCoverLookOver == m_sight_type) && (m_internal_state != 2));
}

float CSightAction::head_speed					() const
{
	VERIFY	(SightManager::eSightTypeCoverLookOver == m_sight_type);
	return	(PI_DIV_8*.5f);
}

void CSightAction::initialize_fire_object		()
{
	m_holder_start_position	= m_object->Position();
	m_object_start_position	= m_object_to_look->Position();
	m_state_fire_object		= 0;
}

void CSightAction::execute_fire_object			()
{
	switch (m_state_fire_object) {
		case 0 : {
			execute_object	();

			if (target_reached() && object().inventory().ActiveItem()) {
				if (!m_object->can_kill_enemy() || m_object->can_kill_member())
					m_state_fire_object	= 1;
			}
			break;
		}
		case 1 : {
			if (!m_holder_start_position.similar(m_object->Position(),.5f)) {
				m_state_fire_object	= 0;
				break;
			}

			if (!m_object_start_position.similar(m_object_to_look->Position(),.5f)) {
				m_state_fire_object	= 0;
				break;
			}

			m_object->feel_vision_get	(objects);
			if (std::find(objects.begin(),objects.end(),m_object_to_look) != objects.end()) {
				m_vector3d			= m_object->feel_vision_get_vispoint(const_cast<CGameObject*>(m_object_to_look));
				execute_position	();
				break;
			}
			break;
		}
		default : NODEFAULT;
	}
}
