#include "stdafx.h"
#pragma hdrstop

#include "ESceneWayTools.h"
#include "WayPoint.h"

CCustomObject* ESceneWayTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CWayObject>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

