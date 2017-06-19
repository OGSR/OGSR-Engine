#include "stdafx.h"
#pragma hdrstop

#include "EScenePSTools.h"
#include "ui_leveltools.h"
#include "EScenePSControls.h"
#include "FramePS.h"
#include "EParticlesObject.h"

void EScenePSTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlPSAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraPS>((TComponent*)0);
}
//----------------------------------------------------
 
void EScenePSTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

CCustomObject* EScenePSTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<EParticlesObject>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

