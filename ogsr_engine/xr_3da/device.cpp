#include "stdafx.h"

#include <mmsystem.h>

#include "x_ray.h"
#include "render.h"
#include "xr_input.h"

#include "igame_persistent.h"

#include "imgui.h"
#include "..\Layers\xrRenderDX10\imgui_impl_dx11.h"

ENGINE_API CRenderDevice Device;
ENGINE_API CLoadScreenRenderer load_screen_renderer;

ENGINE_API BOOL g_bRendering = FALSE;

u32 g_dwFPSlimit = 60;

ENGINE_API int g_3dscopes_fps_factor = 2; // На каком кадре с момента прошлого рендера во второй вьюпорт мы начнём новый (не может быть меньше 2 - каждый второй кадр, чем больше
                                          // тем более низкий FPS во втором вьюпорте)

BOOL g_bLoaded = FALSE;

BOOL CRenderDevice::Begin()
{
    switch (m_pRender->GetDeviceState())
    {
    case IRenderDeviceRender::dsOK: break;

    case IRenderDeviceRender::dsLost:
        // If the device was lost, do not render until we get it back
        Sleep(33);
        return FALSE;
        break;

    case IRenderDeviceRender::dsNeedReset:
        // Check if the device is ready to be reset
        Reset();
        break;

    default: R_ASSERT(0);
    }

    m_pRender->Begin();

    FPU::m24r();

    g_bRendering = TRUE;

    return TRUE;
}

void CRenderDevice::Clear() { m_pRender->Clear(); }

void CRenderDevice::End(void)
{
    if (dwPrecacheFrame)
    {
        ::Sound->set_master_volume(0.f);
        dwPrecacheFrame--;

        if (!load_screen_renderer.b_registered)
            m_pRender->ClearTarget();

        if (0 == dwPrecacheFrame)
        {
            m_pRender->updateGamma();
            
            ::Sound->set_master_volume(1.f);

            m_pRender->ResourcesDestroyNecessaryTextures();
            Memory.mem_compact();
            Msg("* MEMORY USAGE: %d K", Memory.mem_usage() / 1024);
            Msg("* End of synchronization A[%d] R[%d]", b_is_Active, b_is_Ready);

        }
    }

    g_bRendering = FALSE;

    extern BOOL g_appLoaded;

    if (g_appLoaded)
    {
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    m_pRender->End();
}

#include "igame_level.h"

void CRenderDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
    if (m_pRender->GetForceGPU_REF())
        amount = 0;

    // Msg			("* PCACHE: start for %d...",amount);
    dwPrecacheFrame = dwPrecacheTotal = amount;

    if (amount && b_draw_loadscreen && load_screen_renderer.b_registered == false)
    {
        load_screen_renderer.start(b_wait_user_input);
    }
}

ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

