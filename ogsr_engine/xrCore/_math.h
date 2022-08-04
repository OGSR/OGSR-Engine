#pragma once

struct _processor_info;

namespace FPU
{
XRCORE_API void m24();
XRCORE_API void m24r();
XRCORE_API void m53();
XRCORE_API void m53r();
XRCORE_API void m64();
XRCORE_API void m64r();
} // namespace FPU

namespace CPU
{
XRCORE_API extern u64 qpc_freq;
XRCORE_API extern u32 qpc_counter;

XRCORE_API extern _processor_info ID;
XRCORE_API extern u64 QPC();

inline u64 GetCLK() { return __rdtsc(); }
} // namespace CPU

XRCORE_API void _initialize_cpu();
XRCORE_API void set_current_thread_name(const char* threadName);
XRCORE_API void set_thread_name(const char* threadName, std::thread& thread);
