#include "stdafx.h"
#pragma hdrstop

#include "LevelGameDef.h"

xr_token rpoint_type[]={
	{ "Actor Spawn",	rptActorSpawn	},
	{ "Artefact Spawn",	rptArtefactSpawn	},
	{ "TeamBase Particle",	rptTeamBaseParticle	},
	{ 0,				0	}
};

xr_token rpoint_game_type[]={
	{ "Any game",		rpgtGameAny				},
	{ "Deathmatch",		rpgtGameDeathmatch		},
	{ "TeamDeathmatch",	rpgtGameTeamDeathmatch	},
	{ "ArtefactHunt",	rpgtGameArtefactHunt	},
	{ 0,				0	}
};
 