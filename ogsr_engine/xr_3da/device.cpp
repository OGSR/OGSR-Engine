#include "stdafx.h"
#include "frustum.h"

#pragma warning(disable:4995)
// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>
// d3dx9.h
#include <d3dx9.h>
#pragma warning(default:4995)

#include "x_ray.h"
#include "render.h"
#include "XR_IOConsole.h"
#include "IGame_Persistent.h"
#include "xr_input.h"

ENGINE_API CRenderDevice Device;
ENGINE_API BOOL g_bRendering = FALSE; 
u32 g_dwFPSlimit = 60;

BOOL		g_bLoaded = FALSE;
ref_light	precache_light = 0;

BOOL CRenderDevice::Begin	()
{
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

	CHK_DX					(HW.pDevice->BeginScene());
	RCache.OnFrameBegin		();
	RCache.set_CullMode		(CULL_CW);
	RCache.set_CullMode		(CULL_CCW);
	if (HW.Caps.SceneMode)	overdrawBegin	();
	FPU::m24r	();
	g_bRendering = 	TRUE;
	return		TRUE;
}

void CRenderDevice::Clear	()
{
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		(psDeviceFlags.test(rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,0,0),1,0
		));
}

void xrRender_apply_tf()
{
	Console->Execute("r__tf_aniso");
	Console->Execute("r2_tf_mipbias");
}

#include "resourcemanager.h"