void CRenderDevice::on_idle()
{
    if (!b_is_Ready)
    {
        Sleep(100);
        return;
    }

    const auto FrameStartTime = std::chrono::high_resolution_clock::now();

    if (psDeviceFlags.test(rsStatistic))
        g_bEnableStatGather = TRUE;
    else
        g_bEnableStatGather = FALSE;

    if (g_loading_events.size())
    {
        if (g_loading_events.front()())
            g_loading_events.pop_front();

        pApp->LoadDraw();

        return;
    }

    ImGui_ImplDX11_NewFrame(); // должно быть перед FrameMove

    FrameMove();

    // Precache
    if (dwPrecacheFrame)
    {
        float factor = float(dwPrecacheFrame) / float(dwPrecacheTotal);
        float angle = PI_MUL_2 * factor;
        vCameraDirection.set(_sin(angle), 0, _cos(angle));
        vCameraDirection.normalize();
        vCameraTop.set(0, 1, 0);
        vCameraRight.crossproduct(vCameraTop, vCameraDirection);

        mView.build_camera_dir(vCameraPosition, vCameraDirection, vCameraTop);
    }

    // Matrices
    mInvView.invert(mView);
    mFullTransform.mul(mProject, mView);
    m_pRender->SetCacheXform(mView, mProject);
    mInvFullTransform.invert_44(mFullTransform);

    vCameraPosition_saved = vCameraPosition;
    mFullTransform_saved = mFullTransform;
    mView_saved = mView;
    mProject_saved = mProject;

    const auto SecondThreadStartTime = std::chrono::high_resolution_clock::now();

    // allow secondary thread to do its job
    auto awaiter = TTAPI->submit([this] { second_thread(); });

    Statistic->RenderTOTAL_Real.FrameStart();
    Statistic->RenderTOTAL_Real.Begin();

    if (b_is_Active)
    {
        if (Begin())
        {
            seqRender.Process(rp_Render);

            if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || Statistic->errors.size())
                Statistic->Show();

            Statistic->Show_HW_Stats();

            End();
        }
    }

    ImGui::EndFrame();

    Statistic->RenderTOTAL_Real.End();
    Statistic->RenderTOTAL_Real.FrameEnd();
    Statistic->RenderTOTAL.accum = Statistic->RenderTOTAL_Real.accum;

    const auto FrameEndTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> FrameElapsedTime = FrameEndTime - FrameStartTime;

    constexpr u32 menuFPSlimit{60}, pauseFPSlimit{60};
    const u32 curFPSLimit = IsMainMenuActive() ? menuFPSlimit : Device.Paused() ? pauseFPSlimit : g_dwFPSlimit;
    if (curFPSLimit > 0 && !m_SecondViewport.IsSVPFrame())
    {
        const std::chrono::duration<double, std::milli> FpsLimitMs{std::floor(1000.f / static_cast<float>(curFPSLimit + 1))};
        if (FrameElapsedTime < FpsLimitMs)
        {
            const auto TimeToSleep = FpsLimitMs - FrameElapsedTime;
            // std::this_thread::sleep_until(FrameEndTime + TimeToSleep); // часто спит больше, чем надо. Скорее всего из-за округлений в большую сторону.
            Sleep(iFloor(TimeToSleep.count()));
            // Msg("~~[%s] waited [%f] ms", __FUNCTION__, TimeToSleep.count());
        }
    }

    const auto SecondThreadEndTime = std::chrono::high_resolution_clock::now();

    bool show_stats{};
    if (awaiter.wait_for(FrameElapsedTime) == std::future_status::timeout)
        show_stats = true;

    awaiter.get();

    if (show_stats && dwPrecacheFrame == 0)
    {
        const std::chrono::duration<double, std::milli> SecondThreadElapsedTime = SecondThreadEndTime - SecondThreadStartTime;
        const std::chrono::duration<double, std::milli> SecondThreadFreeTime = SecondThreadElapsedTime - SecondThreadTasksElapsedTime;
        Msg("##[%s] Second thread work time is too long! Avail: [%f]ms, used: [%f]ms, free: [%f]ms", __FUNCTION__, SecondThreadElapsedTime.count(),
            SecondThreadTasksElapsedTime.count(), SecondThreadFreeTime.count());
    }

    if (!b_is_Active)
        Sleep(1);
}

void CRenderDevice::message_loop()
{
    MSG msg;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        on_idle();
    }
}

void CRenderDevice::second_thread()
{
    const auto SecondThreadTasksStartTime = std::chrono::high_resolution_clock::now();

    auto size = seqParallel.size();

    while (size > 0)
    {
        seqParallel.front()();

        seqParallel.pop_front();

        size--;
    }

    seqFrameMT.Process(rp_Frame);

    const auto SecondThreadTasksEndTime = std::chrono::high_resolution_clock::now();
    SecondThreadTasksElapsedTime = SecondThreadTasksEndTime - SecondThreadTasksStartTime;
}

void CRenderDevice::Run()
{
    //	DUMP_PHASE;
    g_bLoaded = FALSE;

    Log("Starting engine...");
    set_current_thread_name("X-RAY Primary thread");
    mainThreadId = std::this_thread::get_id();

    // Startup timers and calculate timer delta
    dwTimeGlobal = 0;
    Timer_MM_Delta = 0;
    {
        u32 time_mm = timeGetTime();
        while (timeGetTime() == time_mm)
            ; // wait for next tick
        u32 time_system = timeGetTime();
        u32 time_local = TimerAsync();
        Timer_MM_Delta = time_system - time_local;
    }

    // Message cycle
    seqAppStart.Process(rp_AppStart);

    m_pRender->ClearTarget();

    message_loop();

    seqAppEnd.Process(rp_AppEnd);
}

