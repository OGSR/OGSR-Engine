#include "stdafx.h"
#include "control_manager.h"
#include "control_combase.h"
#include "BaseMonster/base_monster.h"

enum EActiveComAction {
	eRemove			= u32(0),
	eAdd			
};


// DEBUG purpose only
char *dbg_control_name_table[] = {
		"Control_Movement",
		"Control_Path",
		"Control_Dir",
		"Control_Animation",
		"Control_Sequencer",
		"Control_RotationJump",
		"Control_Animation_BASE",
		"Control_Movement_BASE",
		"Control_Path_BASE",
		"Control_Dir_BASE"
};	


CControl_Manager::CControl_Manager(CBaseMonster *obj)
{
	m_object			= obj;

	m_animation			= xr_new<CControlAnimation>	();
	m_movement			= xr_new<CControlMovement>	();
	m_direction			= xr_new<CControlDirection>	();

	add			(m_animation,		ControlCom::eControlAnimation);
	add			(m_direction,		ControlCom::eControlDir);
	add			(m_movement,		ControlCom::eControlMovement);
}

CControl_Manager::~CControl_Manager()
{
	xr_delete(m_animation);
	xr_delete(m_direction);
	xr_delete(m_movement);
}

void CControl_Manager::init_external()
{
	//for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)
	//	it->second->init_external(this, m_object);
}

void CControl_Manager::load(LPCSTR section)
{
	init_external	();

	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)
		it->second->load(section);
}
void CControl_Manager::reload(LPCSTR section)
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it) 
		it->second->reload(section);
}

