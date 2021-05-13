#include "stdafx.h"

#include "../xrCDB/frustum.h"

#include <mmsystem.h>

#include "x_ray.h"
#include "render.h"
#include "xr_input.h"

// must be defined before include of FS_impl.h
// KRodin: зачем это?
//#define INCLUDE_FROM_ENGINE
//#include "../xrCore/FS_impl.h"

#ifdef INGAME_EDITOR
#	include "../include/editor/ide.hpp"
#	include "engine_impl.hpp"
#endif // #ifdef INGAME_EDITOR

//#include "xrSash.h"
#include "igame_persistent.h"

//#define SHOW_SECOND_THREAD_STATS

ENGINE_API CRenderDevice Device;
ENGINE_API CLoadScreenRenderer load_screen_renderer;

ENGINE_API BOOL g_bRendering = FALSE; 
u32 g_dwFPSlimit = 60;
ENGINE_API int g_3dscopes_fps_factor = 2; // На каком кадре с момента прошлого рендера во второй вьюпорт мы начнём новый (не может быть меньше 2 - каждый второй кадр, чем больше тем более низкий FPS во втором вьюпорте)

BOOL		g_bLoaded = FALSE;
//static ref_light precache_light{};

BOOL CRenderDevice::Begin	()
{
#ifndef DEDICATED_SERVER

	/*
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
	*/

	switch (m_pRender->GetDeviceState())
	{
	case IRenderDeviceRender::dsOK:
		break;

	case IRenderDeviceRender::dsLost:
		// If the device was lost, do not render until we get it back
		Sleep(33);
		return FALSE;
		break;

	case IRenderDeviceRender::dsNeedReset:
		// Check if the device is ready to be reset
		Reset();
		break;

	default:
		R_ASSERT(0);
	}

	m_pRender->Begin();

	/*
	CHK_DX					(HW.pDevice->BeginScene());
	RCache.OnFrameBegin		();
	RCache.set_CullMode		(CULL_CW);
	RCache.set_CullMode		(CULL_CCW);
	if (HW.Caps.SceneMode)	overdrawBegin	();
	*/

	FPU::m24r	();
	g_bRendering = 	TRUE;
#endif
	return		TRUE;
}

void CRenderDevice::Clear	()
{
	m_pRender->Clear();
}


