#include "stdafx.h"


#include "Blender_Editor_Wire.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Editor_Wire::CBlender_Editor_Wire()
{
    description.CLS = B_EDITOR_WIRE;
    xr_strcpy(oT_Factor, "$null");
}

CBlender_Editor_Wire::~CBlender_Editor_Wire() {}

void CBlender_Editor_Wire::Save(IWriter& fs)
{
    IBlender::Save(fs);
    xrPWRITE_PROP(fs, "TFactor", xrPID_CONSTANT, oT_Factor);
}

void CBlender_Editor_Wire::Load(IReader& fs, u16 version)
{
    IBlender::Load(fs, version);
    xrPREAD_PROP(fs, xrPID_CONSTANT, oT_Factor);
}

void CBlender_Editor_Wire::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    C.r_Pass("editor", "simple_color", FALSE, TRUE, TRUE);
    C.r_End();
}
