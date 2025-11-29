#include "stdafx.h"

#include "x_ray.h"
#include "render.h"
#include "xr_input.h"
#include "igame_persistent.h"
#include "imgui.h"
#include "..\Layers\xrRenderDX10\imgui_impl_dx11.h"
#include "igame_level.h"
#include "splash.h"
#include <mmsystem.h>
#include "XR_IOConsole.h"
#include "xr_ioc_cmd.h"

//#define LOG_SECOND_THREAD_STATS

ENGINE_API CRenderDevice Device;
ENGINE_API CLoadScreenRenderer load_screen_renderer;

ENGINE_API BOOL g_bRendering = FALSE;

extern ENGINE_API float psHUD_FOV;

u32 g_dwFPSlimit = 60;

BOOL g_bLoaded = FALSE;

bool IsEditorShouldOpen();
bool is_editor_active{};

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

    
    ::Render->Begin();

    R_ASSERT(g_bRendering == FALSE, "Something Strange");

    g_bRendering = TRUE;

    return TRUE;
}

void CRenderDevice::Clear() { ::Render->Clear(); }

void CRenderDevice::End()
{
    if (dwPrecacheFrame)
    {
        ::Sound->set_master_volume(0.f);

        const HWND insertPos = IsDebuggerPresent() ? HWND_NOTOPMOST : HWND_TOPMOST;

        SetWindowPos(Device.m_hWnd, insertPos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        dwPrecacheFrame--;

        if (!load_screen_renderer.b_registered)
        {
            ::Render->ClearTarget();

            if (0 == dwPrecacheFrame)
            {
                ::Sound->set_master_volume(1.f);
            }
        }        

        if (0 == dwPrecacheFrame)
        {
            SetWindowPos(Device.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            //m_pRender->updateGamma();
            
            ::Sound->set_master_volume(1.f);

            //Memory.mem_compact();

            Msg("* MEMORY USAGE: %d K", Memory.mem_usage() / 1024);
            Msg("* End of synchronization A[%d] R[%d]", b_is_Active, b_is_Ready);
        }
    }

    g_bRendering = FALSE;

    extern BOOL g_appLoaded;

    if (g_appLoaded && is_editor_active)
    {
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    ::Render->End();

    vCameraPositionSaved = vCameraPosition;
    vCameraDirectionSaved = vCameraDirection;
    vCameraTopSaved = vCameraTop;
    vCameraRightSaved = vCameraRight;

    mFullTransformSaved = mFullTransform;
    mViewSaved = mView;
    mProjectSaved = mProject;
}

void CRenderDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
    dwPrecacheFrame = dwPrecacheTotal = amount;

    if (amount && b_draw_loadscreen && load_screen_renderer.b_registered == false)
    {
        load_screen_renderer.start(b_wait_user_input);
    }
}

ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

void CRenderDevice::OnCameraUpdated(bool from_actor)
{
    static u32 frame{u32(-1)};
    if (frame == dwFrame)
        return;

    frame = dwFrame;

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
    mInvFullTransform.invert_44(mFullTransform);

    ::Render->OnCameraUpdated(from_actor);

    ::Render->SetCacheXform(mView, mProject);

    mView_hud_old = mView_hud;
    mView_hud_old2 = mView_hud2;
    mProject_hud_old = mProject_hud;
    mProject_hud_old2 = mProject_hud2;
    mFullTransform_hud_old = mFullTransform_hud;
    mFullTransform_hud_old2 = mFullTransform_hud2;

    mView_old = mViewSaved;
    mProject_old = mProjectSaved;
    mFullTransform_old = mFullTransformSaved;

    ::Render->SetCacheXformOld(mView_old, mProject_old);

    mView_hud.build_camera_dir(Fvector{}, vCameraDirection, vCameraTop);
    mProject_hud.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), fASPECT, HUD_VIEWPORT_NEAR,
                                  g_pGamePersistent->Environment().CurrentEnv->far_plane);
    mFullTransform_hud.mul(mProject_hud, mView_hud);

    mView_hud2.set(mView);
    mProject_hud2.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), fASPECT, HUD_VIEWPORT_NEAR,
                                   g_pGamePersistent->Environment().CurrentEnv->far_plane);
    mFullTransform_hud2.mul(mProject_hud2, mView_hud2);
}

struct _SoundProcessor : public pureFrame
{
    void OnFrame() override
    {
        ZoneScopedN("_SoundProcessor");

        Device.Statistic->Sound.Begin();
        ::Sound->update(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop /*, Device.Paused()*/);
        Device.Statistic->Sound.End();
    }
} g_sound_update;

#pragma todo("SIMP: для этого кода нужны переделки в xrSound")
/*
struct _SoundRender : public pureFrame
{
    virtual void OnFrame()
    {
        ZoneScopedN("_SoundRender");

        // Msg							("------------- sound: %d [%3.2f,%3.2f,%3.2f]",u32(Device.dwFrame),VPUSH(Device.vCameraPosition));
        Device.Statistic->Sound.Begin();
        ::Sound->render();
        Device.Statistic->Sound.End();
    }
} g_sound_render;
*/

