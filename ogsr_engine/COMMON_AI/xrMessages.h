#ifndef _INCDEF_XRMESSAGES_H_
#define _INCDEF_XRMESSAGES_H_

#pragma once

// CL	== client 2 server message
// SV	== server 2 client message

enum
{
    M_UPDATE = 0, // DUAL: Update state
    M_SPAWN, // DUAL: Spawning, full state

    M_SV_CONFIG_NEW_CLIENT,
    M_SV_CONFIG_GAME,
    M_SV_CONFIG_FINISHED,

    M_EVENT, // Game Event
    M_CLIENTREADY, // Client has finished to load level and are ready to play

    M_CHANGE_LEVEL, // changing level
    M_LOAD_GAME,
    M_RELOAD_GAME,
    M_SAVE_GAME,
    M_SAVE_PACKET,

    M_SWITCH_DISTANCE,
    M_EVENT_PACK, // Pack of M_EVENT

    //-----------------------------------------------------
    M_CLIENT_CONNECT_RESULT,
    M_CLIENT_REQUEST_CONNECTION_DATA,

    M_CHANGE_LEVEL_GAME,
    //-----------------------------------------------------
    M_AUTH_CHALLENGE,
    M_CL_AUTH,
    //-----------------------------------------------------
    M_STATISTIC_UPDATE,
    M_STATISTIC_UPDATE_RESPOND,
    //-----------------------------------------------------
    
    M_CHANGE_SELF_NAME,
};

enum
{
    GE_OWNERSHIP_TAKE, // DUAL: Client request for ownership of an item
    GE_OWNERSHIP_REJECT, // DUAL: Client request ownership rejection
    GE_TRANSFER_AMMO, // DUAL: Take ammo out of weapon for our weapon
    GE_HIT, //
    GE_DIE, //
    GE_ASSIGN_KILLER, //
    GE_DESTROY, // authorative client request for entity-destroy
    GE_TELEPORT_OBJECT,

    GE_ADD_RESTRICTION,
    GE_REMOVE_RESTRICTION,
    GE_REMOVE_ALL_RESTRICTIONS,

    GE_INFO_TRANSFER, // transfer _new_ info on PDA

    GE_TRADE_SELL,
    GE_TRADE_BUY,

    GE_WPN_AMMO_ADD,
    GE_WPN_STATE_CHANGE,

    GE_GRENADE_EXPLODE,

    GE_ZONE_STATE_CHANGE,

    GE_CHANGE_POS,

    GE_CHANGE_VISUAL,

    GEG_PLAYER_ACTIVATE_SLOT,
    GEG_PLAYER_ITEM2SLOT,
    GEG_PLAYER_ITEM2BELT,
    GEG_PLAYER_ITEM2RUCK,
    GEG_PLAYER_ITEM_EAT,
    //GEG_PLAYER_ITEM_SELL,
    GEG_PLAYER_ACTIVATEARTEFACT,

    GEG_PLAYER_ATTACH_HOLDER,
    GEG_PLAYER_DETACH_HOLDER,
    
    //-------------------------------------
    GE_KILL_SOMEONE,

    GE_FREEZE_OBJECT,
    GE_LAUNCH_ROCKET,

    GE_TRANSFER_TAKE,
    GE_TRANSFER_REJECT,

    GE_FORCEDWORD = u32(-1)
};

enum EGameMessages
{ // game_cl <----> game_sv messages

    GAME_EVENT_PLAYER_CONNECTED,

    GAME_EVENT_CREATE_CLIENT,
    GAME_EVENT_ON_HIT,

    GAME_EVENT_PLAYER_AUTH,
    
    GAME_EVENT_FORCEDWORD = u32(-1)
};

enum
{
    M_SPAWN_OBJECT_LOCAL = (1 << 0), // after spawn it becomes local (authorative)
    M_SPAWN_OBJECT_HASUPDATE = (1 << 2), // after spawn info it has update inside message
    M_SPAWN_OBJECT_ASPLAYER = (1 << 3), // after spawn it must become viewable
    M_SPAWN_OBJECT_PHANTOM = (1 << 4), // after spawn it must become viewable
    M_SPAWN_VERSION = (1 << 5), // control version
    M_SPAWN_UPDATE = (1 << 6), // + update packet
    M_SPAWN_TIME = (1 << 7), // + spawn time
    M_SPAWN_DENIED = (1 << 8), // don't spawn entity with this flag

    M_SPAWN_OBJECT_FORCEDWORD = u32(-1)
};

#endif /*_INCDEF_XRMESSAGES_H_*/