u32 app_inactive_time = 0;
u32 app_inactive_time_start = 0;

void CRenderDevice::FrameMove()
{
    dwFrame++;

    dwTimeContinual = TimerMM.GetElapsed_ms() - app_inactive_time;

    {
        // Timer
        float fPreviousFrameTime = Timer.GetElapsed_sec();
        Timer.Start(); // previous frame
        fTimeDelta = 0.1f * fTimeDelta + 0.9f * fPreviousFrameTime; // smooth random system activity - worst case ~7% error

        if (fTimeDelta > .1f)
            fTimeDelta = .1f; // limit to 15fps minimum

        if (fTimeDelta <= 0.f)
            fTimeDelta = EPS_S + EPS_S; // limit to 15fps minimum

        if (Paused())
            fTimeDelta = 0.0f;

        fTimeGlobal = TimerGlobal.GetElapsed_sec(); // float(qTime)*CPU::cycles2seconds;

        u32 _old_global = dwTimeGlobal;
        dwTimeGlobal = TimerGlobal.GetElapsed_ms();
        dwTimeDelta = dwTimeGlobal - _old_global;
    }

    // Frame move
    Statistic->EngineTOTAL.Begin();

    Device.seqFrame.Process(rp_Frame);
    g_bLoaded = TRUE;

    Statistic->EngineTOTAL.End();
}

ENGINE_API BOOL bShowPauseString = TRUE;

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
    static int snd_emitters_ = -1;

    if (g_bBenchmark)
        return;

#ifdef DEBUG
//	Msg("pause [%s] timer=[%s] sound=[%s] reason=%s",bOn?"ON":"OFF", bTimer?"ON":"OFF", bSound?"ON":"OFF", reason);
#endif // DEBUG


    if (bOn)
    {
        if (!Paused())
            bShowPauseString =
#ifdef DEBUG
                !xr_strcmp(reason, "li_pause_key_no_clip") ? FALSE :
#endif // DEBUG
                                                             TRUE;

        if (bTimer && (!g_pGamePersistent || g_pGamePersistent->CanBePaused()))
        {
            g_pauseMngr.Pause(TRUE);
#ifdef DEBUG
            if (!xr_strcmp(reason, "li_pause_key_no_clip"))
                TimerGlobal.Pause(FALSE);
#endif // DEBUG
        }

        if (bSound && ::Sound)
        {
            snd_emitters_ = ::Sound->pause_emitters(true);
#ifdef DEBUG
//			Log("snd_emitters_[true]",snd_emitters_);
#endif // DEBUG
        }
    }
    else
    {
        if (bTimer && /*g_pGamePersistent->CanBePaused() &&*/ g_pauseMngr.Paused())
        {
            fTimeDelta = EPS_S + EPS_S;
            g_pauseMngr.Pause(FALSE);
        }

        if (bSound)
        {
            if (snd_emitters_ > 0) // avoid crash
            {
                snd_emitters_ = ::Sound->pause_emitters(false);
#ifdef DEBUG
//				Log("snd_emitters_[false]",snd_emitters_);
#endif // DEBUG
            }
            else
            {
#ifdef DEBUG
                Log("Sound->pause_emitters underflow");
#endif // DEBUG
            }
        }
    }

}

BOOL CRenderDevice::Paused() { return g_pauseMngr.Paused(); };

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
            app_inactive_time += TimerMM.GetElapsed_ms() - app_inactive_time_start;
        }
        else
        {
            app_inactive_time_start = TimerMM.GetElapsed_ms();
            Device.seqAppDeactivate.Process(rp_AppDeactivate);
        }
    }
}

CLoadScreenRenderer::CLoadScreenRenderer() : b_registered(false) {}

void CLoadScreenRenderer::start(bool b_user_input)
{
    Device.seqRender.Add(this, 0);
    b_registered = true;
    b_need_user_input = b_user_input;
}

void CLoadScreenRenderer::stop()
{
    if (!b_registered)
        return;
    Device.seqRender.Remove(this);
    pApp->DestroyLoadingScreen();
    b_registered = false;
    b_need_user_input = false;
}

void CLoadScreenRenderer::OnRender() { pApp->load_draw_internal(); }

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
