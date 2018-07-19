#include "stdafx.h"

#ifndef XR_USE_BLACKBOX

//!!! KRodin: It is necessary to disable in the settings of all projects "Frame pointer ommision" (/Oy), otherwise it will not work !!!

#ifdef _M_IX86
#pragma message( "CaptureStackBackTrace на x86 не всегда может развернуть стек. Это не баг, так и должно быть." )
#endif

#include "stacktrace_collector.h"

#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "dbghelp.lib")

thread_local static void* stack[MaxStackTraceDepth];
thread_local static char symbol[sizeof(SYMBOL_INFO) + (MaxFrameLength + 1) * sizeof(char)];

// Disable optimizations for the CaptureStackBackTrace() function. It is
// important to disable at least frame pointer optimization ("y"), since
// that breaks CaptureStackBackTrace() and prevents StackTrace from working
// in Release builds (it may still be janky if other frames are using FPO,
// but at least it will make it further).
#pragma optimize("", off)

void BuildStackTrace(StackTraceInfo& stackTrace) {
	static std::mutex dbghelpMutex;
	std::lock_guard<std::mutex> lock(dbghelpMutex);

	const auto processHandle = GetCurrentProcess();
	const BOOL SymInit = SymInitialize(processHandle, nullptr, TRUE);

	const auto framesCount = CaptureStackBackTrace(1, MaxStackTraceDepth, stack, nullptr);
	auto symbolInfo = reinterpret_cast<SYMBOL_INFO*>(&symbol);
	symbolInfo->MaxNameLen = MaxFrameLength;
	symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

	IMAGEHLP_LINE64 lineInfo = { 0 };
	lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	IMAGEHLP_MODULE64 moduleInfo = { 0 };
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

	for (size_t i = 0; i < framesCount; i++) {
		const auto addr = reinterpret_cast<DWORD64>(stack[i]);
		SymFromAddr(processHandle, addr, nullptr, symbolInfo);
		DWORD displacement = 0;
		SymGetLineFromAddr64(processHandle, addr, &displacement, &lineInfo);
		SymGetModuleInfo64(processHandle, addr, &moduleInfo);
		auto dst = stackTrace.frames + (MaxFrameLength + 1) * i;
		std::snprintf(dst, MaxFrameLength + 1, "[%zi]: [%s]: [%s()] at [%s:%u]", framesCount - i, moduleInfo.ImageName, symbolInfo->Name, lineInfo.FileName, lineInfo.LineNumber);
	}

	stackTrace.count = framesCount;

	if (SymInit)
		SymCleanup(processHandle);
}

#pragma optimize("", on)

#endif
