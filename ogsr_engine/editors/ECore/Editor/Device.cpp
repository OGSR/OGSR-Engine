//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop
#include "gamefont.h"
#include "dxerr.h"
#include "ImageManager.h"
#include "ui_main.h"
#include "render.h"
#include "GameMtlLib.h"
#include "ResourceManager.h"

#pragma package(smart_init)

CRenderDevice 		Device;

extern int	rsDVB_Size;
extern int	rsDIB_Size;

ENGINE_API BOOL g_bRendering = FALSE; 

//---------------------------------------------------------------------------
CRenderDevice::CRenderDevice()
{
	psDeviceFlags.assign(rsStatistic|rsFilterLinear|rsFog|rsDrawGrid);
// dynamic buffer size
	rsDVB_Size		= 2048;
	rsDIB_Size		= 2048;
// default initialization
    m_ScreenQuality = 1.f;
    dwWidth 		= dwHeight 	= 256;
    m_RealWidth 	= m_RealHeight 		= 256;
    m_RenderWidth_2	= m_RenderHeight_2 	= 128;
	mProjection.identity();
    mFullTransform.identity();
    mView.identity	();
	m_WireShader	= 0;
	m_SelectionShader = 0;

    b_is_Ready 			= FALSE;
	b_is_Active			= FALSE;

	// Engine flow-control
	fTimeDelta		= 0;
	fTimeGlobal		= 0;
	dwTimeDelta		= 0;
	dwTimeGlobal	= 0;

	dwFillMode		= D3DFILL_SOLID;
    dwShadeMode		= D3DSHADE_GOURAUD;

    m_CurrentShader	= 0;
    pSystemFont		= 0;

	fASPECT 		= 1.f;
	fFOV 			= 60.f;
    dwPrecacheFrame = 0;
}

CRenderDevice::~CRenderDevice(){
	VERIFY(!b_is_Ready);
}

extern void Surface_Init();
void CRenderDevice::Initialize()
{
//	m_Camera.Reset();

    m_DefaultMat.set(1,1,1);
	Surface_Init();

	// game materials
    GMLib.Load	();

	// compiler shader
    string_path fn;
    FS.update_path(fn,_game_data_,"shaders_xrlc.xr");
    if (FS.exist(fn)){
    	ShaderXRLC.Load(fn);
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn);
    }

	// Startup shaders
	Create				();

    ::Render->Initialize();
}

void CRenderDevice::ShutDown()
{
    ::Render->ShutDown	();

	ShaderXRLC.Unload	();
	GMLib.Unload		();

	// destroy context
	Destroy				();
	xr_delete			(pSystemFont);

	// destroy shaders
//	PSLib.xrShutDown	();
}

void CRenderDevice::InitTimer(){
	Timer_MM_Delta	= 0;
	{
		u32 time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		u32 time_system		= timeGetTime	();
		u32 time_local		= TimerAsync	();
		Timer_MM_Delta			= time_system-time_local;
	}
}
//---------------------------------------------------------------------------
void CRenderDevice::RenderNearer(float n){
    mProjection._43=m_fNearer-n;
    RCache.set_xform_project(mProjection);
}
void CRenderDevice::ResetNearer(){
    mProjection._43=m_fNearer;
    RCache.set_xform_project(mProjection);
}
//---------------------------------------------------------------------------
bool CRenderDevice::Create()
{
	if (b_is_Ready)	return false;
    Statistic			= xr_new<CStats>();
	ELog.Msg(mtInformation,"Starting RENDER device...");

	HW.CreateDevice		(m_hRenderWnd/*,dwWidth,dwHeight*/);

	// after creation
	dwFrame				= 0;

	string_path 		sh;
    FS.update_path		(sh,_game_data_,"shaders.xr");

    IReader* F			= 0;
	if (FS.exist(sh))
		F				= FS.r_open(0,sh);
	Resources			= xr_new<CResourceManager>	();

    // if build options - load textures immediately
    if (strstr(Core.Params,"-build")||strstr(Core.Params,"-ebuild"))
        Device.Resources->DeferredLoad(FALSE);

    _Create				(F);
	FS.r_close			(F);

	ELog.Msg			(mtInformation, "D3D: initialized");

	return true;
}

