#include "stdafx.h"
#include "controller_psy_hit.h"
#include "../BaseMonster/base_monster.h"
#include "controller.h"
#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../../../level.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../../xr_3da/CameraBase.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../level_debug.h"
#include "../../../ActorCondition.h"
#include "../../../HudManager.h"
#include "../../../inventory.h"

void CControllerPsyHit::load(LPCSTR section)
{
	m_min_tube_dist = READ_IF_EXISTS( pSettings, r_float, section, "tube_condition_min_distance", 10.f );
	m_disable_camera_effect = !!READ_IF_EXISTS(pSettings, r_bool, section, "tube_disable_camera_effect", false);
	m_disable_actor_block = !!READ_IF_EXISTS(pSettings, r_bool, section, "tube_disable_actor_block", false);
}

void CControllerPsyHit::reinit()
{
	inherited::reinit();

	IKinematicsAnimated	*skel = smart_cast<IKinematicsAnimated *>(m_object->Visual());
	m_stage[0] = skel->ID_Cycle_Safe("psy_attack_0"); VERIFY(m_stage[0]);
	m_stage[1] = skel->ID_Cycle_Safe("psy_attack_1"); VERIFY(m_stage[1]);
	m_stage[2] = skel->ID_Cycle_Safe("psy_attack_2"); VERIFY(m_stage[2]);
	m_stage[3] = skel->ID_Cycle_Safe("psy_attack_3"); VERIFY(m_stage[3]);
	m_current_index		= 0;

	m_time_last_tube	= 0;
	m_sound_state		= eNone;
}


bool CControllerPsyHit::tube_ready () const
{
	u32 tube_condition_min_delay	=	5000;
	if ( CController* controller = smart_cast<CController*>(m_object) )
		tube_condition_min_delay	=	controller->m_tube_condition_min_delay;

	return m_time_last_tube + tube_condition_min_delay < time();
}

bool CControllerPsyHit::check_start_conditions()
{
	if (is_active())				
		return false;	

	if (m_man->is_captured_pure())	
		return false;

	if (Actor()->Cameras().GetCamEffector(eCEControllerPsyHit))
		return						false;

	if ( !see_enemy() )
		return						false;

	if ( !tube_ready() )
		return						false;

 	if (m_object->Position().distance_to(Actor()->Position()) < m_min_tube_dist) 
 		return						false;

	return							true;
}

void CControllerPsyHit::activate()
{
	m_man->capture_pure				(this);
	m_man->subscribe				(this, ControlCom::eventAnimationEnd);

	m_man->path_stop				(this);
	m_man->move_stop				(this);

	//////////////////////////////////////////////////////////////////////////
	// set direction
	SControlDirectionData			*ctrl_dir = (SControlDirectionData*)m_man->data(this, ControlCom::eControlDir); 
	VERIFY							(ctrl_dir);
	ctrl_dir->heading.target_speed	= 3.f;
	ctrl_dir->heading.target_angle	= m_man->direction().angle_to_target(Actor()->Position());

	//////////////////////////////////////////////////////////////////////////
	m_current_index					= 0;
	play_anim						();

	m_blocked						= false;

	set_sound_state					(ePrepare);
}

void CControllerPsyHit::deactivate()
{
	m_man->release_pure				(this);
	m_man->unsubscribe				(this, ControlCom::eventAnimationEnd);

	if (m_blocked)
		Actor()->inventory().SetSlotsBlocked(INV_STATE_BLOCK_ALL, false);

	set_sound_state(eNone);
}

void CControllerPsyHit::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	if (type == ControlCom::eventAnimationEnd) {
		if (m_current_index < 3) {
			m_current_index++;
			play_anim			();
			
			switch (m_current_index) {
				case 1: death_glide_start();	break;
				case 2: hit();					break;
				case 3: death_glide_end();		break;
			}
		} else {
			m_man->deactivate	(this);
			return;
		}
	}
}

void CControllerPsyHit::play_anim()
{
	SControlAnimationData		*ctrl_anim = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	VERIFY						(ctrl_anim);

	ctrl_anim->global.set_motion ( m_stage[m_current_index] );
	ctrl_anim->global.actual	= false;
}

namespace detail
{

bool check_actor_visibility (const Fvector trace_from, 
							 const Fvector trace_to,
							 CObject* object)
{
	const float dist = trace_from.distance_to(trace_to);
	Fvector trace_dir;
	trace_dir.sub(trace_to, trace_from);

	//DBG().level_info(this).add_item	(trace_from,trace_to,D3DCOLOR_XRGB(0,150,150));


	collide::rq_result l_rq;
	l_rq.O = NULL;
	Level().ObjectSpace.RayPick(trace_from,
								trace_dir, 
								dist, 
								collide::rqtBoth, 
								l_rq, 
								object);

	return l_rq.O == Actor() || (l_rq.range >= dist - 0.1f);
}

} // namespace detail

bool CControllerPsyHit::see_enemy ()
{
	return	m_object->EnemyMan.see_enemy_now(Actor());
}

bool CControllerPsyHit::check_conditions_final()
{
	if (!m_object->g_Alive())						
		return false;
	
	if (!g_actor)									
		return false;
	
 	if (!m_object->EnemyMan.is_enemy(Actor()))
 		return false;

	if (!Actor()->g_Alive())						
		return false;

	if ( m_object->Position().distance_to_xz(Actor()->Position()) < m_min_tube_dist - 2 )
		return false;
	
	return	see_enemy();
}