void CRenderDevice::End		(void)
{
	VERIFY	(HW.pDevice);

	if (HW.Caps.SceneMode)	overdrawEnd		();

	// 
	if (dwPrecacheFrame)
	{
		::Sound->set_master_volume	(0.f);
		dwPrecacheFrame	--;
		pApp->load_draw_internal	();
		if (0==dwPrecacheFrame)
		{
			Gamma.Update		();

			if(precache_light) precache_light->set_active	(false);
			if(precache_light) precache_light.destroy		();
			::Sound->set_master_volume						(1.f);
			pApp->destroy_loading_shaders					();
			Resources->DestroyNecessaryTextures				();
			Memory.mem_compact								();
			Msg												("* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
			xrRender_apply_tf(); //KRodin: вот это надо делать именно здесь, и нигде больше! Иначе не работает.
		}
	}

	g_bRendering		= FALSE;
	// end scene
	RCache.OnFrameEnd	();
    CHK_DX(HW.pDevice->EndScene());

	if (!Device.m_SecondViewport.IsSVPFrame() && !Device.m_SecondViewport.m_bCamReady) //--#SM+#-- +SecondVP+ Не выводим кадр из второго вьюпорта на экран (на практике у нас экранная картинка обновляется минимум в два раза реже) [don't flush image into display for SecondVP-frame]
		HW.pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}


volatile u32	mt_Thread_marker		= 0x12345678;
void mt_Thread()
{
	set_current_thread_name("X-RAY Secondary thread");
	//
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//
	while (true) {
		// waiting for Device permission to execute
		Device.mt_csEnter.Enter	();

		if (Device.mt_bMustExit) {
			Device.mt_bMustExit = FALSE;				// Important!!!
			Device.mt_csEnter.Leave();					// Important!!!
			return;
		}
		// we has granted permission to execute
		mt_Thread_marker			= Device.dwFrame;
 
		for (u32 pit=0; pit<Device.seqParallel.size(); pit++)
			Device.seqParallel[pit]	();
		Device.seqParallel.clear_not_free	();
		Device.seqFrameMT.Process	(rp_Frame);

		// now we give control to device - signals that we are ended our work
		Device.mt_csEnter.Leave	();
		// waits for device signal to continue - to start again
		Device.mt_csLeave.Enter	();
		// returns sync signal to device
		Device.mt_csLeave.Leave	();
	}
}

#include "igame_level.h"
void CRenderDevice::PreCache	(u32 amount)
{
	if (HW.Caps.bForceGPU_REF)	amount=0;
	// Msg			("* PCACHE: start for %d...",amount);
	dwPrecacheFrame	= dwPrecacheTotal = amount;
	if(amount && !precache_light && g_pGameLevel)
	{
		precache_light					= ::Render->light_create();
		precache_light->set_shadow		(false);
		precache_light->set_position	(vCameraPosition);
		precache_light->set_color		(255,255,255);
		precache_light->set_range		(5.0f);
		precache_light->set_active		(true);
	}
}


ENGINE_API xr_list<LOADING_EVENT>			g_loading_events;

void CRenderDevice::Run			()
{
	g_bLoaded		= FALSE;
	MSG				msg;
    BOOL			bGotMsg;
	Log				("Starting engine...");
	set_current_thread_name("X-RAY Primary thread");

	// Startup timers and calculate timer delta
	dwTimeGlobal				= 0;
	Timer_MM_Delta				= 0;
	{
		u32 time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		u32 time_system		= timeGetTime	();
		u32 time_local		= TimerAsync	();
		Timer_MM_Delta		= time_system-time_local;
	}

	// Start all threads
//	InitializeCriticalSection	(&mt_csEnter);
//	InitializeCriticalSection	(&mt_csLeave);
	mt_csEnter.Enter			();
	mt_bMustExit				= FALSE;
	std::thread second_thread(mt_Thread);

	// Message cycle
    PeekMessage					( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	seqAppStart.Process			(rp_AppStart);

	CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1,0));

	while( WM_QUIT != msg.message  )
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        if( bGotMsg )
        {
              TranslateMessage	( &msg );
              DispatchMessage	( &msg );
         }
        else
        {
			if (b_is_Ready) {

				// FPS Lock
				static constexpr u32 menuFPSlimit  = 60;
				static constexpr u32 pauseFPSlimit = 60;
				u32 curFPSLimit = IsMainMenuActive() ? menuFPSlimit : Device.Paused() ? pauseFPSlimit : g_dwFPSlimit;

				if ( curFPSLimit > 0 )
				{
					static DWORD dwLastFrameTime = 0;
					DWORD dwCurrentTime = timeGetTime();
					if ( dwCurrentTime - dwLastFrameTime < 1000 / ( curFPSLimit + 1 ) )
						continue;
					dwLastFrameTime = dwCurrentTime;
				}

				if (psDeviceFlags.test(rsStatistic))	g_bEnableStatGather	= TRUE;
				else									g_bEnableStatGather	= FALSE;
				if(g_loading_events.size())
				{
					if( g_loading_events.front() () )
						g_loading_events.pop_front();
					
					pApp->LoadDraw				();
					continue;
				}else
					FrameMove						( );

				// Precache
				if (dwPrecacheFrame)
				{
					float factor					= float(dwPrecacheFrame)/float(dwPrecacheTotal);
					float angle						= PI_MUL_2 * factor;
					vCameraDirection.set			(_sin(angle),0,_cos(angle));	vCameraDirection.normalize	();
					vCameraTop.set					(0,1,0);
					vCameraRight.crossproduct		(vCameraTop,vCameraDirection);

					mView.build_camera_dir			(vCameraPosition,vCameraDirection,vCameraTop);
				}

				// Matrices
				mFullTransform.mul			( mProject,mView	);
				RCache.set_xform_view		( mView				);
				RCache.set_xform_project	( mProject			);
				D3DXMatrixInverse			( (D3DXMATRIX*)&mInvFullTransform, 0, (D3DXMATRIX*)&mFullTransform);

				vCameraPositionSaved = vCameraPosition;
				vCameraDirectionSaved = vCameraDirection;
				vCameraTopSaved = vCameraTop;
				vCameraRightSaved = vCameraRight;

				mFullTransformSaved = mFullTransform;
				mViewSaved = mView;
				mProjectSaved = mProject;

				// *** Resume threads
				// Capture end point - thread must run only ONE cycle
				// Release start point - allow thread to run
				mt_csLeave.Enter			();
				mt_csEnter.Leave			();

				Statistic->RenderTOTAL_Real.FrameStart	();
				Statistic->RenderTOTAL_Real.Begin		();

				Sleep(0);

				if (b_is_Active)							{
					if (Begin())				{

						seqRender.Process						(rp_Render);
						if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || Statistic->errors.size())	
							Statistic->Show						();
						End										();
					}
				}
				Statistic->RenderTOTAL_Real.End			();
				Statistic->RenderTOTAL_Real.FrameEnd	();
				Statistic->RenderTOTAL.accum	= Statistic->RenderTOTAL_Real.accum;
				// *** Suspend threads
				// Capture startup point
				// Release end point - allow thread to wait for startup point
				mt_csEnter.Enter						();
				mt_csLeave.Leave						();

				// Ensure, that second thread gets chance to execute anyway
				if (dwFrame!=mt_Thread_marker)			{
					for (u32 pit=0; pit<Device.seqParallel.size(); pit++)
						Device.seqParallel[pit]			();
					Device.seqParallel.clear_not_free	();
					seqFrameMT.Process					(rp_Frame);
				}
			} else {
				Sleep		(100);
			}
			if (!b_is_Active)	Sleep	(1);
        }
    }
	seqAppEnd.Process		(rp_AppEnd);

	// Stop Balance-Thread
	mt_bMustExit			= TRUE;
	mt_csEnter.Leave		();

	second_thread.join();
}

