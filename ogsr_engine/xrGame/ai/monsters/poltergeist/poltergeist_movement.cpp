#include "stdafx.h"
#include "poltergeist_movement.h"
#include "poltergeist.h"
#include "../../../detail_path_manager.h"

void CPoltergeisMovementManager::move_along_path(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	if (!m_monster->is_hidden()) {
		inherited::move_along_path(movement_control, dest_position, time_delta);
		return;
	}

	dest_position		= m_monster->m_current_position;

	// Если нет движения по пути
	if ( !inherited::move_along_path( m_monster->m_current_position ) ) {
		m_speed	= 0.f;
		dest_position		= CalculateRealPosition();
		return;
	}

	if (time_delta < EPS) {
		dest_position	= CalculateRealPosition();
		return;
	}

	// Вычислить пройденную дистанцию, определить целевую позицию на маршруте, 
	//			 изменить detail().curr_travel_point_index()

	float desirable_speed = old_desirable_speed(); // желаемая скорость объекта
	float desirable_dist; // = desirable_speed * time_delta;
	float dist;

	// position_computation
	Fvector dir_to_target;
	float dist_to_target;
	u32 current_travel_point = detail().m_current_travel_point;
	dest_position = path_position(old_desirable_speed(), m_monster->m_current_position, time_delta, current_travel_point, dist, dist_to_target, dir_to_target, desirable_dist);

	if (detail().m_current_travel_point != current_travel_point)
		on_travel_point_change(detail().m_current_travel_point);
	detail().m_current_travel_point = current_travel_point;

	Fvector motion;
	if ( !fis_zero( dist_to_target ) ) {
	  motion.mul( dir_to_target, dist / dist_to_target );
	  dest_position.add( motion );
	}
	else {
		m_speed			= 0.f;
		dest_position	= CalculateRealPosition();
		return;
        }

	// установить скорость
	float real_motion = motion.magnitude() + desirable_dist; // - dist;
	float real_speed = real_motion / time_delta;

	m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;

	// Обновить позицию
	m_monster->m_current_position	= dest_position;
	m_monster->Position()			= CalculateRealPosition();
	dest_position					= m_monster->Position();
}

Fvector CPoltergeisMovementManager::CalculateRealPosition()
{
	Fvector ret_val = m_monster->m_current_position;
	ret_val.y += m_monster->m_height;
	return (ret_val);
}