void CControllerPsyHit::death_glide_start()
{
	if (!check_conditions_final()) 
	{
		m_man->deactivate(this);
		return;
	}

	// opt 2
	if (!m_disable_actor_block)
	{
		HUD().GetUI()->HideGameIndicators();

		if (CController* controller = smart_cast<CController*>(m_object))
		{
			controller->CControlledActor::install();
			controller->CControlledActor::dont_need_turn();
		}
	}

	if (!m_disable_camera_effect)
	{
		// Start effector
		VERIFY(!Actor()->Cameras().GetCamEffector(eCEControllerPsyHit));

		Fvector src_pos = Actor()->cam_Active()->vPosition;
		Fvector target_pos = m_object->Position();
		target_pos.y += 1.2f;

		Fvector				dir;
		dir.sub(target_pos, src_pos);

		float dist = dir.magnitude();
		dir.normalize();


		float const actor_psy_immunity = Actor()->conditions().GetHitImmunity(ALife::eHitTypeTelepatic);

		target_pos.mad(src_pos, dir, 0.01f + actor_psy_immunity * (dist - 4.8f));


		float const base_fov = g_fov;
		float const dest_fov = g_fov - (g_fov - 10.f)*actor_psy_immunity;

		Actor()->Cameras().AddCamEffector(xr_new<CControllerPsyHitCamEffector>(eCEControllerPsyHit, src_pos, target_pos,
			m_man->animation().motion_time(m_stage[1], m_object->Visual()),
			base_fov, dest_fov));

		smart_cast<CController *>(m_object)->draw_fire_particles();

		dir.sub(src_pos, target_pos);
		dir.normalize();
		float h, p;
		dir.getHP(h, p);
		dir.setHP(h, p + PI_DIV_3);
		Actor()->character_physics_support()->movement()->ApplyImpulse(dir, Actor()->GetMass() * 530.f);
	}

	set_sound_state					(eStart);

	// opt 2
	if (!m_disable_actor_block)
	{
		Actor()->inventory().SetSlotsBlocked(INV_STATE_BLOCK_ALL, true);

		m_blocked = true;
	}

	//////////////////////////////////////////////////////////////////////////
	// set direction
	SControlDirectionData			*ctrl_dir = (SControlDirectionData*)m_man->data(this, ControlCom::eControlDir); 
	VERIFY							(ctrl_dir);
	ctrl_dir->heading.target_speed	= 3.f;
	ctrl_dir->heading.target_angle	= m_man->direction().angle_to_target(Actor()->Position());

	//////////////////////////////////////////////////////////////////////////
}

void CControllerPsyHit::death_glide_end()
{
	CController *monster = smart_cast<CController *>(m_object);
	monster->draw_fire_particles();

	monster->m_sound_tube_hit_left.play_at_pos(Actor(), Fvector().set(-1.f, 0.f, 1.f), sm_2D);
	monster->m_sound_tube_hit_right.play_at_pos(Actor(), Fvector().set(1.f, 0.f, 1.f), sm_2D);

	m_object->Hit_Psy		(Actor(), monster->m_tube_damage);

	m_time_last_tube	=	Device.dwTimeGlobal;
	stop					();
}

void CControllerPsyHit::update_frame()
{
	//if (m_sound_state == eStart) {
	//	CController *monster = smart_cast<CController *>(m_object);
	//	if (!monster->m_sound_tube_start._feedback()) {
	//		m_sound_state = ePull;
	//		monster->m_sound_tube_pull.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
	//	}
	//}
}

void CControllerPsyHit::set_sound_state(ESoundState state)
{
	CController *monster = smart_cast<CController *>(m_object);
	if (state == ePrepare)
	{
		monster->m_sound_tube_prepare.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
	}
	else if (state == eStart)
	{
		if (monster->m_sound_tube_prepare._feedback())	monster->m_sound_tube_prepare.stop();

		monster->m_sound_tube_start.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
		monster->m_sound_tube_pull.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
	}
	else if (state == eHit)
	{
		if (monster->m_sound_tube_start._feedback())	monster->m_sound_tube_start.stop();
		if (monster->m_sound_tube_pull._feedback())		monster->m_sound_tube_pull.stop();

		//monster->m_sound_tube_hit_left.play_at_pos(Actor(), Fvector().set(-1.f, 0.f, 1.f), sm_2D);
		//monster->m_sound_tube_hit_right.play_at_pos(Actor(), Fvector().set(1.f, 0.f, 1.f), sm_2D);
	}
	else if (state == eNone)
	{
		if (monster->m_sound_tube_start._feedback())	monster->m_sound_tube_start.stop();
		if (monster->m_sound_tube_pull._feedback())		monster->m_sound_tube_pull.stop();
		if (monster->m_sound_tube_prepare._feedback())	monster->m_sound_tube_prepare.stop();
	}

	m_sound_state = state;
}

void CControllerPsyHit::hit()
{
	//CController *monster	= smart_cast<CController *>(m_object);
	
	set_sound_state			(eHit);
	//m_object->Hit_Psy		(Actor(), monster->m_tube_damage);
}

void CControllerPsyHit::stop ()
{
	HUD().GetUI()->ShowGameIndicators();

	if ( CController* controller = smart_cast<CController*>(m_object) )
		if ( controller->CControlledActor::is_controlling() )
			controller->CControlledActor::release();

	// Stop camera effector
	CEffectorCam* ce = Actor()->Cameras().GetCamEffector(eCEControllerPsyHit);
	if (ce)
		Actor()->Cameras().RemoveCamEffector(eCEControllerPsyHit);
}

void CControllerPsyHit::on_death()
{
	if (!is_active()) 
		return;

	stop ();

	m_man->deactivate		(this);
}
