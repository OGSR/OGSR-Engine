#pragma once

#include <atomic>

#include "pure.h"
#include "Render.h"
#include "../xrcore/ftimer.h"
#include "stats.h"
#include <numeric>

extern u32 g_dwFPSlimit;

#define VIEWPORT_NEAR 0.2f
#define HUD_VIEWPORT_NEAR 0.005f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RenderDeviceRender.h"

class engine_impl;

#pragma pack(push, 4)

class IRenderDevice
{
};

class ENGINE_API CRenderDeviceData
{
public:

public:
    u32 dwWidth;
    u32 dwHeight;

    u32 dwPrecacheFrame;
    BOOL b_is_Ready;
    BOOL b_is_Active;

public:
    // Engine flow-control
    u32 dwFrame;

    float fTimeDelta{};
    float fTimeDeltaReal{}, fTimeDeltaRealMS{};
    float fTimeGlobal{};

    u32 dwTimeDelta{};
    u32 dwTimeGlobal{};
    u32 dwTimeContinual{};
    u32 dwTimeDeltaContinual{};

    // Cameras & projection
    Fvector vCameraPosition{};
    Fvector vCameraDirection{};
    Fvector vCameraTop{};
    Fvector vCameraRight{};

    Fmatrix mView{};
    Fmatrix mInvView{};
    Fmatrix mProject{};
    Fmatrix mFullTransform{};
    Fmatrix mInvFullTransform{};

    // Copies of corresponding members. Used for synchronization.
    Fvector vCameraPositionSaved{};
    Fvector vCameraDirectionSaved{};
    Fvector vCameraTopSaved{};
    Fvector vCameraRightSaved{};

    Fmatrix mViewSaved{};
    Fmatrix mProjectSaved{};
    Fmatrix mFullTransformSaved{};


    Fmatrix mView_old;
    Fmatrix mProject_old;
    Fmatrix mFullTransform_old;

    Fmatrix mView_hud, mView_hud2;
    Fmatrix mProject_hud, mProject_hud2;
    Fmatrix mFullTransform_hud, mFullTransform_hud2;

    Fmatrix mView_hud_old, mView_hud_old2;
    Fmatrix mProject_hud_old, mProject_hud_old2;
    Fmatrix mFullTransform_hud_old, mFullTransform_hud_old2;

    float fFOV;
    float fASPECT;

    // Генерирует псевдо-рандомное число в диапазоне от 0 до 1 https://stackoverflow.com/a/10625698
    template <typename T, size_t sizeDest>
    inline T NoiseRandom(const T (&args)[sizeDest]) const
    {
        constexpr double consts[]{23.14069263277926, 2.665144142690225};

        static_assert(sizeDest == std::size(consts));
        static_assert(std::is_floating_point_v<T>);

        constexpr auto frac = [](const double& x) {
            double res = x - std::floor(x);
            if constexpr (std::is_same_v<double, T>)
                return res;
            else
                return static_cast<T>(res);
        };

        return frac(std::cos(std::inner_product(std::begin(args), std::end(args), std::begin(consts), 0.0)) * 12345.6789);
    }

protected:
    u32 Timer_MM_Delta{};
    CTimer_paused Timer;
    CTimer_paused TimerGlobal;

    std::thread::id mainThreadId;

public:
    // Registrators
    CRegistrator<pureRender> seqRender;
    CRegistrator<pureFrame> seqFrame;

    CRegistrator<pureAppActivate> seqAppActivate;
    CRegistrator<pureAppDeactivate> seqAppDeactivate;
    CRegistrator<pureAppStart> seqAppStart;
    CRegistrator<pureAppEnd> seqAppEnd;

    CRegistrator<pureScreenResolutionChanged> seqResolutionChanged;

    HWND m_hWnd;

    bool OnMainThread() const
    {
        extern BOOL g_bLoaded;
        return !g_bLoaded || std::this_thread::get_id() == mainThreadId;
    }
};

class ENGINE_API CRenderDeviceBase : public IRenderDevice, public CRenderDeviceData
{
public:
};

#pragma pack(pop)
// refs
class ENGINE_API CRenderDevice : public CRenderDeviceBase
{
private:
    // Main objects used for creating and rendering the 3D scene
    u32 m_dwWindowStyle;
    RECT m_rcWindowBounds;
    RECT m_rcWindowClient;

    CTimer TimerMM;

    void _Create();
    void _Destroy(BOOL bKeepTextures);
    void _SetupStates();

    xr_deque<fastdelegate::FastDelegate<void()>> seqParallel;

public:
    LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

    u32 dwPrecacheTotal;
    float fWidth_2, fHeight_2;
    void OnWM_Activate(WPARAM wParam, LPARAM lParam);

public:

    IRenderDeviceRender* m_pRender;

    void DumpResourcesMemoryUsage() { m_pRender->ResourcesDumpMemoryUsage(); }

public:
    CRegistrator<pureFrame> seqFrameMT;
    CRegistrator<pureDeviceReset> seqDeviceReset;

    xr_vector<std::future<void>> async_waiter;

    CStats* Statistic;

    CRenderDevice()
        : m_pRender(nullptr)
    {
        m_hWnd = nullptr;
        b_is_Active = FALSE;
        b_is_Ready = FALSE;
        Timer.Start();
    };

    void Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
    BOOL Paused();

    // Scene control
    void PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input);
    BOOL Begin();
    void Clear();
    void End();
    void FrameMove();

#pragma warning(push)
#pragma warning(disable : 4366)
    IC CTimer_paused* GetTimerGlobal() { return &TimerGlobal; }
#pragma warning(pop)

    u32 TimerAsync() { return TimerGlobal.GetElapsed_ms(); }
    u32 TimerAsync_MMT() { return TimerMM.GetElapsed_ms() + Timer_MM_Delta; }

    // Creation & Destroying
    void ConnectToRender();
    void Create(void);
    void ShowMainWindow() const;
    void Run(void);
    void Destroy(void);
    void Reset(bool precache = true);

    void Initialize(void);

    void time_factor(const float& time_factor);
    inline const float time_factor() const
    {
        VERIFY(Timer.time_factor() == TimerGlobal.time_factor());
        return (Timer.time_factor());
    }

private:
    std::chrono::duration<double, std::milli> SecondThreadTasksElapsedTime{}, SecondThreadFreeTimeLast{};

public:
    ICF bool add_to_seq_parallel(const fastdelegate::FastDelegate<void()>& delegate)
    {
        auto I = std::find(seqParallel.begin(), seqParallel.end(), delegate);
        if (I != seqParallel.end())
            return false;
        seqParallel.push_back(delegate);
        return true;
    }

    ICF void remove_from_seq_parallel(const fastdelegate::FastDelegate<void()>& delegate)
    {
        seqParallel.erase(std::remove(seqParallel.begin(), seqParallel.end(), delegate), seqParallel.end());
    }

public:
    void on_idle();
    bool on_message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result);

    void OnCameraUpdated(bool from_actor);

private:
    void message_loop();

public:
    bool m_AltScopeActive{};
    IC bool IsAltScopeActive() { return m_AltScopeActive; }
    void SetAltScopeActive(bool bState);
};

extern ENGINE_API CRenderDevice Device;

extern ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

class ENGINE_API CLoadScreenRenderer : public pureRender
{
public:
    CLoadScreenRenderer();
    void start(bool b_user_input);
    void stop();
    virtual void OnRender();

    bool b_registered;
};

extern ENGINE_API CLoadScreenRenderer load_screen_renderer;
