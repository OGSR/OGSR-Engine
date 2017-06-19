#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"
#include "../ECore/Editor/ui_main.h"
#include "ui_levelmain.h"
#include "scene.h"

#define CHUNK_TOOLS_TAG	0x7777

ESceneCustomMTools::ESceneCustomMTools(ObjClassID cls)
{
    ClassID				= cls;
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
    m_EditFlags.assign	(flEnable);
    m_ModifName			= "";
    m_ModifTime			= 0;
}

ESceneCustomMTools::~ESceneCustomMTools()
{
}

void ESceneCustomMTools::Clear(bool bSpecific)
{
    m_ModifName			= "";
    m_ModifTime			= 0;
}

void ESceneCustomMTools::Reset()
{
	Clear				();
    m_EditFlags.set		(flReadonly,FALSE);
}
 
void ESceneCustomMTools::OnCreate()
{
    OnDeviceCreate		();
    CreateControls		();
}
void ESceneCustomMTools::OnDestroy()
{
    OnDeviceDestroy		();
    RemoveControls		();
}

bool ESceneCustomMTools::Load(IReader& F)
{
	if (F.find_chunk(CHUNK_TOOLS_TAG)){
	    F.r_stringZ	(m_ModifName);
    	F.r			(&m_ModifTime,sizeof(m_ModifTime));
    }else{
	    m_ModifName	= "";
    	m_ModifTime	= 0;
    }
    return true;
}

void ESceneCustomMTools::Save(IWriter& F)
{
    xr_string mn	= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    time_t mt		= time(NULL);
    
	F.open_chunk	(CHUNK_TOOLS_TAG);
	F.w_stringZ		(mn);
	F.w				(&mt,sizeof(mt));
    F.close_chunk	();
}

