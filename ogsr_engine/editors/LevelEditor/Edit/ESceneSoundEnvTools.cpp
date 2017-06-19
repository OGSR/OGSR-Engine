#include "stdafx.h"
#pragma hdrstop

#include "ESceneSoundEnvTools.h"
#include "SoundManager_LE.h"
#include "UI_LevelTools.h"
#include "ESound_Environment.h"

void ESceneSoundEnvTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
}
//----------------------------------------------------
 
void ESceneSoundEnvTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneSoundEnvTools::Clear(bool bSpecific)
{
	inherited::Clear	(bSpecific);
    LSndLib->RefreshEnvGeometry	();
}
//----------------------------------------------------

CCustomObject* ESceneSoundEnvTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<ESoundEnvironment>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

