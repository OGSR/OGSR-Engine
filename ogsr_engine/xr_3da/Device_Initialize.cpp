#include "stdafx.h"
#include "resource.h"
#include <winternl.h>

static void LogOsVersion()
{
    static auto RtlGetVersion = reinterpret_cast<NTSTATUS(WINAPI*)(LPOSVERSIONINFOEXW)>(GetProcAddress(GetModuleHandle("ntdll"), "RtlGetVersion"));

    if (RtlGetVersion)
    {
        OSVERSIONINFOEXW osInfo{};
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);

        if (NT_SUCCESS(RtlGetVersion(&osInfo)))
        {
            Msg("--OS Version major: [%d] minor: [%d], build: [%d]. Server OS: [%s]", osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber,
                osInfo.wProductType != VER_NT_WORKSTATION ? "yes" : "no");
            return;
        }
    }
    Msg("!![%s] Can't get RtlGetVersion", __FUNCTION__);
}

static void LogWorkingDriveInfo()
{
    // Setup the DWORD variables.
    ULARGE_INTEGER TotalNumberOfBytes, TotalNumberOfFreeBytes;

    if (GetDiskFreeSpaceEx(Core.ApplicationPath, nullptr, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
    {
        Msg("Current drive space free: [%0.f]Mb total: [%0.f]Mb", static_cast<float>(TotalNumberOfFreeBytes.QuadPart) / 1024.f / 1024.f,
            static_cast<float>(TotalNumberOfBytes.QuadPart) / 1024.f / 1024.f);
    }
}


extern LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void CRenderDevice::Initialize()
{
    ZoneScoped;

    LogOsVersion();
    LogWorkingDriveInfo();

    Log("Initializing Engine...");
    TimerGlobal.Start();
    TimerMM.Start();

    // Unless a substitute hWnd has been specified, create a window to render into
    if (m_hWnd == nullptr)
    {
        const char* wndclass = "_XRAY_1.5";

        // Register the windows class
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(nullptr);
        WNDCLASS wndClass = {0,    WndProc, 0, 0, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)), LoadCursor(nullptr, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH),
                             nullptr, wndclass};
        RegisterClass(&wndClass);

        // Set the window's initial style
        m_dwWindowStyle = WS_BORDER | WS_DLGFRAME;

        // Set the window's initial width
        RECT rc;
        SetRect(&rc, 0, 0, 640, 480);
        AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);

        // Create the render window
        m_hWnd = CreateWindow /*Ex*/ ( // WS_EX_TOPMOST,
            wndclass, "OGSR Engine", m_dwWindowStyle,
            /*rc.left, rc.top, */ CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left), (rc.bottom - rc.top), 0L, 0, hInstance, 0L);
        gGameWindow = m_hWnd;

        TracySetProgramName("OGSR Engine");
    }

    // Save window properties
    m_dwWindowStyle = u32(GetWindowLongPtr(m_hWnd, GWL_STYLE));
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);

    /*
    if (strstr(lpCmdLine,"-gpu_sw")!=NULL)		HW.Caps.bForceGPU_SW		= TRUE;
    else										HW.Caps.bForceGPU_SW		= FALSE;
    if (strstr(lpCmdLine,"-gpu_nopure")!=NULL)	HW.Caps.bForceGPU_NonPure	= TRUE;
    else										HW.Caps.bForceGPU_NonPure	= FALSE;
    if (strstr(lpCmdLine,"-gpu_ref")!=NULL)		HW.Caps.bForceGPU_REF		= TRUE;
    else										HW.Caps.bForceGPU_REF		= FALSE;
    */
}
