////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const CALifeSpawnHeader& CALifeSpawnRegistry::header() const { return (m_header); }

//IC void CALifeSpawnRegistry::assign_artefact_position(CSE_ALifeAnomalousZone* anomaly, CSE_ALifeDynamicObject* object) const
//{
//    object->m_tGraphID = anomaly->m_tGraphID;
//    VERIFY3(anomaly->m_artefact_spawn_count, "Anomaly is outside of the AI-map but is used for artefact generation : ", anomaly->name_replace());
//    u32 index = anomaly->m_artefact_position_offset + anomaly->randI(anomaly->m_artefact_spawn_count);
//    object->o_Position = m_artefact_spawn_positions[index].level_point();
//    object->m_tNodeID = m_artefact_spawn_positions[index].level_vertex_id();
//    object->m_fDistance = m_artefact_spawn_positions[index].distance();
//}

IC const CALifeSpawnRegistry::SPAWN_GRAPH& CALifeSpawnRegistry::spawns() const { return (m_spawns); }

IC void CALifeSpawnRegistry::process_spawns(SPAWN_IDS& spawns)
{
    std::sort(spawns.begin(), spawns.end());
    spawns.erase(std::unique(spawns.begin(), spawns.end()), spawns.end());
}
