#pragma once

enum	GAME_TYPE
{
	GAME_UNKNOWN			= 0	,
	GAME_SINGLE					,
	GAME_DEATHMATCH				,
	GAME_TEAMDEATHMATCH			,
	GAME_ARTEFACTHUNT			,

	GAME_END_LIST				,
};
/*
enum EGameTypes
{
	GAME_ANY							= 0,
	GAME_SINGLE							= 1,
	GAME_DEATHMATCH						= 2,
	//	GAME_CTF							= 3,
	//	GAME_ASSAULT						= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS								= 5,
	GAME_TEAMDEATHMATCH					= 6,
	GAME_ARTEFACTHUNT					= 7,

	//identifiers in range [100...254] are registered for script game type
	GAME_DUMMY							= 255	// temporary game type
};
*/
enum STALKER_GAME_TYPE
{
	ST_GAME_ANY							= 0,
	ST_GAME_SINGLE							= 1,
	ST_GAME_DEATHMATCH						= 2,
	//	GAME_CTF							= 3,
	//	GAME_ASSAULT						= 4,	// Team1 - assaulting, Team0 - Defending
	ST_GAME_CS								= 5,
	ST_GAME_TEAMDEATHMATCH					= 6,
	ST_GAME_ARTEFACTHUNT					= 7,

	//identifiers in range [100...254] are registered for script game type
	ST_GAME_DUMMY							= 255	// temporary game type
};
#define EGameTypes	STALKER_GAME_TYPE

static LPSTR	g_GameTypeName	[]		= {
	"UNKNOWN"					,
	"Single"					,	
	"Deathmatch"				,
	"TeamDeathmatch"			,
	"ArtefactHunt"				,

	"END_LIST"					,
};