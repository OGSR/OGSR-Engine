// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifdef XRENGINE_STATIC
#define DLL_API
#define ENGINE_API
#elif defined ENGINE_BUILD
#define DLL_API __declspec(dllimport)
#define ENGINE_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllexport)
#define ENGINE_API __declspec(dllimport)
#endif

#define ECORE_API

// TODO: this should be in render configuration
#define R__NUM_SUN_CASCADES (3u) // csm/s.ligts
#define R__NUM_AUX_CONTEXTS (6u) // rain/s.lights
#define R__NUM_PARALLEL_CONTEXTS (R__NUM_SUN_CASCADES + R__NUM_AUX_CONTEXTS)
#define R__NUM_CONTEXTS (R__NUM_PARALLEL_CONTEXTS + 1 /* imm */)

#include "engineAPI.h"
#include "eventAPI.h"
#include "xrSheduler.h"

class ENGINE_API CEngine
{
public:
    // DLL api stuff
    CEngineAPI External;
    CEventAPI Event;
    CSheduler Sheduler;

    void Initialize();
    void Destroy();

    CEngine() = default;
    ~CEngine() = default;
};

ENGINE_API extern CEngine Engine;
