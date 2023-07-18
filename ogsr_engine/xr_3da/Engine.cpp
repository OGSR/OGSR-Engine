// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine Engine;

void CEngine::Initialize()
{
    Engine.Sheduler.Initialize();
}

void CEngine::Destroy()
{
    Engine.Sheduler.Destroy();
    Engine.External.Destroy();
}
