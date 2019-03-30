#include "stdafx.h"

#include "blender_lut.h"

CBlender_lut::CBlender_lut() { description.CLS = 0; }
CBlender_lut::~CBlender_lut() { }

void CBlender_lut::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("null", "lut", FALSE, FALSE, FALSE);
	C.r_Sampler_clf("s_image", r2_RT_generic0);
	
	C.r_Sampler_clf("s_lut_1", "shaders\\lut_1");
	C.r_Sampler_clf("s_lut_2", "shaders\\lut_2");
	C.r_Sampler_clf("s_lut_3", "shaders\\lut_3");
	C.r_Sampler_clf("s_lut_4", "shaders\\lut_4");
	C.r_Sampler_clf("s_lut_5", "shaders\\lut_5");
	C.r_End();
} 