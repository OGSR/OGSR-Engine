////////////////////////////////////////////////////////////////////////////
//	Module 		: detail_path_builder.h
//  Modified 	: 21.02.2005
//  Modified 	: 21.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Detail path builder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "detail_path_manager.h"

class CDetailPathBuilder {
private:
	typedef CMovementManager::CLevelPathManager CLevelPathManager;
	typedef CLevelPathManager::PATH				PATH;
private:
	CMovementManager		*m_object;
	const PATH				*m_level_path;
	u32						m_path_vertex_index;

public:
	IC						CDetailPathBuilder	(CMovementManager *object)
	{
		VERIFY				(object);
		m_object			= object;
	}
	
	IC		void			setup			(const PATH &level_path, const u32 &path_vertex_index)
	{
		m_object->m_wait_for_distributed_computation	= true;
		m_level_path		= &level_path;
		m_path_vertex_index	= path_vertex_index;
		Device.seqParallel.push_back	(fastdelegate::FastDelegate0<>(this,&CDetailPathBuilder::process));
	}

			void __stdcall	process			()
	{
		m_object->m_wait_for_distributed_computation	= false;
		m_object->detail().build_path	(*m_level_path,m_path_vertex_index);

		m_object->on_build_path			();

		if (m_object->detail().failed())
			m_object->m_path_state		= CMovementManager::ePathStateBuildLevelPath;
		else
			m_object->m_path_state		= CMovementManager::ePathStatePathVerification;
	}

	IC		void			remove			()
	{
		if (m_object->m_wait_for_distributed_computation)
			m_object->m_wait_for_distributed_computation	= false;

		Device.remove_from_seq_parallel	(
			fastdelegate::FastDelegate0<>(
				this,
				&CDetailPathBuilder::process
			)
		);
	}
};