//---------------------------------------------------------------------------
void CRenderDevice::Destroy(){
	if (!b_is_Ready) return;

	ELog.Msg( mtInformation, "Destroying Direct3D...");

	HW.Validate			();

	// before destroy
	_Destroy			(FALSE);
	xr_delete			(Resources);

	// real destroy
	HW.DestroyDevice	();

	ELog.Msg( mtInformation, "D3D: device cleared" );
    xr_delete			(Statistic);
}
//---------------------------------------------------------------------------
void CRenderDevice::_SetupStates()
{
	HW.Caps.Update();
	for (u32 i=0; i<HW.Caps.raster.dwStages; i++){
		float fBias = -1.f;
		CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
	}
	Device.SetRS(D3DRS_DITHERENABLE,	TRUE				);
    Device.SetRS(D3DRS_COLORVERTEX,		TRUE				);
    Device.SetRS(D3DRS_STENCILENABLE,	FALSE				);
    Device.SetRS(D3DRS_ZENABLE,			TRUE				);
    Device.SetRS(D3DRS_SHADEMODE,		D3DSHADE_GOURAUD	);
	Device.SetRS(D3DRS_CULLMODE,		D3DCULL_CCW			);
	Device.SetRS(D3DRS_ALPHAFUNC,		D3DCMP_GREATER		);
	Device.SetRS(D3DRS_LOCALVIEWER,		TRUE				);
    Device.SetRS(D3DRS_NORMALIZENORMALS,TRUE				);

	Device.SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	);

    ResetMaterial();
}
//---------------------------------------------------------------------------
void CRenderDevice::_Create(IReader* F)
{
	b_is_Ready				= TRUE;

	// General Render States
    _SetupStates		();
    
    RCache.OnDeviceCreate		();
	Resources->OnDeviceCreate	(F);
    ::Render->OnDeviceCreate	();

    m_WireShader.create			("editor\\wire");
    m_SelectionShader.create	("editor\\selection");

	// signal another objects
    UI->OnDeviceCreate			();           
//.	seqDevCreate.Process		(rp_DeviceCreate);

	pSystemFont					= xr_new<CGameFont>("hud_font_small");
//	pSystemFont					= xr_new<CGameFont>("hud_font_medium");
}

void CRenderDevice::_Destroy(BOOL	bKeepTextures)
{
	xr_delete					(pSystemFont);

	b_is_Ready 						= FALSE;
    m_CurrentShader				= 0;

    UI->OnDeviceDestroy			();

	m_WireShader.destroy		();
	m_SelectionShader.destroy	();

//.	seqDevDestroy.Process		(rp_DeviceDestroy);
	::Render->Models->OnDeviceDestroy	();

	Resources->OnDeviceDestroy	(bKeepTextures);

	RCache.OnDeviceDestroy		();
    ::Render->OnDeviceDestroy	();
}

//---------------------------------------------------------------------------
void __fastcall CRenderDevice::Resize(int w, int h)
{
    m_RealWidth 	= w;
    m_RealHeight 	= h;
    m_RenderArea	= w*h;

    dwWidth  		= m_RealWidth * m_ScreenQuality;
    dwHeight 		= m_RealHeight * m_ScreenQuality;
    m_RenderWidth_2 = dwWidth * 0.5f;
    m_RenderHeight_2= dwHeight * 0.5f;

    fASPECT 		= (float)dwHeight / (float)dwWidth;
    mProjection.build_projection( deg2rad(fFOV), fASPECT, m_Camera.m_Znear, m_Camera.m_Zfar );
    m_fNearer 		= mProjection._43;

    Reset			();

    RCache.set_xform_project(mProjection);
    RCache.set_xform_world	(Fidentity);

    UI->RedrawScene	();
}

void CRenderDevice::Reset  	()
{
    u32 tm_start			= TimerAsync();
    Resources->reset_begin	();
    Memory.mem_compact		();
    HW.DevPP.BackBufferWidth= dwWidth;
    HW.DevPP.BackBufferHeight= dwHeight;
    HW.Reset				(m_hRenderWnd);
    dwWidth					= HW.DevPP.BackBufferWidth;
    dwHeight				= HW.DevPP.BackBufferHeight;
    m_RenderWidth_2 		= dwWidth * 0.5f;
    m_RenderHeight_2		= dwHeight * 0.5f;
//		fWidth_2			= float(dwWidth/2);
//		fHeight_2			= float(dwHeight/2);
    Resources->reset_end	();
    _SetupStates			();
    u32 tm_end				= TimerAsync();
    Msg						("*** RESET [%d ms]",tm_end-tm_start);
}

