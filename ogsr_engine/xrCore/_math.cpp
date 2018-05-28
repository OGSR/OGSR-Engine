#include "stdafx.h"

#include <powerbase.h>
#pragma comment(lib, "PowrProf.lib")

typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

// Initialized on startup
XRCORE_API Fmatrix Fidentity;
XRCORE_API Dmatrix Didentity;
XRCORE_API CRandom Random;


/*
Функции управления точностью вычислений с плавающей точкой.
Более подробную информацию можно получить здесь:
https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/control87-controlfp-control87-2
Число 24, 53 и 64 - определяют ограничение точности в битах.
Наличие 'r' - включает округление результатов.
Реально в движке используются только m24r и m64r.
*/
namespace FPU
{
	void m24()
	{
#ifdef _M_IX86
		_controlfp(_PC_24, MCW_PC);
#endif
		_controlfp(_RC_CHOP, MCW_RC);
	}

	void m24r()
	{
#ifdef _M_IX86
		_controlfp(_PC_24, MCW_PC);
#endif
		_controlfp(_RC_NEAR, MCW_RC);
	}

	void m53()
	{
#ifdef _M_IX86
		_controlfp(_PC_53, MCW_PC);
#endif
		_controlfp(_RC_CHOP, MCW_RC);
	}

	void m53r()
	{
#ifdef _M_IX86
		_controlfp(_PC_53, MCW_PC);
#endif
		_controlfp(_RC_NEAR, MCW_RC);
	}

	void m64()
	{
#ifdef _M_IX86
		_controlfp(_PC_64, MCW_PC);
#endif
		_controlfp(_RC_CHOP, MCW_RC);
	}

	void m64r()
	{
#ifdef _M_IX86
		_controlfp(_PC_64, MCW_PC);
#endif
		_controlfp(_RC_NEAR, MCW_RC);
	}

	void initialize()
	{
		_clearfp();

		// По-умолчанию для плагинов экспорта из 3D-редакторов включена высокая точность вычислений с плавающей точкой
		//if (Core.PluginMode)
		//	m64r();
		//else
			m24r();

		::Random.seed(u32(CPU::GetCLK() % (1i64 << 32i64)));
	}
}


namespace CPU
{
	u64 qpc_freq;
	u32 qpc_counter = 0;
	_processor_info	ID;
	u64 QPC() {
		u64 _dest;
		QueryPerformanceCounter(PLARGE_INTEGER(&_dest));
		qpc_counter++;
		return _dest;
	}
}

void _initialize_cpu()
{
	Msg("* Detected CPU: %s [%s], F%d/M%d/S%d",
		CPU::ID.brand, CPU::ID.vendor,
		CPU::ID.family, CPU::ID.model, CPU::ID.stepping
	);

	if (strstr(Core.Params, "-x86"))
		CPU::ID.clearFeatures();

	string256 features;
	strcpy_s(features, sizeof(features), "RDTSC");
	if (CPU::ID.hasMMX()) strcat(features, ", MMX");
	if (CPU::ID.has3DNOWExt()) strcat(features, ", 3DNowExt!");
	if (CPU::ID.has3DNOW()) strcat(features, ", 3DNow!");
	if (CPU::ID.hasSSE()) strcat(features, ", SSE");
	if (CPU::ID.hasSSE2()) strcat(features, ", SSE2");
	R_ASSERT(CPU::ID.hasSSE2());
	if (CPU::ID.hasSSE3()) strcat(features, ", SSE3");
	if (CPU::ID.hasMWAIT()) strcat(features, ", MONITOR/MWAIT");
	if (CPU::ID.hasSSSE3()) strcat(features, ", SSSE3");
	if (CPU::ID.hasSSE41()) strcat(features, ", SSE4.1");
	if (CPU::ID.hasSSE42()) strcat(features, ", SSE4.2");
	if (CPU::ID.hasSSE4a()) strcat(features, ", SSE4a");
	if (CPU::ID.hasAVX()) strcat(features, ", AVX");
	if (CPU::ID.hasAVX2()) strcat(features, ", AVX2");
	Msg("* CPU features: %s", features);
	Msg("* CPU cores: [%u], threads: [%u]", CPU::ID.coresCount, CPU::ID.threadCount);

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	const size_t cpusCount = sysInfo.dwNumberOfProcessors;

	xr_vector<PROCESSOR_POWER_INFORMATION> cpusInfo(cpusCount);
	CallNtPowerInformation(ProcessorInformation, nullptr, 0, cpusInfo.data(),
		sizeof(PROCESSOR_POWER_INFORMATION) * cpusCount);

	for (size_t i = 0; i < cpusInfo.size(); i++) {
		const PROCESSOR_POWER_INFORMATION& cpuInfo = cpusInfo[i];
		Msg("* CPU%zu current freq: %lu MHz, max freq: %lu MHz",
			i, cpuInfo.CurrentMhz, cpuInfo.MaxMhz);
	}

	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	CPU::qpc_freq = Freq.QuadPart;

	Fidentity.identity();	// Identity matrix
	Didentity.identity();	// Identity matrix
	pvInitializeStatics();	// Lookup table for compressed normals
	FPU::initialize();
	//
	if (CPU::ID.hasSSE41()) {
		iFloor = iFloorSSE41;
		iCeil  = iCeilSSE41;
	}
	//
}


