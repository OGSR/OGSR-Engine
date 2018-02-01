// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine				Engine;

extern	void msCreate		(LPCSTR name);

void CEngine::Initialize()
{
	//
#ifdef NEW_TTAPI
	u32 th_count = CPU::ID.threadCount; //CPU::ID.coresCount
	// Check for override from command line
	char szSearchFor[] = "-max-threads";
	char* pszTemp = strstr(GetCommandLine(), szSearchFor);
	u32 dwOverride = 0;
	if (pszTemp)
		if (sscanf_s(pszTemp + strlen(szSearchFor), "%u", &dwOverride))
			if ((dwOverride >= 1) && (dwOverride <= CPU::ID.threadCount))
				th_count = dwOverride;
	TTAPI->initialize(th_count);
	R_ASSERT(TTAPI->threads.size());
	Msg("TTAPI number of threads: [%zi]", TTAPI->threads.size());
#else
	ttapi_Init();
#endif
	//
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
