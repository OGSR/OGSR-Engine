#include "stdafx.h"

#include "blender_blur.h"

CBlender_blur::CBlender_blur() { description.CLS = 0; }

CBlender_blur::~CBlender_blur()
{
}

void CBlender_blur::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:	//Fullres Horizontal
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 1:	//Fullres Vertical
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_blur_h_2);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 2: //Halfres Horizontal
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 3: //Halfres Vertical
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_blur_h_4);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;		
	case 4: //Quarterres Horizontal
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 5: //Quarterres Vertical
		C.r_Pass("stub_screen_space", "pp_blur", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_blur_h_8);
		C.r_dx10Texture("s_position", r2_RT_P);		
		C.r_dx10Texture("s_lut_atlas", "shaders\\lut_atlas");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;				
	}
}
