////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_physic.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager : physic character movement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"
#include "detail_path_manager.h"
#include "level.h"
#include "custommonster.h"
#include "IColisiondamageInfo.h"
#include "profiler.h"
#ifdef DEBUG
#		include "PHDebug.h"
#		define	DBG_PH_MOVE_CONDITIONS(c)				c

#else
#		define	DBG_PH_MOVE_CONDITIONS(c)					
#endif

#define DISTANCE_PHISICS_ENABLE_CHARACTERS 2.f

float CMovementManager::speed			(CPHMovementControl *movement_control) const
{
	VERIFY					(movement_control);
	if (fis_zero(m_speed))
		return				(0.f);

	if (movement_control->IsCharacterEnabled())
		return				(movement_control->GetXZActVelInGoingDir());

	return					(m_speed);
}

void CMovementManager::apply_collision_hit(CPHMovementControl *movement_control)
{
		VERIFY(movement_control);
	if (object().g_Alive()&&!fsimilar(0.f,movement_control->gcontact_HealthLost))
	{
		const ICollisionDamageInfo * di=movement_control->CollisionDamageInfo();
		VERIFY(di);
		Fvector dir;
		di->HitDir(dir);

//		object().Hit	(movement_control->gcontact_HealthLost,dir,di->DamageInitiator(),movement_control->ContactBone(),di->HitPos(), 0.f,ALife::eHitTypeStrike);
		SHit	HDS = SHit(movement_control->gcontact_HealthLost,dir,di->DamageInitiator(),movement_control->ContactBone(),di->HitPos(), 0.f,di->HitType());
		object().Hit(&HDS);
	}
}

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	START_PROFILE("Build Path/Move Along Path")
	VERIFY(movement_control);
	Fvector				motion;
	dest_position		= object().Position();

	float				precision = 0.5f;
	
	
	// Если нет движения по пути
	if (	!enabled() || 
			!actual()  ||
//			path_completed() || 
			detail().path().empty() ||
			detail().completed(dest_position,true) || 
			(detail().curr_travel_point_index() >= detail().path().size() - 1) ||
			fis_zero(old_desirable_speed())
		)
	{
		m_speed			= 0.f;
		

		DBG_PH_MOVE_CONDITIONS( if(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)){movement_control->SetPosition(dest_position);movement_control->DisableCharacter();})
		if(movement_control->IsCharacterEnabled()) {
			movement_control->Calculate(detail().path(),0.f,detail().m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
		}

		// проверка на хит
		apply_collision_hit(movement_control);
		return;
	}

	if(!movement_control->CharacterExist())
	{
#ifdef	DEBUG
		Msg("!! Can not move - physics movement shell does not exist. Try to move in wonded state?");
#endif
		return;
	}

