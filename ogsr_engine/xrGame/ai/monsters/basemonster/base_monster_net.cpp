#include "stdafx.h"
#include "base_monster.h"

#include "xrserver_objects_alife_monsters.h"
#include "../../../ai_object_location.h"
#include "game_graph.h"
#include "../../../../xr_3da/NET_Server_Trash/net_utils.h"
#include "../../../ai_space.h"
#include "../../../hit.h"
#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"
void CBaseMonster::net_Save(NET_Packet& P)
{
    inherited::net_Save(P);
    m_pPhysics_support->in_NetSave(P);
}

BOOL CBaseMonster::net_SaveRelevant() { return (inherited::net_SaveRelevant() || BOOL(PPhysicsShell() != NULL)); }

void CBaseMonster::net_Export(CSE_Abstract* E)
{
    R_ASSERT(Local());

    // export last known packet
    R_ASSERT(!NET.empty());
    net_update& N = NET.back();

    CSE_ALifeCreatureAbstract* creature = smart_cast<CSE_ALifeCreatureAbstract*>(E);
    creature->fHealth = GetfHealth();
    creature->timestamp = N.dwTimeStamp;
    creature->flags = 0;
    creature->o_Position = N.p_pos;
    creature->o_model = N.o_model;
    creature->o_torso.yaw = N.o_torso.yaw;
    creature->o_torso.pitch = N.o_torso.pitch;
    creature->o_torso.roll = N.o_torso.roll;
    creature->s_team = u8(g_Team());
    creature->s_squad = u8(g_Squad());
    creature->s_group = u8(g_Group());

    CSE_ALifeMonsterAbstract* monster = smart_cast<CSE_ALifeMonsterAbstract*>(E);
    GameGraph::_GRAPH_ID l_game_vertex_id = ai_location().game_vertex_id();
    monster->m_tNextGraphID = l_game_vertex_id;
    monster->m_tPrevGraphID = l_game_vertex_id;
    if (ai().game_graph().valid_vertex_id(l_game_vertex_id))
    {
        monster->m_fDistanceFromPoint = Position().distance_to(ai().game_graph().vertex(l_game_vertex_id)->level_point());
        monster->m_fDistanceToPoint = Position().distance_to(ai().game_graph().vertex(l_game_vertex_id)->level_point());
    }
    else
    {
        monster->m_fDistanceFromPoint = 0;
        monster->m_fDistanceToPoint = 0;
    }
}