void CRenderDevice::End		(void)
{
#ifndef DEDICATED_SERVER


#ifdef INGAME_EDITOR
	bool							load_finished = false;
#endif // #ifdef INGAME_EDITOR
	if (dwPrecacheFrame)
	{
		::Sound->set_master_volume	(0.f);
		dwPrecacheFrame	--;
		if (!load_screen_renderer.b_registered)
			m_pRender->ClearTarget();
		if (0==dwPrecacheFrame)
		{

#ifdef INGAME_EDITOR
			load_finished			= true;
#endif // #ifdef INGAME_EDITOR
			//Gamma.Update		();
			m_pRender->updateGamma();

			//if(precache_light) precache_light->set_active	(false);
			//if(precache_light) precache_light.destroy		();
			::Sound->set_master_volume						(1.f);
//			pApp->destroy_loading_shaders					();

			m_pRender->ResourcesDestroyNecessaryTextures	();
			Memory.mem_compact								();
			Msg												("* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
			Msg												("* End of synchronization A[%d] R[%d]",b_is_Active, b_is_Ready);

#ifdef FIND_CHUNK_BENCHMARK_ENABLE
			g_find_chunk_counter.flush();
#endif // FIND_CHUNK_BENCHMARK_ENABLE
		}
	}

	g_bRendering		= FALSE;
	// end scene
	//	Present goes here, so call OA Frame end.
		//KRodin: бенчмарк не нужен
	/*if (g_SASH.IsBenchmarkRunning())
		g_SASH.DisplayFrame(Device.fTimeGlobal);*/
	m_pRender->End();
	//RCache.OnFrameEnd	();
	//Memory.dbg_check		();
    //CHK_DX				(HW.pDevice->EndScene());

	//HRESULT _hr		= HW.pDevice->Present( NULL, NULL, NULL, NULL );
	//if				(D3DERR_DEVICELOST==_hr)	return;			// we will handle this later
	//R_ASSERT2		(SUCCEEDED(_hr),	"Presentation failed. Driver upgrade needed?");
#	ifdef INGAME_EDITOR
		if (load_finished && m_editor)
			m_editor->on_load_finished	();
#	endif // #ifdef INGAME_EDITOR
#endif
}


#include "igame_level.h"
void CRenderDevice::PreCache	(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
	if (m_pRender->GetForceGPU_REF()) amount=0;
#ifdef DEDICATED_SERVER
	amount = 0;
#endif
	// Msg			("* PCACHE: start for %d...",amount);
	dwPrecacheFrame	= dwPrecacheTotal = amount;
	/*if (amount && !precache_light && g_pGameLevel && g_loading_events.empty()) {
		precache_light					= ::Render->light_create();
		precache_light->set_shadow		(false);
		precache_light->set_position	(vCameraPosition);
		precache_light->set_color		(255,255,255);
		precache_light->set_range		(5.0f);
		precache_light->set_active		(true);
	}*/

	if(amount && b_draw_loadscreen && load_screen_renderer.b_registered==false)
	{
		load_screen_renderer.start	(b_wait_user_input);
	}
}


ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

void CRenderDevice::on_idle		()
{
	if (!b_is_Ready) {
		Sleep	(100);
		return;
	}


	const auto FrameStartTime = std::chrono::high_resolution_clock::now();


	if (psDeviceFlags.test(rsStatistic))	g_bEnableStatGather	= TRUE;
	else									g_bEnableStatGather	= FALSE;
	if(g_loading_events.size())
	{
		if( g_loading_events.front()() )
			g_loading_events.pop_front();
		pApp->LoadDraw				();
		return;
	}else 
	{
		FrameMove						( );
	}

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
	m_pRender->SetCacheXform(mView, mProject);
	mInvFullTransform.invert_44(mFullTransform);

	vCameraPosition_saved	= vCameraPosition;
	mFullTransform_saved	= mFullTransform;
	mView_saved				= mView;
	mProject_saved			= mProject;

#ifdef SHOW_SECOND_THREAD_STATS
	const auto SecondThreadStartTime = std::chrono::high_resolution_clock::now();
#endif

	syncProcessFrame.Set(); // allow secondary thread to do its job

	Statistic->RenderTOTAL_Real.FrameStart	();
	Statistic->RenderTOTAL_Real.Begin		();
	if (b_is_Active)							{
		if (Begin())				{

			seqRender.Process						(rp_Render);
			if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || Statistic->errors.size())	
				Statistic->Show						();

			Statistic->Show_HW_Stats();

			//	TEST!!!
			//Statistic->RenderTOTAL_Real.End			();
			//	Present goes here
			End										();
		}
	}
	Statistic->RenderTOTAL_Real.End			();
	Statistic->RenderTOTAL_Real.FrameEnd	();
	Statistic->RenderTOTAL.accum	= Statistic->RenderTOTAL_Real.accum;


	const auto FrameEndTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> FrameElapsedTime = FrameEndTime - FrameStartTime;

	constexpr u32 menuFPSlimit{ 60 }, pauseFPSlimit{ 60 };
	const u32 curFPSLimit = IsMainMenuActive() ? menuFPSlimit : Device.Paused() ? pauseFPSlimit : g_dwFPSlimit;
	if (curFPSLimit > 0 && !m_SecondViewport.IsSVPFrame())
	{
		const std::chrono::duration<double, std::milli> FpsLimitMs{ std::floor(1000.f / (curFPSLimit + 1)) };
		if (FrameElapsedTime < FpsLimitMs)
		{
			const auto TimeToSleep = FpsLimitMs - FrameElapsedTime;
			//std::this_thread::sleep_until(FrameEndTime + TimeToSleep); // часто спит больше, чем надо. Скорее всего из-за округлений в большую сторону.
			Sleep(iFloor(TimeToSleep.count()));
			//Msg("~~[%s] waited [%f] ms", __FUNCTION__, TimeToSleep.count());
		}
	}

#ifdef SHOW_SECOND_THREAD_STATS
	const auto SecondThreadEndTime = std::chrono::high_resolution_clock::now();
#endif

	syncFrameDone.WaitEx(66); // wait until secondary thread finish its job

#ifdef SHOW_SECOND_THREAD_STATS
	const std::chrono::duration<double, std::milli> SecondThreadElapsedTime = SecondThreadEndTime - SecondThreadStartTime;
	const std::chrono::duration<double, std::milli> SecondThreadFreeTime = SecondThreadElapsedTime - SecondThreadTasksElapsedTime;
	Msg("##[%s] Second thread work time: [%f]ms, used: [%f]ms, free: [%f]ms", __FUNCTION__, SecondThreadElapsedTime.count(), SecondThreadTasksElapsedTime.count(), SecondThreadFreeTime.count());
#endif

	if (!b_is_Active)
		Sleep		(1);
}

