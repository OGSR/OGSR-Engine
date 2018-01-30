// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine				Engine;

extern	void msCreate		(LPCSTR name);

void CEngine::Initialize()
{
	TTAPI.initialize();
	R_ASSERT(TTAPI.threads.size());
	Msg("TTAPI number of threads: [%zi]", TTAPI.threads.size());

	Engine.Sheduler.Initialize			( );

#ifdef DEBUG
	msCreate							("game");
#endif
}

void CEngine::Destroy	()
{
	Engine.Sheduler.Destroy				( );
#ifdef DEBUG_MEMORY_MANAGER
	extern void	dbg_dump_leaks_prepare	( );
	if (Memory.debug_mode)				dbg_dump_leaks_prepare	();
#endif // DEBUG_MEMORY_MANAGER
	Engine.External.Destroy				( );
}
