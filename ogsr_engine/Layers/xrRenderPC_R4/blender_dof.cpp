#include "stdafx.h"

#include "blender_dof.h"

CBlender_dof::CBlender_dof() { description.CLS = 0; }

CBlender_dof::~CBlender_dof()
{
}

void CBlender_dof::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("stub_screen_space", "depth_of_field", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_position", r2_RT_P);
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_blur_2", r2_RT_blur_2);		
	
		C.r_dx10Sampler("smp_base");
		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 1:
		C.r_Pass("stub_screen_space", "post_processing", FALSE, FALSE, FALSE);
		C.r_dx10Texture("samplero_pepero", r2_RT_dof);
	
		C.r_dx10Sampler("smp_base");
		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	}
}
