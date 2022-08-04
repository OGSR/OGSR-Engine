#pragma once

enum EPointType
{
    ptRPoint = 0,
    ptEnvMod,
    ptSpawnPoint,
    ptMaxType,
    pt_force_dword = u32(-1)
};

enum EWayType
{
    wtPatrolPath = 0,
    wtMaxType,
    wt_force_dword = u32(-1)
};

enum ERPpointType
{ // [0..255]
    rptActorSpawn = 0,
    rptArtefactSpawn,
    rptItemSpawn,
};

enum EEnvModUsedParams
{
    eViewDist = (1 << 0),
    eFogColor = (1 << 1),
    eFogDensity = (1 << 2),
    eAmbientColor = (1 << 3),
    eSkyColor = (1 << 4),
    eHemiColor = (1 << 5)
};

// BASE offset
#define WAY_BASE 0x1000
#define POINT_BASE 0x2000

// POINT chunks
#define RPOINT_CHUNK POINT_BASE + ptRPoint

// WAY chunks
#define WAY_PATROLPATH_CHUNK WAY_BASE + wtPatrolPath
//----------------------------------------------------

#define WAYOBJECT_VERSION 0x0013
//----------------------------------------------------
#define WAYOBJECT_CHUNK_VERSION 0x0001
#define WAYOBJECT_CHUNK_POINTS 0x0002
#define WAYOBJECT_CHUNK_LINKS 0x0003
#define WAYOBJECT_CHUNK_TYPE 0x0004
#define WAYOBJECT_CHUNK_NAME 0x0005

#define NPC_POINT_VERSION 0x0001
//----------------------------------------------------
#define NPC_POINT_CHUNK_VERSION 0x0001
#define NPC_POINT_CHUNK_DATA 0x0002
//----------------------------------------------------
