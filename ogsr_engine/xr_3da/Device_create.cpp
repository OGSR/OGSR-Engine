#include "stdafx.h"

#include "resourcemanager.h"
#include "xr_effgamma.h"
#include "render.h"

void CRenderDevice::_SetupStates	()
{
	// General Render States
	mView.identity			();
	mProject.identity		();
	mFullTransform.identity	();
	vCameraPosition.set		(0,0,0);
	vCameraDirection.set	(0,0,1);
	vCameraTop.set			(0,1,0);
	vCameraRight.set		(1,0,0);

	HW.Caps.Update			();
	for (u32 i=0; i<HW.Caps.raster.dwStages; i++)				{
		float fBias = -.5f	;
		CHK_DX(HW.pDevice->SetSamplerState	( i, D3DSAMP_MAXANISOTROPY, 4				));
		CHK_DX(HW.pDevice->SetSamplerState	( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
		CHK_DX(HW.pDevice->SetSamplerState	( i, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR 		));
		CHK_DX(HW.pDevice->SetSamplerState	( i, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR 		));
		CHK_DX(HW.pDevice->SetSamplerState	( i, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR		));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZENABLE,			TRUE				));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_LOCALVIEWER,		TRUE				));

	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,	FALSE			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,		TRUE			));

	if (psDeviceFlags.test(rsWireframe))	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME	)); }
	else									{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		)); }

	// ******************** Fog parameters
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,			0					));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_RANGEFOGENABLE,	FALSE				));
	if (HW.Caps.bTableFog)	{
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR		));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE			));
	} else {
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR		));
	}
}

void CRenderDevice::_Create	(LPCSTR shName)
{
	Memory.mem_compact			();

	// after creation
	b_is_Ready					= TRUE;
	_SetupStates				();

	// Signal everyone - device created
	RCache.OnDeviceCreate		();
	Gamma.Update				();
	Resources->OnDeviceCreate	(shName);
	::Render->create			();
	Statistic->OnDeviceCreate	();

#ifndef DEDICATED_SERVER
	m_WireShader.create			("editor\\wire");
	m_SelectionShader.create	("editor\\selection");

	DU.OnDeviceCreate			();
#endif

	dwFrame						= 0;
}


void CRenderDevice::Create	() 
{
	if (b_is_Ready)		return;		// prevent double call
	Statistic			= xr_new<CStats>();
	Log					("Starting RENDER device...");

#ifdef _EDITOR
	psCurrentVidMode[0]	= dwWidth;
	psCurrentVidMode[1] = dwHeight;
#endif

	HW.CreateDevice		(m_hWnd);
	dwWidth				= HW.DevPP.BackBufferWidth	;
	dwHeight			= HW.DevPP.BackBufferHeight	;
	fWidth_2			= float(dwWidth/2)			;
	fHeight_2			= float(dwHeight/2)			;
	fFOV				= 90.f;
	fASPECT				= 1.f;

	string_path			fname; 
	FS.update_path		(fname,"$game_data$","shaders.xr");

	//////////////////////////////////////////////////////////////////////////
	Resources			= xr_new<CResourceManager>		();
	_Create				(fname);

	PreCache			(0);
}
