#include "stdafx.h"
#include "xrServer_Objects_ALife_All.h"
#include "level.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "ai_space.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "client_spawn_manager.h"
#include "..\xr_3da\xr_object.h"
#include "..\xr_3da\IGame_Persistent.h"

void CLevel::cl_Process_Spawn(NET_Packet& P)
{
    // Begin analysis
    shared_str s_name;
    P.r_stringZ(s_name);

    // Create DC (xrSE)
    CSE_Abstract* E = F_entity_Create(*s_name);
    R_ASSERT2(E, *s_name);

    E->Spawn_Read(P);
    if (E->s_flags.is(M_SPAWN_UPDATE))
        E->UPDATE_Read(P);
    //-------------------------------------------------
    //	Msg ("M_SPAWN - %s[%d][%x] - %d", *s_name,  E->ID, E,E->ID_Parent);
    //-------------------------------------------------
    // force object to be local for server client
    {
        E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
    };

    if (std::find(m_just_destroyed.begin(), m_just_destroyed.end(), E->ID) != m_just_destroyed.end())
    {
        Msg("* [%s]: skip just destroyed [%s] ID: [%u] ID_Parent: [%u]", __FUNCTION__, E->name_replace(), E->ID, E->ID_Parent);
        m_just_destroyed.erase(std::remove(m_just_destroyed.begin(), m_just_destroyed.end(), E->ID), m_just_destroyed.end());
        F_entity_Destroy(E);
        return;
    }

    if (Device.dwPrecacheFrame == 0)
    {
        CSE_ALifeMonsterAbstract* monster = smart_cast<CSE_ALifeMonsterAbstract*>(E);
        CSE_ALifeTraderAbstract* trader = smart_cast<CSE_ALifeTraderAbstract*>(E);
        bool postpone = ((trader || monster) && !is_removing_objects()) ? true : false;
        if (!(monster || trader))
        {
            for (const auto& it : game_spawn_queue)
            {
                if (E->ID_Parent == it->ID)
                {
                    postpone = true;
                    break;
                }
            }
        }
        if (postpone)
        {
            //Msg( "* [%s]: delay spawn ID[%d] ID_Parent[%d] name_replace[%s]", __FUNCTION__, E->ID, E->ID_Parent, E->name_replace() );
            game_spawn_queue.push_back(E);
            return;
        }
    }

    /*
    game_spawn_queue.push_back(E);
    if (g_bDebugEvents)		ProcessGameSpawns();
    /*/
    g_sv_Spawn(E);

    F_entity_Destroy(E);
    //*/
};

void CLevel::g_cl_Spawn(LPCSTR name, u8 rp, u16 flags, Fvector pos)
{
    // Create
    CSE_Abstract* E = F_entity_Create(name);
    VERIFY(E);

    // Fill
    E->s_name = name;
    E->set_name_replace("");
    E->s_gameid = u8(GameID());
    E->s_RP = rp;
    E->ID = 0xffff;
    E->ID_Parent = 0xffff;
    E->ID_Phantom = 0xffff;
    E->s_flags.assign(flags);
    E->RespawnTime = 0;
    E->o_Position = pos;

    // Send
    NET_Packet P;
    E->Spawn_Write(P, TRUE);
    Send(P, net_flags(TRUE));

    // Destroy
    F_entity_Destroy(E);
}

#ifdef DEBUG
extern Flags32 psAI_Flags;
#include "ai_debug.h"
#endif // DEBUG

void CLevel::g_sv_Spawn(CSE_Abstract* E)
{
    //-----------------------------------------------------------------
    //	CTimer		T(false);

#ifdef DEBUG
    Msg("* CLIENT: Spawn: %s, ID=%d", *E->s_name, E->ID);
#endif

    auto obj = Objects.net_Find(E->ID);
    if (obj && obj->getDestroy())
    {
        Msg("[%s]: %s[%u] already net_Spawn'ed, call ProcessDestroyQueue()", __FUNCTION__, obj->cName().c_str(), obj->ID());
        Objects.ProcessDestroyQueue();
    }

    // Client spawn
    //	T.Start		();
    CObject* O = Objects.Create(*E->s_name);
    // Msg				("--spawn--CREATE: %f ms",1000.f*T.GetAsync());

    //	T.Start		();
    if (0 == O || (!O->net_Spawn(E)))
    {
        O->net_Destroy();
        client_spawn_manager().clear(O->ID());
        Objects.Destroy(O);
        Msg("! Failed to spawn entity '%s'", *E->s_name);
    }
    else
    {
        client_spawn_manager().callback(O);
        // Msg			("--spawn--SPAWN: %f ms",1000.f*T.GetAsync());
        if ((E->s_flags.is(M_SPAWN_OBJECT_LOCAL)) && (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)))
        {
            if (CurrentEntity() != NULL)
            {
                CGameObject* pGO = smart_cast<CGameObject*>(CurrentEntity());
                if (pGO)
                    pGO->On_B_NotCurrentEntity();
            }
            SetEntity(O);
            SetControlEntity(O);
        }

        if (0xffff != E->ID_Parent)
        {
            NET_Packet GEN;
            GEN.write_start();
            GEN.read_start();
            GEN.w_u16(u16(O->ID()));
            cl_Process_Event(E->ID_Parent, GE_OWNERSHIP_TAKE, GEN);
        }
    }
    //---------------------------------------------------------
    Game().OnSpawn(O);
    //---------------------------------------------------------
}