#ifdef _WIN32
static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void set_thread_name(DWORD dwThreadID, const char* threadName) {
	// DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}
void set_current_thread_name(const char* threadName) { set_thread_name(GetCurrentThreadId(), threadName); }

void set_thread_name(const char* threadName, std::thread& thread) {
	DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));
	set_thread_name(threadId, threadName);
}

#else
void set_thread_name(const char* threadName, std::thread& thread) {
	auto handle = thread.native_handle();
	pthread_setname_np(handle, threadName);
}

#include <sys/prctl.h>
void set_current_thread_name(const char* threadName) { prctl(PR_SET_NAME, threadName, 0, 0, 0); }

#endif


void spline1	( float t, Fvector *p, Fvector *ret )
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	float     m[4];

	ret->x=0.0f;
	ret->y=0.0f;
	ret->z=0.0f;
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	for( int i=0; i<4; i++ )
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

/* Не используется
void spline2( float t, Fvector *p, Fvector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float   m[4];

	m[0] = s*s*s;
	m[1] = 3.0f*t3 - 6.0f*t2 + 4.0f;
	m[2] = -3.0f*t3 + 3.0f*t2 + 3.0f*t +1;
	m[3] = t3;

	ret->x = (p[0].x*m[0]+p[1].x*m[1]+p[2].x*m[2]+p[3].x*m[3])/6.0f;
	ret->y = (p[0].y*m[0]+p[1].y*m[1]+p[2].y*m[2]+p[3].y*m[3])/6.0f;
	ret->z = (p[0].z*m[0]+p[1].z*m[1]+p[2].z*m[2]+p[3].z*m[3])/6.0f;
}

#define beta1 1.0f
#define beta2 0.8f

void spline3( float t, Fvector *p, Fvector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float	b12=beta1*beta2;
	float	b13=b12*beta1;
	float	delta=2.0f-b13+4.0f*b12+4.0f*beta1+beta2+2.0f;
	float	d=1.0f/delta;
	float	b0=2.0f*b13*d*s*s*s;
	float	b3=2.0f*t3*d;
	float	b1=d*(2*b13*t*(t2-3*t+3)+2*b12*(t3-3*t2+2)+2*beta1*(t3-3*t+2)+beta2*(2*t3-3*t2+1));
	float	b2=d*(2*b12*t2*(-t+3)+2*beta1*t*(-t2+3)+beta2*t2*(-2*t+3)+2*(-t3+1));

	ret->x = p[0].x*b0+p[1].x*b1+p[2].x*b2+p[3].x*b3;
	ret->y = p[0].y*b0+p[1].y*b1+p[2].y*b2+p[3].y*b3;
	ret->z = p[0].z*b0+p[1].z*b1+p[2].z*b2+p[3].z*b3;
}
*/