void CRenderDevice::on_idle()
{
    is_editor_active = IsEditorShouldOpen();

    if (!b_is_Ready)
    {
        Sleep(100);
        return;
    }

    if (!g_loading_events.empty())
    {
        if (g_loading_events.front()())
            g_loading_events.pop_front();

        pApp->LoadDraw();

        return;
    }

    {
        ZoneScoped;

#ifdef LOG_SECOND_THREAD_STATS
        const auto FrameStartTime = std::chrono::high_resolution_clock::now();
#endif

        if (psDeviceFlags.test(rsStatistic))
            g_bEnableStatGather = TRUE;
        else
            g_bEnableStatGather = FALSE;

        if (is_editor_active)
        {
            ImGui_ImplDX11_NewFrame(); // должно быть перед FrameMove
        }

        // if (b_is_Active)
        {
            FrameMove();
        }

        if (!g_loading_events.empty()) // that is to avoid loading screen flickering
        {
            pApp->LoadDraw();

            return;
        }

        OnCameraUpdated(false);

        const auto SecondThreadStartTime = std::chrono::high_resolution_clock::now();

        // allow secondary thread to do its job
        auto awaiter = TTAPI->submit([this] {
            ZoneScoped;

            const auto SecondThreadTasksStartTime = std::chrono::high_resolution_clock::now();

            g_sound_update.OnFrame();

            // Device.async_waiter.push_back(TTAPI->submit([]() { g_sound_render.OnFrame(); }));

            {
                ZoneScopedN("seqParallel");

                auto size = seqParallel.size();
                while (size > 0)
                {
                    seqParallel.front()();
                    seqParallel.pop_front();

                    size--;
                }
            }

            if (g_pGameLevel && ps_lua_gc_method == gc_timeout)
            {
                const auto GCTime = (SecondThreadFreeTimeLast.count() / 2.0) * 1000.0;
                psLUA_GCTIMEOUT = std::max(psLUA_GCTIMEOUT_MIN, static_cast<int>(GCTime));
                async_waiter.emplace_back(TTAPI->submit([] { g_pGameLevel->script_gc(); }));
            }

            {
                ZoneScopedN("seqFrameMT");

                seqFrameMT.Process(rp_Frame); // only PHWorld and NetProcessor
            }

            const auto SecondThreadTasksEndTime = std::chrono::high_resolution_clock::now();
            SecondThreadTasksElapsedTime = SecondThreadTasksEndTime - SecondThreadTasksStartTime;
        });

        Statistic->RenderTOTAL_Real.FrameStart();
        Statistic->RenderTOTAL_Real.Begin();

        if (b_is_Active)
        {
            ZoneScopedN("DoRender");

            if (Begin())
            {
                {
                    ZoneScopedN("Render process");
                    seqRender.Process(rp_Render);
                }

                {
                    ZoneScopedN("DoRender End");
                    End();
                }
            }
        }

        if (is_editor_active)
        {
            ImGui::EndFrame();
        }

        Statistic->RenderTOTAL_Real.End();
        Statistic->RenderTOTAL_Real.FrameEnd();

        Statistic->RenderTOTAL.accum = Statistic->RenderTOTAL_Real.accum;

#ifdef LOG_SECOND_THREAD_STATS
        const auto FrameEndTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> FrameElapsedTime = FrameEndTime - FrameStartTime;
#endif

        {
            ZoneScopedN("WaitSecondThread");

            const auto SecondThreadEndTime = std::chrono::high_resolution_clock::now();

#ifdef LOG_SECOND_THREAD_STATS
            bool show_stats{};
            if (awaiter.wait_for(FrameElapsedTime) == std::future_status::timeout)
                show_stats = true;
#else
            if (awaiter.valid())
#endif
            awaiter.wait();

            while (!async_waiter.empty())
            {
                if (async_waiter.back().valid())
                    async_waiter.back().wait();
                async_waiter.pop_back();
            }

            const std::chrono::duration<double, std::milli> SecondThreadElapsedTime = SecondThreadEndTime - SecondThreadStartTime;
            SecondThreadFreeTimeLast = SecondThreadElapsedTime - SecondThreadTasksElapsedTime;

#ifdef LOG_SECOND_THREAD_STATS
            if (show_stats && dwPrecacheFrame == 0)
            {
                Msg("##[%s] Second thread work time is too long! Avail: [%f]ms, used: [%f]ms, free: [%f]ms", __FUNCTION__, SecondThreadElapsedTime.count(),
                    SecondThreadTasksElapsedTime.count(), SecondThreadFreeTimeLast.count());
            }
#endif
        }

        extern void LogD3D11DebugMessages();
        LogD3D11DebugMessages();

        FrameMark;
    }

    if (!b_is_Active)
        Sleep(1);
}

