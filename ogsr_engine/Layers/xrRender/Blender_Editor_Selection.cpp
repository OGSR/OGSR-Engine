#include "stdafx.h"


#include "Blender_Editor_Selection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Editor_Selection::CBlender_Editor_Selection()
{
    description.CLS = B_EDITOR_SEL;
    xr_strcpy(oT_Factor, "$null");
}

CBlender_Editor_Selection::~CBlender_Editor_Selection() {}

void CBlender_Editor_Selection::Save(IWriter& fs)
{
    IBlender::Save(fs);
    xrPWRITE_PROP(fs, "TFactor", xrPID_CONSTANT, oT_Factor);
}

void CBlender_Editor_Selection::Load(IReader& fs, u16 version)
{
    IBlender::Load(fs, version);
    xrPREAD_PROP(fs, xrPID_CONSTANT, oT_Factor);
}

void CBlender_Editor_Selection::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    C.r_Pass("editor", "simple_color", FALSE, TRUE, FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    C.r_End();
}