#ifdef INGAME_EDITOR
void CRenderDevice::message_loop_editor	()
{
	m_editor->run			();
	m_editor_finalize		(m_editor);
	xr_delete				(m_engine);
}
#endif // #ifdef INGAME_EDITOR

void CRenderDevice::message_loop()
{
#ifdef INGAME_EDITOR
	if (editor()) {
		message_loop_editor	();
		return;
	}
#endif // #ifdef INGAME_EDITOR

	MSG						msg;
    PeekMessage				(&msg, NULL, 0U, 0U, PM_NOREMOVE );
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage	(&msg);
			continue;
		}

		on_idle				();
    }
}

void CRenderDevice::Run			()
{
//	DUMP_PHASE;
	g_bLoaded		= FALSE;
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
	mt_bMustExit = false;
	// KRodin: TODO: Use C++20 std::jthread
	std::thread second_thread([] (void* context) {
		set_current_thread_name("X-RAY Secondary thread");

		CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		auto& device = *static_cast<CRenderDevice*>(context);

		while (true) {
			device.syncProcessFrame.Wait();

			if (device.mt_bMustExit) {
				device.syncThreadExit.Set();
				return;
			}

#ifdef SHOW_SECOND_THREAD_STATS
			const auto SecondThreadTasksStartTime = std::chrono::high_resolution_clock::now();
#endif

			for (const auto& Func : device.seqParallel)
				Func();
			device.seqParallel.clear_and_free();
			device.seqFrameMT.Process(rp_Frame);

#ifdef SHOW_SECOND_THREAD_STATS
			const auto SecondThreadTasksEndTime = std::chrono::high_resolution_clock::now();
			device.SecondThreadTasksElapsedTime = SecondThreadTasksEndTime - SecondThreadTasksStartTime;
#endif

			device.syncFrameDone.Set();
		}
	}, this);

	// Message cycle
	seqAppStart.Process			(rp_AppStart);

	//CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1,0));
	m_pRender->ClearTarget		();

	message_loop				();

	seqAppEnd.Process		(rp_AppEnd);

	// Stop Balance-Thread
	mt_bMustExit = true;
	syncProcessFrame.Set();
	syncThreadExit.Wait();
	second_thread.join();
}

u32 app_inactive_time		= 0;
u32 app_inactive_time_start = 0;

void CRenderDevice::FrameMove()
{
	dwFrame			++;

	dwTimeContinual	= TimerMM.GetElapsed_ms() - app_inactive_time;

	if (psDeviceFlags.test(rsConstantFPS))	{
		// 20ms = 50fps
		//fTimeDelta		=	0.020f;			
		//fTimeGlobal		+=	0.020f;
		//dwTimeDelta		=	20;
		//dwTimeGlobal	+=	20;
		// 33ms = 30fps
		fTimeDelta		=	0.033f;			
		fTimeGlobal		+=	0.033f;
		dwTimeDelta		=	33;
		dwTimeGlobal	+=	33;
	} else {
		// Timer
		float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
		fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;			// smooth random system activity - worst case ~7% error
		//fTimeDelta = 0.7f * fTimeDelta + 0.3f*fPreviousFrameTime;			// smooth random system activity
		if (fTimeDelta>.1f)    
			fTimeDelta = .1f;							// limit to 15fps minimum

		if (fTimeDelta <= 0.f) 
			fTimeDelta = EPS_S + EPS_S;					// limit to 15fps minimum

		if(Paused())	
			fTimeDelta = 0.0f;

//		u64	qTime		= TimerGlobal.GetElapsed_clk();
		fTimeGlobal		= TimerGlobal.GetElapsed_sec(); //float(qTime)*CPU::cycles2seconds;
		u32	_old_global	= dwTimeGlobal;
		dwTimeGlobal = TimerGlobal.GetElapsed_ms();
		dwTimeDelta		= dwTimeGlobal-_old_global;
	}

	// Frame move
	Statistic->EngineTOTAL.Begin	();

	Device.seqFrame.Process(rp_Frame);
	g_bLoaded = TRUE;

	Statistic->EngineTOTAL.End	();
}


ENGINE_API BOOL bShowPauseString = TRUE;
#include "IGame_Persistent.h"

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
	static int snd_emitters_ = -1;

	if (g_bBenchmark)	return;


#ifdef DEBUG
//	Msg("pause [%s] timer=[%s] sound=[%s] reason=%s",bOn?"ON":"OFF", bTimer?"ON":"OFF", bSound?"ON":"OFF", reason);
#endif // DEBUG

