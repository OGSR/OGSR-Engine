//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "../ECore/Editor/EditObject.h"
#include "SceneObject.h"
#include "ELight.h"
#include "SpawnPoint.h"
#include "WayPoint.h"
#include "xr_ini.h"
#include "xr_efflensflare.h"
#include "GroupObject.h"
//----------------------------------------------------

//----------------------------------------------------
BOOL SceneBuilder::ParseLTX(CInifile* pIni, ObjectList& lst, LPCSTR prefix)
{
    return TRUE;
}
//----------------------------------------------------

BOOL SceneBuilder::BuildLTX()
{
	bool bResult	= true;
	int objcount 	= Scene->ObjCount();
	if( objcount <= 0 ) return true;

	xr_string ltx_filename	= MakeLevelPath("level.ltx");

    if (FS.exist(ltx_filename.c_str()))
    	EFS.MarkFile(ltx_filename.c_str(),true);

	// -- defaults --           
    IWriter* F		= FS.w_open(ltx_filename.c_str());
    if (F){
    	F->w_string("[map_usage]");
        if(Scene->m_LevelOp.m_bDeathmatch)
        	F->w_string("deathmatch");
        if(Scene->m_LevelOp.m_bTeamDeathmatch)
        	F->w_string("teamdeathmatch");
        if(Scene->m_LevelOp.m_bArtefacthunt)
        	F->w_string("artefacthunt");

        F->w_string( ";");
        F->w_string( "; level script file");
        if(Scene->m_LevelOp.m_BOPText.size())
            F->w_stringZ( Scene->m_LevelOp.m_BOPText );

        FS.w_close	(F);
    }else{
    	bResult 	= false;
    }

	return bResult;
}

