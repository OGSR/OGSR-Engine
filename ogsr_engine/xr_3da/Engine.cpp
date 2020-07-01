// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cpuid.h"
#include "Engine.h"

CEngine				Engine;

extern	void msCreate		(LPCSTR name);

void CEngine::Initialize()
{
/*
// TTAPI больше нигде не используется. См. ParticleEffect.cpp и
// particle_actions_collection.cpp
	//
	u32 th_count = CPU::ID.threadCount;
	// Check for override from command line
	constexpr char* szSearchFor = "-max-threads";
	char* pszTemp = strstr(Core.Params, szSearchFor);
	u32 dwOverride = 0;
	if (pszTemp)
		if (sscanf_s(pszTemp + strlen(szSearchFor), "%u", &dwOverride))
			if ((dwOverride >= 1) && (dwOverride <= CPU::ID.threadCount))
				th_count = dwOverride;
	TTAPI->initialize(th_count, "TTAPI thread");
	R_ASSERT(TTAPI->threads.size());
	Msg("TTAPI number of threads: [%zi]", TTAPI->threads.size());
*/
	//
	Engine.Sheduler.Initialize			( );

#ifdef DEBUG
	msCreate							("game");
#endif
}

void CEngine::Destroy()
{
	Engine.Sheduler.Destroy();
	Engine.External.Destroy();
}
