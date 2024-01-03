#pragma once

#include <atomic>
#include "Event.hpp"

#include "pure.h"
#include "../xrcore/ftimer.h"
#include "stats.h"
#include <numeric>

extern u32 g_dwFPSlimit;

#define VIEWPORT_NEAR 0.2f
#define HUD_VIEWPORT_NEAR 0.05f

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
    class ENGINE_API CSecondVPParams //--#SM+#-- +SecondVP+
    {
    public:
        bool m_bCamReady; // Флаг готовности камеры (FOV, позиция, и т.п) к рендеру второго вьюпорта
    private:
        bool m_bIsActive; // Флаг активации рендера во второй вьюпорт
    public:
        IC bool IsSVPActive() { return m_bIsActive; }
        void SetSVPActive(bool bState);
        bool IsSVPFrame();
    };

public:
    u32 dwWidth;
    u32 dwHeight;

    u32 dwPrecacheFrame;
    BOOL b_is_Ready;
    BOOL b_is_Active;

public:
    // Engine flow-control
    u32 dwFrame;

    float fTimeDelta;
    float fTimeGlobal;
    u32 dwTimeDelta;
    u32 dwTimeGlobal;
    u32 dwTimeContinual;

    Fvector vCameraPosition;
    Fvector vCameraDirection;
    Fvector vCameraTop;
    Fvector vCameraRight;

    Fmatrix mView;
    Fmatrix mInvView;
    Fmatrix mProject;
    Fmatrix mFullTransform;
    Fmatrix mInvFullTransform;

    // Copies of corresponding members. Used for synchronization.
    Fvector vCameraPosition_saved;

    Fmatrix mView_saved;
    Fmatrix mProject_saved;
    Fmatrix mFullTransform_saved;

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
    u32 Timer_MM_Delta;
    CTimer_paused Timer;
    CTimer_paused TimerGlobal;

    std::thread::id mainThreadId;

public:
    // Registrators
    CRegistrator<pureRender> seqRender;
    CRegistrator<pureAppActivate> seqAppActivate;
    CRegistrator<pureAppDeactivate> seqAppDeactivate;
    CRegistrator<pureAppStart> seqAppStart;
    CRegistrator<pureAppEnd> seqAppEnd;
    CRegistrator<pureFrame> seqFrame;
    CRegistrator<pureScreenResolutionChanged> seqResolutionChanged;

    HWND m_hWnd;

    bool OnMainThread() const { return std::this_thread::get_id() == mainThreadId; }
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

    void _Create(LPCSTR shName);
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

    BOOL m_bNearer;
    void SetNearer(BOOL enabled)
    {
        if (enabled && !m_bNearer)
        {
            m_bNearer = TRUE;
            mProject._43 -= EPS_L;
        }
        else if (!enabled && m_bNearer)
        {
            m_bNearer = FALSE;
            mProject._43 += EPS_L;
        }
        m_pRender->SetCacheXform(mView, mProject);
    }

    void DumpResourcesMemoryUsage() { m_pRender->ResourcesDumpMemoryUsage(); }

public:
    CRegistrator<pureFrame> seqFrameMT;
    CRegistrator<pureDeviceReset> seqDeviceReset;

    CSecondVPParams m_SecondViewport; //--#SM+#-- +SecondVP+

    CStats* Statistic;

    CRenderDevice()
        : m_pRender(0)
    {
        m_hWnd = NULL;
        b_is_Active = FALSE;
        b_is_Ready = FALSE;
        Timer.Start();
        m_bNearer = FALSE;

        //--#SM+#-- +SecondVP+
        m_SecondViewport.SetSVPActive(false);
        m_SecondViewport.m_bCamReady = false;
    };

    void Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
    BOOL Paused();

    // Scene control
    void PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input);
    BOOL Begin();
    void Clear();
    void End();
    void FrameMove();

    void overdrawBegin();
    void overdrawEnd();

#pragma warning(push)
#pragma warning(disable : 4366)
    IC CTimer_paused* GetTimerGlobal() { return &TimerGlobal; }
#pragma warning(pop)

    u32 TimerAsync() { return TimerGlobal.GetElapsed_ms(); }
    u32 TimerAsync_MMT() { return TimerMM.GetElapsed_ms() + Timer_MM_Delta; }

    // Creation & Destroying
    void ConnectToRender();
    void Create(void);
    void Run(void);
    void Destroy(void);
    void Reset(bool precache = true);

    void Initialize(void);

    void time_factor(const float& time_factor); //--#SM+#--
    inline const float time_factor() const
    {
        VERIFY(Timer.time_factor() == TimerGlobal.time_factor());
        return (Timer.time_factor());
    }

private:
    std::chrono::duration<double, std::milli> SecondThreadTasksElapsedTime;

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

private:
    void message_loop();
    void second_thread();
};

extern ENGINE_API CRenderDevice Device;

#define RDEVICE Device

extern ENGINE_API bool g_bBenchmark;

extern ENGINE_API xr_list<fastdelegate::FastDelegate<bool()>> g_loading_events;

class ENGINE_API CLoadScreenRenderer : public pureRender
{
public:
    CLoadScreenRenderer();
    void start(bool b_user_input);
    void stop();
    virtual void OnRender();

    bool b_registered;
    bool b_need_user_input{};
};

extern ENGINE_API CLoadScreenRenderer load_screen_renderer;
