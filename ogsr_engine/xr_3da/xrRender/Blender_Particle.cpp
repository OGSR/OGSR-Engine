#include "stdafx.h"
#pragma hdrstop

#include "Blender_Particle.h"

#define					oBlendCount	6

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Particle::CBlender_Particle()
{
	description.CLS		= B_PARTICLE;
	description.version	= 0;
	oBlend.Count		= oBlendCount;
	oBlend.IDselected	= 0;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oClamp.value		= TRUE;
}

CBlender_Particle::~CBlender_Particle()
{
	
}

void	CBlender_Particle::Save	( IWriter& fs	)
{
	IBlender::Save	(fs);

	// Blend mode
	xrP_TOKEN::Item	I;
	xrPWRITE_PROP	(fs,"Blending",	xrPID_TOKEN,     oBlend);
	I.ID = 0; strcpy(I.str,"SET");			fs.w		(&I,sizeof(I));
	I.ID = 1; strcpy(I.str,"BLEND");		fs.w		(&I,sizeof(I));
	I.ID = 2; strcpy(I.str,"ADD");			fs.w		(&I,sizeof(I));
	I.ID = 3; strcpy(I.str,"MUL");			fs.w		(&I,sizeof(I));
	I.ID = 4; strcpy(I.str,"MUL_2X");		fs.w		(&I,sizeof(I));
	I.ID = 5; strcpy(I.str,"ALPHA-ADD");	fs.w		(&I,sizeof(I));
	
	// Params
	xrPWRITE_PROP		(fs,"Texture clamp",xrPID_BOOL,		oClamp);
	xrPWRITE_PROP		(fs,"Alpha ref",	xrPID_INTEGER,	oAREF);
}

void	CBlender_Particle::Load	( IReader& fs, u16 version)
{
	IBlender::Load		(fs,version);

	xrPREAD_PROP		(fs,xrPID_TOKEN,		oBlend);	oBlend.Count =   oBlendCount;
	xrPREAD_PROP		(fs,xrPID_BOOL,			oClamp);
	xrPREAD_PROP		(fs,xrPID_INTEGER,		oAREF);
}

#if RENDER==R_R1
void	CBlender_Particle::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	switch (oBlend.IDselected)
	{
		case 0:	C.r_Pass	("particle","particle",TRUE,	TRUE,TRUE,	FALSE,	D3DBLEND_ONE,		D3DBLEND_ZERO,			TRUE,200);	break;	// SET
		case 1: C.r_Pass	("particle","particle",FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,0);	break;	// BLEND
		case 2:	C.r_Pass	("particle","particle",FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_ONE,		D3DBLEND_ONE,			TRUE,0);	break;	// ADD
		case 3:	C.r_Pass	("particle","particle",FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,			TRUE,0);	break;	// MUL
		case 4:	C.r_Pass	("particle","particle",FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_SRCCOLOR,		TRUE,0);	break;	// MUL_2X
		case 5:	C.r_Pass	("particle","particle",FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_SRCALPHA,	D3DBLEND_ONE,			TRUE,0);	break;	// ALPHA-ADD
	}
	C.r_Sampler				("s_base",	C.L_textures[0],false,oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
	C.r_End					();
}	
#else
void	CBlender_Particle::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	switch	(C.iElement) 
	{
	case SE_R2_NORMAL_HQ: 	// deffer
	case SE_R2_NORMAL_LQ: 	// deffer
		switch (oBlend.IDselected)
		{
		case 0:	C.r_Pass	("deffer_particle",	"deffer_particle",	FALSE,	TRUE,TRUE,	FALSE,	D3DBLEND_ONE,		D3DBLEND_ZERO,			FALSE,200);	break;	// SET
		case 1: C.r_Pass	("particle",		"particle",			FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,0);	break;	// BLEND
		case 2:	C.r_Pass	("particle",		"particle",			FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_ONE,		D3DBLEND_ONE,			TRUE,0);	break;	// ADD
		case 3:	C.r_Pass	("particle",		"particle",			FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,			TRUE,0);	break;	// MUL
		case 4:	C.r_Pass	("particle",		"particle",			FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_SRCCOLOR,		TRUE,0);	break;	// MUL_2X
		case 5:	C.r_Pass	("particle",		"particle",			FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_SRCALPHA,	D3DBLEND_ONE,			TRUE,0);	break;	// ALPHA-ADD
		};
		C.r_Sampler			("s_base",	C.L_textures[0],false,oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
		C.r_Sampler_rtf		("s_position",	"$user$position"); // KD: for soft particles
		C.r_End				();
		break;
	case SE_R2_SHADOW:		// smap
		// HARD or SOFT: shadow-map
		switch (oBlend.IDselected)
		{
		case 0:	C.r_Pass	("particle",		"particle",			FALSE,	TRUE,TRUE,	FALSE,	D3DBLEND_ONE,		D3DBLEND_ZERO,	TRUE,200);	break;	// SET
		case 1: C.r_Pass	("particle-clip",	"particle_s-blend",	FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,	TRUE,0);	break;	// BLEND
		case 2:	C.r_Pass	("particle-clip",	"particle_s-add",	FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,	TRUE,0);	break;	// ADD
		case 3:	C.r_Pass	("particle-clip",	"particle_s-mul",	FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,	TRUE,0);	break;	// MUL
		case 4:	C.r_Pass	("particle-clip",	"particle_s-mul",	FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,	TRUE,0);	break;	// MUL_2X
		case 5:	C.r_Pass	("particle-clip",	"particle_s-aadd",	FALSE,	TRUE,FALSE,	TRUE,	D3DBLEND_DESTCOLOR,	D3DBLEND_ZERO,	TRUE,0);	break;	// ALPHA-ADD
		};
		C.r_Sampler			("s_base",	C.L_textures[0],false,oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
		C.r_Sampler_rtf		("s_position",	"$user$position"); // KD: for soft particles
		C.r_End				();
		break;
	case 4: 	// deffer-EMAP
		break;
	};
}
#endif