CSE_Abstract* CLevel::spawn_item(LPCSTR section, const Fvector& position, u32 level_vertex_id, u16 parent_id, bool return_item)
{
    CSE_Abstract* abstract = F_entity_Create(section);
    R_ASSERT3(abstract, "Cannot find item with section", section);
    CSE_ALifeDynamicObject* dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(abstract);
    if (dynamic_object && ai().get_level_graph())
    {
        dynamic_object->m_tNodeID = level_vertex_id;
        if (ai().level_graph().valid_vertex_id(level_vertex_id) && ai().get_game_graph() && ai().get_cross_table())
            dynamic_object->m_tGraphID = ai().cross_table().vertex(level_vertex_id).game_vertex_id();
    }

    //оружие спавним с полным магазинои
    CSE_ALifeItemWeapon* weapon = smart_cast<CSE_ALifeItemWeapon*>(abstract);
    if (weapon)
        weapon->a_elapsed = weapon->get_ammo_magsize();

    // Fill
    abstract->s_name = section;
    abstract->set_name_replace(section);
    abstract->s_gameid = u8(GameID());
    abstract->o_Position = position;
    abstract->s_RP = 0xff;
    abstract->ID = 0xffff;
    abstract->ID_Parent = parent_id;
    abstract->ID_Phantom = 0xffff;
    abstract->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
    abstract->RespawnTime = 0;

    if (!return_item)
    {
        NET_Packet P;
        abstract->Spawn_Write(P, TRUE);
        Send(P, net_flags(TRUE));
        F_entity_Destroy(abstract);
        return (0);
    }
    else
        return (abstract);
}


void CLevel::ProcessGameSpawns()
{
    CSE_Abstract* trader = nullptr;
    while (!game_spawn_queue.empty())
    {
        CSE_Abstract* E = game_spawn_queue.front();
        game_spawn_queue.pop_front();
        //Msg( "* [%s]: delayed spawn dwFrame[%u] ID[%d] ID_Parent[%d] name_replace[%s]", __FUNCTION__, Device.dwFrame, E->ID, E->ID_Parent, E->name_replace() );
        g_sv_Spawn(E);
        if (smart_cast<CSE_ALifeMonsterAbstract*>(E) || smart_cast<CSE_ALifeTraderAbstract*>(E))
        {
            trader = E;
            break;
        }
        F_entity_Destroy(E);
    }

    if (trader)
    {
        for (auto& E : game_spawn_queue)
        {
            if (E->ID_Parent == trader->ID)
            {
                // Msg( "* [%s]: delayed spawn dwFrame[%u] trader[%d] ID[%d] ID_Parent[%d] name_replace[%s]", __FUNCTION__, Device.dwFrame, trader->ID, E->ID, E->ID_Parent,
                // E->name_replace() );
                g_sv_Spawn(E);
            }
        }
        game_spawn_queue.erase(std::remove_if(game_spawn_queue.begin(), game_spawn_queue.end(), [&](auto& E) { return E->ID_Parent == trader->ID; }), game_spawn_queue.end());
        F_entity_Destroy(trader);
    }
}

void CLevel::ProcessGameSpawnsDestroy(u16 dest, u16 type, NET_Packet& P)
{
    if (type != GE_DESTROY)
        return;

    game_spawn_queue.erase(std::remove_if(game_spawn_queue.begin(), game_spawn_queue.end(),
                                          [&](auto& E) {
                                                  if (E->ID == dest || E->ID_Parent == dest)
                                                  {
                                                      // Msg( "* [CLevel::ProcessGameSpawnsDestroy]: delayed spawn GE_DESTROY dest[%d] ID[%d] ID_Parent[%d] name_replace[%s]", dest,
                                                      // E->ID, E->ID_Parent, E->name_replace() );
                                                      F_entity_Destroy(E);
                                                      return true;
                                                  }
                                              return false;
                                          }),
                           game_spawn_queue.end());
}
