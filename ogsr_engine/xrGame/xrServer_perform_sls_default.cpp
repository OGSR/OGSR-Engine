#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"


void xrServer::SLS_Default	()
{
	if (game->custom_sls_default()) {
		game->sls_default	();
		return;
	}

	string_path				fn_spawn;
	if (FS.exist(fn_spawn, "$level$", "level.spawn")) {
		IReader*			SP		= FS.r_open(fn_spawn);
		NET_Packet			P;
		u32					S_id;
		for (IReader *S = SP->open_chunk_iterator(S_id); S; S = SP->open_chunk_iterator(S_id,S)) {
			P.B.count		= S->length();
			S->r			(P.B.data,P.B.count);
			
			u16				ID;
			P.r_begin		(ID);
			R_ASSERT		(M_SPAWN==ID);
			ClientID clientID;clientID.set(0);

			Process_spawn(P,clientID);

		}
		FS.r_close			(SP);
	}
}