BOOL CRenderDevice::Begin	()
{
	VERIFY(b_is_Ready);
	HW.Validate		();
	HRESULT	_hr		= HW.pDevice->TestCooperativeLevel();
    if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if		(D3DERR_DEVICELOST==_hr)		{
			Sleep	(33);
			return	FALSE;
		}

		// Check if the device is ready to be reset
		if		(D3DERR_DEVICENOTRESET==_hr)
		{
			Reset	();
		}
	}

    VERIFY 					(FALSE==g_bRendering);
	CHK_DX					(HW.pDevice->BeginScene());
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		EPrefs->scene_clear_color,1,0
		));
	RCache.OnFrameBegin		();
	g_bRendering = 	TRUE;
	return		TRUE;
}

//---------------------------------------------------------------------------
void CRenderDevice::End()
{
	VERIFY(HW.pDevice);
	VERIFY(b_is_Ready);

    seqRender.Process						(rp_Render);
    
	Statistic->Show(pSystemFont);
	Device.SetRS	(D3DRS_FILLMODE,D3DFILL_SOLID);
	pSystemFont->OnRender();
    Device.SetRS	(D3DRS_FILLMODE,Device.dwFillMode);

	g_bRendering = 	FALSE;

	// end scene
	RCache.OnFrameEnd();
    CHK_DX(HW.pDevice->EndScene());

	CHK_DX(HW.pDevice->Present( NULL, NULL, NULL, NULL ));
}

void CRenderDevice::UpdateView()
{
// set camera matrix
	m_Camera.GetView(mView);

    RCache.set_xform_view(mView);
    mFullTransform.mul(mProjection,mView);

// frustum culling sets
    ::Render->ViewBase.CreateFromMatrix(mFullTransform,FRUSTUM_P_ALL);
}

void CRenderDevice::FrameMove()
{
	dwFrame++;

	// Timer
    float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
    fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;			// smooth random system activity - worst case ~7% error
    if (fTimeDelta>.1f) fTimeDelta=.1f;									// limit to 15fps minimum

    fTimeGlobal		= TimerGlobal.GetElapsed_sec(); //float(qTime)*CPU::cycles2seconds;
    dwTimeGlobal	= TimerGlobal.GetElapsed_ms	();	//u32((qTime*u64(1000))/CPU::cycles_per_second);
    dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);
    dwTimeContinual	= dwTimeGlobal;

    m_Camera.Update(fTimeDelta);

    // process objects
	seqFrame.Process(rp_Frame);
}

void CRenderDevice::DP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 vBase, u32 pc)
{
	ref_shader S 			= m_CurrentShader?m_CurrentShader:m_WireShader;
    u32 dwRequired			= S->E[0]->passes.size();
    RCache.set_Geometry		(geom);
    for (u32 dwPass = 0; dwPass<dwRequired; dwPass++){
    	RCache.set_Shader	(S,dwPass);
		RCache.Render		(pt,vBase,pc);
    }
}

void CRenderDevice::DIP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC)
{
	ref_shader S 			= m_CurrentShader?m_CurrentShader:m_WireShader;
    u32 dwRequired			= S->E[0]->passes.size();
    RCache.set_Geometry		(geom);
    for (u32 dwPass = 0; dwPass<dwRequired; dwPass++){
    	RCache.set_Shader	(S,dwPass);
		RCache.Render		(pt,baseV,startV,countV,startI,PC);
    }
}

void CRenderDevice::ReloadTextures()
{
	UI->SetStatus("Reload textures...");
	Resources->ED_UpdateTextures(0);
	UI->SetStatus("");
}

void CRenderDevice::UnloadTextures()
{
#ifndef _EDITOR
    Resources->DeferredUnload();
#endif    
}

void CRenderDevice::Reset(IReader* F, BOOL bKeepTextures)
{
	CTimer tm;
    tm.Start();
	_Destroy		(bKeepTextures);
	_Create			(F);
	Msg				("*** RESET [%d ms]",tm.GetElapsed_ms());
}


