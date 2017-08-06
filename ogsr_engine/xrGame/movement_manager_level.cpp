////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_level.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for level paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "profiler.h"
#include "level_location_selector.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"
#include "level_path_builder.h"
#include "detail_path_builder.h"
#include "mt_config.h"

void CMovementManager::process_level_path()
{
	START_PROFILE("Build Path/Process Level Path");

	if (!level_path().actual() && (m_path_state > ePathStateBuildLevelPath))
		m_path_state		= ePathStateBuildLevelPath;

	switch (m_path_state) {
		case ePathStateBuildLevelPath : {
			if (can_use_distributed_compuations(mtLevelPath)) {
				level_path_builder().setup(
					object().ai_location().level_vertex_id(),
					level_dest_vertex_id()
				);

				break;
			}

			level_path().build_path(
				object().ai_location().level_vertex_id(),
				level_dest_vertex_id()
			);

			if (level_path().failed())
				break;

			m_path_state		= ePathStateContinueLevelPath;

//			if (time_over())
			if (!m_build_at_once)
				break;
		}
		case ePathStateContinueLevelPath : {
			level_path().select_intermediate_vertex();
			
			m_path_state		= ePathStateBuildDetailPath;

//			if (time_over())
//				break;
		}
		case ePathStateBuildDetailPath : {
			detail().set_state_patrol_path(extrapolate_path());
			detail().set_start_position(object().Position());
			detail().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));

			if (can_use_distributed_compuations(mtDetailPath)) {
				detail_path_builder().setup(
					level_path().path(),
					level_path().intermediate_index()
				);

				break;
			}
			
			detail().build_path(
				level_path().path(),
				level_path().intermediate_index()
			);

			on_build_path	();

			if (detail().failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				break;
			}

			m_path_state		= ePathStatePathVerification;

//			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!level_path().actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (!detail().actual())
					m_path_state	= ePathStateBuildLevelPath;
				else {
					if (detail().completed(object().Position(),!detail().state_patrol_path())) {
						m_path_state	= ePathStateContinueLevelPath;
						if (level_path().completed())
							m_path_state	= ePathStatePathCompleted;
					}
				}
			break;
		}
		case ePathStatePathCompleted : {
			if (!level_path().actual())
				m_path_state = ePathStateBuildLevelPath;
			else
				if (!detail().actual())
					m_path_state = ePathStateBuildLevelPath;
			break;
		}
		default : NODEFAULT;
	}
	STOP_PROFILE;
}