//	VERIFY(movement_control->CharacterExist());


	if (time_delta < EPS) return;

	//#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	
	
	// Вычислить пройденную дистанцию, определить целевую позицию на маршруте, 
	//			 изменить detail().m_current_travel_point
	
	float				desirable_speed		=	old_desirable_speed();				// желаемая скорость объекта
	float				dist				=	desirable_speed * time_delta;		// пройденное расстояние в соостветствие с желаемой скоростью 
	float				desirable_dist		=	dist;

	// определить целевую точку
	Fvector				target;
	
	u32 prev_cur_point_index = detail().curr_travel_point_index();

	// обновить detail().m_current_travel_point в соответствие с текущей позицией
	while (detail().m_current_travel_point < detail().path().size() - 2) {

		float pos_dist_to_cur_point			= dest_position.distance_to(detail().path()[detail().m_current_travel_point].position);
		float pos_dist_to_next_point		= dest_position.distance_to(detail().path()[detail().m_current_travel_point+1].position);
		float cur_point_dist_to_next_point	= detail().path()[detail().m_current_travel_point].position.distance_to(detail().path()[detail().m_current_travel_point+1].position);
		
		if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point)) {
			++detail().m_current_travel_point;			
		} else break;
	}

	target.set			(detail().path()[detail().curr_travel_point_index() + 1].position);
	// определить направление к целевой точке
	Fvector				dir_to_target;
	dir_to_target.sub	(target, dest_position);

	// дистанция до целевой точки
	float				dist_to_target = dir_to_target.magnitude();
	
	while (dist > dist_to_target) {
		dest_position.set	(target);

		if (detail().curr_travel_point_index() + 1 >= detail().path().size())	break;
		else {
			dist			-= dist_to_target;
			++detail().m_current_travel_point;
			if ((detail().curr_travel_point_index()+1) >= detail().path().size())
				break;
			target.set			(detail().path()[detail().curr_travel_point_index() + 1].position);
			dir_to_target.sub	(target, dest_position);
			dist_to_target		= dir_to_target.magnitude();
		}
	}
	
	if (prev_cur_point_index != detail().curr_travel_point_index()) on_travel_point_change(prev_cur_point_index);

	if (dist_to_target < EPS_L) {
#pragma todo("Dima to ? : is this correct?")
		detail().m_current_travel_point = detail().path().size() - 1;
		m_speed			= 0.f;
		return;
	}

	// Физика устанавливает новую позицию
	Device.Statistic->Physics.Begin	();

	// получить физ. объекты в радиусе
	m_nearest_objects.clear_not_free	();
	Level().ObjectSpace.GetNearest		(m_nearest_objects,dest_position,DISTANCE_PHISICS_ENABLE_CHARACTERS + (movement_control->IsCharacterEnabled() ? 0.5f : 0.f),&object()); 

	// установить позицию
	motion.mul			(dir_to_target, dist / dist_to_target);
	dest_position.add	(motion);
	Fvector velocity					=	dir_to_target;
	velocity.normalize_safe();
	if(velocity.y>0.9f)
		velocity.y=0.8f;
	if(velocity.y<-0.9f)
		velocity.y=-0.8f;
	velocity.normalize_safe();							  //как не странно, mdir - не нормирован
	velocity.mul						(desirable_speed);//*1.25f
	if(!movement_control->PhyssicsOnlyMode())
		movement_control->SetCharacterVelocity(velocity);

	if (DBG_PH_MOVE_CONDITIONS(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)||!ph_dbg_draw_mask.test(phDbgAlwaysUseAiPhMove)&&)!(m_nearest_objects.empty())) {  //  физ. объект
		
		if(DBG_PH_MOVE_CONDITIONS(!ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)&&) !movement_control->TryPosition(dest_position)) {
			movement_control->GetPosition	(dest_position);
			movement_control->Calculate		(detail().path(),desirable_speed,detail().m_current_travel_point,precision);

			// проверка на хит
			apply_collision_hit(movement_control);

		} else {
			DBG_PH_MOVE_CONDITIONS( if(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)){movement_control->SetPosition(dest_position);movement_control->DisableCharacter();})
			movement_control->b_exect_position	=	true;

		}
		movement_control->GetPosition	(dest_position);
	}
	else {
		//DBG_PH_MOVE_CONDITIONS( if(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)){movement_control->SetPosition(dest_position);movement_control->DisableCharacter();})
			movement_control->SetPosition(dest_position);
			movement_control->DisableCharacter();
			movement_control->b_exect_position	=	true;

	}
	/*
	} else { // есть физ. объекты

		movement_control->Calculate				(detail().path(), desirable_speed, detail().m_current_travel_point, precision);
		movement_control->GetPosition			(dest_position);
		
		// проверка на хит
		apply_collision_hit						(movement_control);
	}
		*/

	// установить скорость
	float	real_motion	= motion.magnitude() + desirable_dist - dist;
	float	real_speed	= real_motion / time_delta;
	
	m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;
	

	// Физика устанавливает позицию в соответствии с нулевой скоростью 
	if (detail().completed(dest_position,true)) {
		if(!movement_control->PhyssicsOnlyMode()) {
			Fvector velocity				= {0.f,0.f,0.f};
			movement_control->SetVelocity	(velocity);
			m_speed							= 0.f;
		}
	}
	
	Device.Statistic->Physics.End	();

	STOP_PROFILE
}