#ifndef DEDICATED_SERVER	

	if(bOn)
	{
		if(!Paused())						
			bShowPauseString				= 
#ifdef INGAME_EDITOR
				editor() ? FALSE : 
#endif // #ifdef INGAME_EDITOR
#ifdef DEBUG
				!xr_strcmp(reason, "li_pause_key_no_clip")?	FALSE:
#endif // DEBUG
				TRUE;

		if( bTimer && (!g_pGamePersistent || g_pGamePersistent->CanBePaused()) )
		{
			g_pauseMngr.Pause				(TRUE);
#ifdef DEBUG
			if(!xr_strcmp(reason, "li_pause_key_no_clip"))
				TimerGlobal.Pause				(FALSE);
#endif // DEBUG
		}
	
		if (bSound && ::Sound) {
			snd_emitters_ =					::Sound->pause_emitters(true);
#ifdef DEBUG
//			Log("snd_emitters_[true]",snd_emitters_);
#endif // DEBUG
		}
	}else
	{
		if( bTimer && /*g_pGamePersistent->CanBePaused() &&*/ g_pauseMngr.Paused() )
		{
			fTimeDelta						= EPS_S + EPS_S;
			g_pauseMngr.Pause				(FALSE);
		}
		
		if(bSound)
		{
			if(snd_emitters_>0) //avoid crash
			{
				snd_emitters_ =				::Sound->pause_emitters(false);
#ifdef DEBUG
//				Log("snd_emitters_[false]",snd_emitters_);
#endif // DEBUG
			}else {
#ifdef DEBUG
				Log("Sound->pause_emitters underflow");
#endif // DEBUG
			}
		}
	}

#endif

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
	const BOOL isGameActive = ((psDeviceFlags.is(rsAlwaysActive) && !psDeviceFlags.is(rsFullscreen)) || bActive) ? TRUE : FALSE;

	pInput->clip_cursor(fActive != WA_INACTIVE);

	if (isGameActive != Device.b_is_Active)
	{
		Device.b_is_Active = isGameActive;

		if (Device.b_is_Active)	
		{
			Device.seqAppActivate.Process(rp_AppActivate);
			app_inactive_time		+= TimerMM.GetElapsed_ms() - app_inactive_time_start;
		}
		else	
		{
			app_inactive_time_start	= TimerMM.GetElapsed_ms();
			Device.seqAppDeactivate.Process(rp_AppDeactivate);
		}
	}
}

void	CRenderDevice::AddSeqFrame			( pureFrame* f, bool mt )
{
		if ( mt )	
		seqFrameMT.Add	(f,REG_PRIORITY_HIGH);
	else								
		seqFrame.Add		(f,REG_PRIORITY_LOW);

}

void	CRenderDevice::RemoveSeqFrame	( pureFrame* f )
{
	seqFrameMT.Remove	( f );
	seqFrame.Remove		( f );
}

CLoadScreenRenderer::CLoadScreenRenderer()
:b_registered(false)
{}

void CLoadScreenRenderer::start(bool b_user_input) 
{
	Device.seqRender.Add			(this, 0);
	b_registered					= true;
	b_need_user_input				= b_user_input;
}

void CLoadScreenRenderer::stop()
{
	if(!b_registered)				return;
	Device.seqRender.Remove			(this);
	pApp->DestroyLoadingScreen();
	b_registered					= false;
	b_need_user_input				= false;
}

void CLoadScreenRenderer::OnRender() 
{
	pApp->load_draw_internal();
}

void CRenderDevice::CSecondVPParams::SetSVPActive(bool bState) //--#SM+#-- +SecondVP+
{
	m_bIsActive = bState;
	if (g_pGamePersistent)
		g_pGamePersistent->m_pGShaderConstants.m_blender_mode.z = (m_bIsActive ? 1.0f : 0.0f);
}

bool CRenderDevice::CSecondVPParams::IsSVPFrame() //--#SM+#-- +SecondVP+
{
	bool cond = IsSVPActive() && ((Device.dwFrame % g_3dscopes_fps_factor) == 0);
	if (g_pGamePersistent)
		g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y = cond ? 1.0f : 0.0f;
	return cond;
}

void CRenderDevice::time_factor(const float& time_factor)
{
	Timer.time_factor(time_factor);
	TimerGlobal.time_factor(time_factor);
	psSoundTimeFactor = time_factor; //--#SM+#--
}