void ProcessLoading(RP_FUNC *f);
void CRenderDevice::FrameMove()
{
	dwFrame			++;

	dwTimeContinual	= TimerMM.GetElapsed_ms	();
	if (psDeviceFlags.test(rsConstantFPS))	{
		// 20ms = 50fps
		fTimeDelta		=	0.020f;			
		fTimeGlobal		+=	0.020f;
		dwTimeDelta		=	20;
		dwTimeGlobal	+=	20;
	} else {
		// Timer
		float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
		fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;			// smooth random system activity - worst case ~7% error
		if (fTimeDelta>.1f) fTimeDelta=.1f;									// limit to 15fps minimum

		if(Paused())		fTimeDelta = 0.0f;

//		u64	qTime		= TimerGlobal.GetElapsed_clk();
		fTimeGlobal		= TimerGlobal.GetElapsed_sec(); //float(qTime)*CPU::cycles2seconds;
		u32	_old_global	= dwTimeGlobal;
		dwTimeGlobal	= TimerGlobal.GetElapsed_ms	();	//u32((qTime*u64(1000))/CPU::cycles_per_second);
		dwTimeDelta		= dwTimeGlobal-_old_global;
	}

	// Frame move
	Statistic->EngineTOTAL.Begin	();
	if(!g_bLoaded) 
		ProcessLoading				(rp_Frame);
	else
		seqFrame.Process			(rp_Frame);
	Statistic->EngineTOTAL.End	();
}

void ProcessLoading				(RP_FUNC *f)
{
	Device.seqFrame.Process				(rp_Frame);
	g_bLoaded							= TRUE;
}

ENGINE_API BOOL bShowPauseString = TRUE;
#include "IGame_Persistent.h"

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
	static int snd_emitters_ = -1;

	if (g_bBenchmark)	return;


#ifdef DEBUG
	Msg("pause [%s] timer=[%s] sound=[%s] reason=%s",bOn?"ON":"OFF", bTimer?"ON":"OFF", bSound?"ON":"OFF", reason);
#endif // DEBUG

	if(bOn)
	{
		if(!Paused())						
			bShowPauseString				= TRUE;

		if( bTimer )
			g_pauseMngr.Pause				(TRUE);
	
		if(bSound){
			snd_emitters_ =					::Sound->pause_emitters(true);
#ifdef DEBUG
			Log("snd_emitters_[true]",snd_emitters_);
#endif // DEBUG
		}
	}else
	{
		if( bTimer && g_pauseMngr.Paused() )
			g_pauseMngr.Pause				(FALSE);
		
		if(bSound)
		{
			if(snd_emitters_>0) //avoid crash
			{
				snd_emitters_ =				::Sound->pause_emitters(false);
#ifdef DEBUG
				Log("snd_emitters_[false]",snd_emitters_);
#endif // DEBUG
			}else {
#ifdef DEBUG
				Log("Sound->pause_emitters underflow");
#endif // DEBUG
			}
		}
	}
}

BOOL CRenderDevice::Paused()
{
	return g_pauseMngr.Paused();
};

void CRenderDevice::OnWM_Activate(WPARAM wParam, LPARAM lParam)
{
	const u16 fActive = LOWORD(wParam);
	const BOOL fMinimized = (BOOL)HIWORD(wParam);
	const BOOL bActive = ((fActive != WA_INACTIVE) && (!fMinimized)) ? TRUE : FALSE;
	const BOOL isGameActive = (psDeviceFlags.is(rsAlwaysActive) || bActive) ? TRUE : FALSE;

	pInput->clip_cursor(fActive != WA_INACTIVE);

	if (isGameActive != Device.b_is_Active)
	{
		Device.b_is_Active = isGameActive;

		if (Device.b_is_Active)
			Device.seqAppActivate.Process(rp_AppActivate);
		else
			Device.seqAppDeactivate.Process(rp_AppDeactivate);
	}
}

void CRenderDevice::CSecondVPParams::SetSVPActive(bool bState) //--#SM+#-- +SecondVP+
{
	m_bIsActive = bState;
	if (g_pGamePersistent)
		g_pGamePersistent->m_pGShaderConstants.m_blender_mode.z = (m_bIsActive ? 1.0f : 0.0f);
}

bool CRenderDevice::CSecondVPParams::IsSVPFrame() //--#SM+#-- +SecondVP+
{
	bool cond = IsSVPActive() && ((Device.dwFrame % m_FrameDelay) == 0);
	if (g_pGamePersistent)
		g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y = cond ? 1.0f : 0.0f;
	return cond;
}
