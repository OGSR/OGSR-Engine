#include "stdafx.h"
#pragma hdrstop

#include "ESceneSoundSrcTools.h"
#include "UI_LevelTools.h"
#include "ESound_Source.h"

void ESceneSoundSrcTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
}
//----------------------------------------------------
 
void ESceneSoundSrcTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

CCustomObject* ESceneSoundSrcTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<ESoundSource>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

