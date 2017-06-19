#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"

#include "ESceneDummyTools.h"

#include "ESceneAIMapTools.h"
#include "ESceneDOTools.h"
#include "ESceneSoundSrcTools.h"
#include "ESceneSoundEnvTools.h"
#include "ESceneGroupTools.h"
#include "ESceneLightTools.h"
#include "ESceneObjectTools.h"
#include "EScenePortalTools.h"
#include "ESceneSectorTools.h"
#include "ESceneGlowTools.h"
#include "EScenePSTools.h"
#include "ESceneShapeTools.h"
#include "ESceneSpawnTools.h"
#include "ESceneWayTools.h"
#include "ESceneWallmarkTools.h"

void EScene::RegisterSceneTools(ESceneCustomMTools* mt)
{
    m_SceneTools[mt->ClassID]= mt;
    mt->OnCreate			();
}

void EScene::CreateSceneTools()
{
    RegisterSceneTools	   	(xr_new<ESceneDummyTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneObjectTools>	() 	);
    RegisterSceneTools	   	(xr_new<ESceneLightTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneSoundSrcTools>()	);
    RegisterSceneTools	   	(xr_new<ESceneSoundEnvTools>()	);
	RegisterSceneTools	   	(xr_new<ESceneGroupTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneShapeTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneGlowTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneSpawnTools>	()	);
    RegisterSceneTools	   	(xr_new<ESceneWayTools>		()	);
    RegisterSceneTools	   	(xr_new<ESceneSectorTools>	()	);
    RegisterSceneTools	   	(xr_new<EScenePortalTools>	()	);
    RegisterSceneTools	   	(xr_new<EScenePSTools>		()	);
    RegisterSceneTools	   	(xr_new<EDetailManager>		()	);
    RegisterSceneTools	   	(xr_new<ESceneAIMapTools>	()	);
    RegisterSceneTools		(xr_new<ESceneWallmarkTools>()	);
}

void EScene::DestroySceneTools()
{
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){	
    	_I->second->OnDestroy();
    	xr_delete(_I->second);
    }
    m_SceneTools.clear		();
}

