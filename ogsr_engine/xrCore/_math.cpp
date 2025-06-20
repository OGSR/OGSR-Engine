#include "stdafx.h"

#include "cpuid.h"
#include <powerbase.h>

#pragma comment(lib, "PowrProf.lib")
#include <VersionHelpers.h>

typedef struct _PROCESSOR_POWER_INFORMATION
{
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

// Initialized on startup
XRCORE_API Fmatrix Fidentity;

namespace CPU
{
_processor_info ID;
u64 QPC()
{
    u64 _dest;
    QueryPerformanceCounter(PLARGE_INTEGER(&_dest));
    return _dest;
}
} // namespace CPU

void _initialize_cpu()
{
    Msg("* Detected CPU: %s [%s], F%d/M%d/S%d", CPU::ID.brand, CPU::ID.vendor, CPU::ID.family, CPU::ID.model, CPU::ID.stepping);

    // WTF???
    // if (strstr(Core.Params, "-x86"))
    //	CPU::ID.clearFeatures();

    string256 features;
    strcpy_s(features, sizeof(features), "RDTSC");
    if (CPU::ID.hasMMX())
        strcat(features, ", MMX");
    if (CPU::ID.has3DNOWExt())
        strcat(features, ", 3DNowExt!");
    if (CPU::ID.has3DNOW())
        strcat(features, ", 3DNow!");
    if (CPU::ID.hasSSE())
        strcat(features, ", SSE");
    if (CPU::ID.hasSSE2())
        strcat(features, ", SSE2");
    if (CPU::ID.hasSSE3())
        strcat(features, ", SSE3");
    if (CPU::ID.hasMWAIT())
        strcat(features, ", MONITOR/MWAIT");
    if (CPU::ID.hasSSSE3())
        strcat(features, ", SSSE3");
    if (CPU::ID.hasSSE41())
        strcat(features, ", SSE4.1");
    if (CPU::ID.hasSSE42())
        strcat(features, ", SSE4.2");
    if (CPU::ID.hasSSE4a())
        strcat(features, ", SSE4a");
    if (CPU::ID.hasAVX())
        strcat(features, ", AVX");
    if (CPU::ID.hasAVX2())
        strcat(features, ", AVX2");

    Msg("* CPU features: %s", features);
    Msg("* CPU cores: [%u], threads: [%u]", CPU::ID.coresCount, CPU::ID.threadCount);

    R_ASSERT(CPU::ID.hasSSE2());

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const size_t cpusCount = sysInfo.dwNumberOfProcessors;

    xr_vector<PROCESSOR_POWER_INFORMATION> cpusInfo(cpusCount);
    CallNtPowerInformation(ProcessorInformation, nullptr, 0, cpusInfo.data(), ULONG(sizeof(PROCESSOR_POWER_INFORMATION) * cpusCount));

    for (size_t i = 0; i < cpusInfo.size(); i++)
    {
        const PROCESSOR_POWER_INFORMATION& cpuInfo = cpusInfo[i];
        Msg("* CPU%zu current freq: %lu MHz, max freq: %lu MHz", i, cpuInfo.CurrentMhz, cpuInfo.MaxMhz);
    }

    Fidentity.identity(); // Identity matrix
    pvInitializeStatics(); // Lookup table for compressed normals
}

#pragma pack(push, 8)
typedef struct
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void set_thread_name(HANDLE ThreadHandle, const char* threadName)
{
#ifdef TRACY_ENABLE
    tracy::SetThreadName(threadName);
#endif

    if (IsWindows10OrGreater())
    {
        static HMODULE KernelLib = GetModuleHandle("kernel32.dll");
        using FuncSetThreadDescription = HRESULT(WINAPI*)(HANDLE, PCWSTR);
        static auto pSetThreadDescription = (FuncSetThreadDescription)GetProcAddress(KernelLib, "SetThreadDescription");

        if (pSetThreadDescription)
        {
            wchar_t buf[64]{};
            mbstowcs(buf, threadName, std::size(buf));

            pSetThreadDescription(ThreadHandle, buf);

            return;
        }
        else if (!IsDebuggerPresent())
            return;
    }
    else if (!IsDebuggerPresent())
        return;

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = GetThreadId(ThreadHandle);
    info.dwFlags = 0;

    __try
    {
        constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {}
}

void set_current_thread_name(const char* threadName) { set_thread_name(GetCurrentThread(), threadName); }

void set_thread_name(const char* threadName, std::thread& thread) { set_thread_name(static_cast<HANDLE>(thread.native_handle()), threadName); }
