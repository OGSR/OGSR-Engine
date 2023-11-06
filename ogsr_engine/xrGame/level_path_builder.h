////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_builder.h
//  Modified 	: 21.02.2005
//  Modified 	: 21.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level path builder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "level_path_manager.h"

class CLevelPathBuilder
{
private:
    CMovementManager* m_object;
    u32 m_start_vertex_id;
    u32 m_dest_vertex_id;

public:
    IC CLevelPathBuilder(CMovementManager* object)
    {
        VERIFY(object);
        m_object = object;
    }

    IC void setup(const u32& start_vertex_id, const u32& dest_vertex_id)
    {
        VERIFY(ai().level_graph().valid_vertex_id(start_vertex_id));
        m_start_vertex_id = start_vertex_id;

        VERIFY(ai().level_graph().valid_vertex_id(dest_vertex_id));
        m_dest_vertex_id = dest_vertex_id;

        m_object->m_wait_for_distributed_computation_2 = true;
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CLevelPathBuilder::process_level));
    }

    void process()
    {
        m_object->level_path().build_path(m_start_vertex_id, m_dest_vertex_id);

        if (m_object->level_path().failed())
            m_object->m_path_state = CMovementManager::ePathStateBuildLevelPath;
        else
            m_object->m_path_state = CMovementManager::ePathStateContinueLevelPath;
    }

    void process_level()
    {
        m_object->m_wait_for_distributed_computation_2 = false;
        process();
    }

    IC void remove()
    {
        m_object->m_wait_for_distributed_computation_2 = false;

        Device.remove_from_seq_parallel(fastdelegate::MakeDelegate(this, &CLevelPathBuilder::process_level));
    }
};