void CControl_Manager::reinit()
{
	if(	m_object->CCustomMonster::use_simplified_visual() ) return;
	// todo: make it simpler
	// reinit pure first, base second, custom third
	CONTROLLERS_MAP_IT it;

	for (it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (is_pure(it->second)) it->second->reinit();
	
	for (it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (is_base(it->second)) it->second->reinit();
	
	for (it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (!is_pure(it->second) && !is_base(it->second)) it->second->reinit();

	// fill active elems
	m_active_elems.clear	();
	m_active_elems.reserve	(ControlCom::eControllersCount);
	for (it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  {
		if (it->second->is_active() && !is_locked(it->second)) {
			m_active_elems.push_back(it->second);
		}
	}

}

struct predicate_remove {
	IC bool	operator() (const CControl_Com *com) {
		return (com == 0);
	}
};

void CControl_Manager::update_frame()
{
	if (!m_object->g_Alive()) return;

	for (COM_VEC_IT it = m_active_elems.begin(); it != m_active_elems.end(); ++it)  {
		// update coms
		if ((*it)) (*it)->update_frame();
	}

	m_active_elems.erase	(
		std::remove_if(
			m_active_elems.begin(),
			m_active_elems.end(),
			predicate_remove()
		),
		m_active_elems.end()
	);
}

void CControl_Manager::update_schedule()
{
	if (!m_object->g_Alive()) return;

	for (COM_VEC_IT it = m_active_elems.begin(); it != m_active_elems.end(); ++it)  {
		// update coms
		if ((*it)) (*it)->update_schedule();
	}

	m_active_elems.erase	(
		std::remove_if(
			m_active_elems.begin(),
			m_active_elems.end(),
			predicate_remove()
		),
		m_active_elems.end()
	);
}

ControlCom::EControlType CControl_Manager::com_type(CControl_Com *com)
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it) 
		if (it->second == com) return it->first;

	return ControlCom::eControlInvalid;
}


void CControl_Manager::notify(ControlCom::EEventType event, ControlCom::IEventData *data)
{
	CONTROLLERS_VECTOR &vect = m_listeners[event];

	for (u32 i = 0; i < vect.size(); i++) {
		VERIFY(vect[i]->cing());
		vect[i]->cing()->on_event(event, data);
	}
}

//////////////////////////////////////////////////////////////////////////
// Messaging
//////////////////////////////////////////////////////////////////////////
void CControl_Manager::subscribe(CControl_Com *com, ControlCom::EEventType type)
{
	m_listeners[type].push_back(com);
}

void CControl_Manager::unsubscribe(CControl_Com *com, ControlCom::EEventType type)
{
	CONTROLLERS_VECTOR &vect = m_listeners[type];

	for (u32 i = 0; i < vect.size(); i++) {
		if (vect[i] == com) {
			vect[i]			= vect.back();
			vect.pop_back	();
			return;
		}
	}
}

ControlCom::IComData *CControl_Manager::data(CControl_Com *who, ControlCom::EControlType type)
{
	CControl_Com *target = m_control_elems[type];

	// get_capturer
	CControl_Com *capturer = target->ced()->capturer();
	if (capturer == who) {
		return target->ced()->data();
	}
	
	return 0;
}

// TODO: check construction of SControl_Element and check init in add-function
void CControl_Manager::add(CControl_Com *com, ControlCom::EControlType type)
{
	m_control_elems[type] = com;
	com->init_external(this, m_object);
}
void CControl_Manager::set_base_controller(CControl_Com *com, ControlCom::EControlType type)
{
	m_base_elems[type]	= com;
}

void CControl_Manager::install_path_manager(CControlPathBuilder *pman)
{
	m_path	= pman;
}

bool CControl_Manager::is_pure(CControl_Com *com)
{
	return (com->cing() == 0);
}

bool CControl_Manager::is_base(CControl_Com *com)
{
	return (com->ced() == 0);
}
bool CControl_Manager::is_locked(CControl_Com *com)
{
	return (com->ced() && com->ced()->is_locked());
}

// capture
void CControl_Manager::capture(CControl_Com *com, ControlCom::EControlType type)  // who, type
{
	CControl_Com *target = m_control_elems[type];
	
	// 1. Check if can capture
	CControl_Com *capturer = target->ced()->capturer();
	VERIFY(!capturer || is_base(capturer));

	if (target->is_active()) {
		target->ced()->on_release						();
		// if there is base capturer - stop control com
		if (capturer) capturer->cing()->on_stop_control	(type);
	}

	// 3. 
	target->ced()->set_capturer		(com);

	// 4.
	target->ced()->on_capture		();

	// 5. 
	com->cing()->on_start_control	(type);
}

void CControl_Manager::release(CControl_Com *com, ControlCom::EControlType type)  // who, type
{
	CControl_Com *target = m_control_elems[type];
	CControl_Com *capturer = target->ced()->capturer();
	VERIFY	(capturer == com);

	// select new capture if there is a base controller
	CONTROLLERS_MAP_IT it = m_base_elems.find(type);
	if (it != m_base_elems.end()) { 
		com->cing()->on_stop_control(type);
		target->ced()->set_capturer	(0);

		capture(m_base_elems[type], type);
	} else {
		// if active - finalize
		if (target->is_active()) {
			target->ced()->on_release		();
			deactivate						(type);
		}

		com->cing()->on_stop_control		(type);
		target->ced()->set_capturer			(0);
	}
}

void CControl_Manager::capture_pure(CControl_Com *com)
{
	capture(com,ControlCom::eControlPath);
	capture(com,ControlCom::eControlAnimation);
	capture(com,ControlCom::eControlMovement);
	capture(com,ControlCom::eControlDir);
}

void CControl_Manager::release_pure(CControl_Com *com)
{
	release(com,ControlCom::eControlPath);
	release(com,ControlCom::eControlAnimation);
	release(com,ControlCom::eControlMovement);
	release(com,ControlCom::eControlDir);
}


void CControl_Manager::activate(ControlCom::EControlType type)
{
	m_control_elems[type]->set_active	();
	check_active_com					(m_control_elems[type], eAdd);
	m_object->on_activate_control		(type);
}
void CControl_Manager::deactivate(ControlCom::EControlType type)
{
	m_control_elems[type]->set_active	(false);
	check_active_com					(m_control_elems[type], eRemove);
}
void CControl_Manager::deactivate(CControl_Com *com)
{
	deactivate(com_type(com));
}

bool CControl_Manager::is_captured(ControlCom::EControlType type)
{
	CControl_Com *capturer = m_control_elems[type]->ced()->capturer();
	if (!capturer || is_base(capturer)) return false;
	
	return true;
}

bool CControl_Manager::is_captured_pure()
{
	return (is_captured(ControlCom::eControlPath) ||
			is_captured(ControlCom::eControlAnimation) ||
			is_captured(ControlCom::eControlMovement) ||
			is_captured(ControlCom::eControlDir));
}


void CControl_Manager::lock(CControl_Com *com, ControlCom::EControlType type)
{
	VERIFY	(is_pure(m_control_elems[type]));
	VERIFY	(m_control_elems[type]->ced()->capturer() == com);
	
	m_control_elems[type]->ced()->set_locked();
	
	// it's now locked so remove from active list
	check_active_com(m_control_elems[type], eRemove);
}

void CControl_Manager::unlock(CControl_Com *com, ControlCom::EControlType type)
{
	VERIFY	(is_pure(m_control_elems[type]));
	VERIFY	(m_control_elems[type]->ced()->capturer() == com);

	m_control_elems[type]->ced()->set_locked(false);
	
	// it's unlocked so add to active list
	check_active_com(m_control_elems[type], eAdd);
}

void CControl_Manager::path_stop(CControl_Com *com)
{
	SControlPathBuilderData		*ctrl_path = (SControlPathBuilderData*)data(com, ControlCom::eControlPath); 
	VERIFY						(ctrl_path);
	ctrl_path->enable			= false;
}

void CControl_Manager::move_stop(CControl_Com *com)
{
	SControlMovementData		*ctrl_move = (SControlMovementData*)data(com, ControlCom::eControlMovement); 
	VERIFY						(ctrl_move);
	ctrl_move->velocity_target	= 0;
	ctrl_move->acc				= flt_max;

}
void CControl_Manager::dir_stop(CControl_Com *com)
{
	SControlDirectionData		*ctrl_dir = (SControlDirectionData*)data(com, ControlCom::eControlDir); 
	VERIFY						(ctrl_dir);
	ctrl_dir->heading.target_speed	= 0;
}

bool CControl_Manager::check_start_conditions(ControlCom::EControlType type)
{
	return m_control_elems[type]->check_start_conditions();
}

bool CControl_Manager::build_path_line(CControl_Com *com, const Fvector &target, u32 node, u32 vel_mask)
{
	CControl_Com *path		= m_control_elems[ControlCom::eControlPath];
	VERIFY					(com == path->ced()->capturer());

	return (path_builder().build_special(target, node, vel_mask));
}

void CControl_Manager::check_active_com(CControl_Com *com, bool b_add)
{
	if (b_add){
		if (com->is_active() && !com->ced()->is_locked()) {
			COM_VEC_IT it = std::find(m_active_elems.begin(),m_active_elems.end(),com);
			if (it == m_active_elems.end()) m_active_elems.push_back(com);
		}
	} else {
		COM_VEC_IT it = std::find(m_active_elems.begin(),m_active_elems.end(),com);
		if (it != m_active_elems.end()) (*it) = 0; // do not remove just mark
	}
}


void CControl_Manager::dump(CControl_Com *com, LPCSTR action, ControlCom::EControlType type)
{
	Msg("---------------------------------------------------------------------------");
	Msg("-- [%s] %s [%s]",dbg_control_name_table[com_type(com)], action, dbg_control_name_table[type]);
	Msg("-- Dump:  -----------------------------------------------------------------");

	u32 index = 0;
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it, index++)  {
		string128 st; st[0]=0;
		if (!it->second->is_inited()) continue;

		if (it->second->ced()) {
			string128 str;
			if (it->second->ced()->capturer()) 
				strcpy(str, dbg_control_name_table[com_type(it->second->ced()->capturer())]);
			else 
				strcpy(str, "NONE");

			sprintf_s(st, "Locked[%u] Capturer[%s]", it->second->ced()->is_locked(), str);
		}

		Msg("[%u] %s: Active[%u] %s",index+1, dbg_control_name_table[it->first], it->second->is_active(),st);
	}
}