void CRenderDevice::message_loop()
{
    MSG msg{};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        {
            on_idle();
        }
    }
}

void CRenderDevice::ShowMainWindow() const
{
    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    SetForegroundWindow(m_hWnd);
    SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CRenderDevice::Run()
{
    //	DUMP_PHASE;
    g_bLoaded = FALSE;

    Log("Starting engine...");

    set_current_thread_name("X-RAY Primary thread");
    mainThreadId = std::this_thread::get_id();
    Msg("Main thread id: [%u]", _Thrd_id());

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

    ::Render->ClearTarget();

    // Destroy LOGO
    HideSplash();

    ShowMainWindow();

    pInput->clip_cursor(true);

    Memory.mem_compact();

    g_bLoaded = TRUE;

    message_loop();

    g_bLoaded = FALSE;

    seqAppEnd.Process(rp_AppEnd);
}

u32 app_inactive_time = 0;
u32 app_inactive_time_start = 0;

void CRenderDevice::FrameMove()
{
    ZoneScoped;

    dwFrame++;

    u32 _old_continual = dwTimeContinual;
    dwTimeContinual = TimerMM.GetElapsed_ms() - app_inactive_time;
    dwTimeDeltaContinual = dwTimeContinual - _old_continual;

    fTimeDeltaReal = Timer.GetElapsed_sec();
    fTimeDeltaRealMS = static_cast<float>(Timer.GetElapsed_ms());
    if (!_valid(fTimeDeltaReal))
        fTimeDeltaReal = EPS_S + EPS_S;
    Timer.Start(); // previous frame

    {
        if (Paused())
            fTimeDelta = 0.0f;
        else
        {
            fTimeDelta = 0.1f * fTimeDelta + 0.9f * fTimeDeltaReal; // smooth random system activity - worst case ~7% error
            clamp(fTimeDelta, EPS_S + EPS_S, .1f); // limit to 10fps minimum
        }
        fTimeGlobal = TimerGlobal.GetElapsed_sec();

        u32 _old_global = dwTimeGlobal;
        dwTimeGlobal = TimerGlobal.GetElapsed_ms();
        dwTimeDelta = dwTimeGlobal - _old_global;
    }

    // Frame move
    Statistic->EngineTOTAL.Begin();

    Device.seqFrame.Process(rp_Frame);

    Statistic->EngineTOTAL.End();
}

ENGINE_API BOOL bShowPauseString = TRUE;

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
    static int snd_emitters_ = -1;

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

        if (bTimer)
        {
            g_pauseMngr->Pause(TRUE);
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
        if (bTimer && g_pauseMngr->Paused())
        {
            fTimeDelta = EPS_S + EPS_S;

            g_pauseMngr->Pause(FALSE);
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

BOOL CRenderDevice::Paused() { return g_pauseMngr->Paused(); }

void CRenderDevice::OnWM_Activate(WPARAM wParam, LPARAM lParam)
{
    const u16 fActive = LOWORD(wParam);
    const BOOL fMinimized = (BOOL)HIWORD(wParam);
    const BOOL bWndActive = (fActive != WA_INACTIVE) && !fMinimized ? TRUE : FALSE;

    const BOOL isGameActive = (!g_loading_events.empty() || (psDeviceFlags.is(rsAlwaysActive)) || bWndActive) ? TRUE : FALSE;

    if (g_bLoaded)
        pInput->clip_cursor(fActive != WA_INACTIVE);

    if (isGameActive != b_is_Active)
    {
        b_is_Active = isGameActive;

        if (b_is_Active)
        {
            seqAppActivate.Process(rp_AppActivate);
            app_inactive_time += TimerMM.GetElapsed_ms() - app_inactive_time_start;
        }
        else
        {
            app_inactive_time_start = TimerMM.GetElapsed_ms();
            seqAppDeactivate.Process(rp_AppDeactivate);
        }
    }
}

void CRenderDevice::SetAltScopeActive(bool bState)
{
    m_AltScopeActive = bState;
}

CLoadScreenRenderer::CLoadScreenRenderer() : b_registered(false) {}

void CLoadScreenRenderer::start(bool b_user_input)
{
    Device.seqRender.Add(this);
    b_registered = true;
}

void CLoadScreenRenderer::stop()
{
    if (!b_registered)
        return;

    Device.seqRender.Remove(this);
    pApp->DestroyLoadingScreen();
    b_registered = false;
}

void CLoadScreenRenderer::OnRender() { pApp->load_draw_internal(); }

void CRenderDevice::time_factor(const float& time_factor)
{
    Timer.time_factor(time_factor);
    TimerGlobal.time_factor(time_factor);
    psSoundTimeFactor = time_factor; //--#SM+#--
}

bool OnMainThread() { return Device.OnMainThread(); } // for core