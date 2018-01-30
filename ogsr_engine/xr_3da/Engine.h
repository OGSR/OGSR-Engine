// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "engineAPI.h"
#include "eventAPI.h"
#include "xrSheduler.h"

class ENGINE_API CEngine
{
public:
	// DLL api stuff
	CEngineAPI			External;
	CEventAPI			Event;
	CSheduler			Sheduler;

	void				Initialize	();
	void				Destroy		();
	
	CEngine() = default;
	~CEngine() = default;
};

ENGINE_API extern CEngine			Engine;
