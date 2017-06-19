#include "stdafx.h"
#pragma hdrstop

#include "ESceneGlowTools.h"
#include "UI_LevelTools.h"
#include "glow.h"

void ESceneGlowTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    m_Flags.zero	();
}
//----------------------------------------------------
 
void ESceneGlowTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneGlowTools::FillProp(LPCSTR pref, PropItemVec& items)
{                      
    PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Test Visibility"),	&m_Flags,	flTestVisibility);
    PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Draw Cross"),		&m_Flags,	flDrawCross);
	inherited::FillProp	(pref, items);
}
//------------------------------------------------------------------------------

CCustomObject* ESceneGlowTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CGlow>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

