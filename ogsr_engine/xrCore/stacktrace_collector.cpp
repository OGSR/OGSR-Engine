#include "stdafx.h"

#include <sstream>

#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "dbghelp.lib")


bool symEngineInitialized = false;

bool InitializeSymbolEngine()
{
	if (!symEngineInitialized)
	{
		DWORD dwOptions = SymGetOptions();
		SymSetOptions(dwOptions | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

		if (SymInitialize(GetCurrentProcess(), nullptr, TRUE))
			symEngineInitialized = true;
	}

	return symEngineInitialized;
}

void DeinitializeSymbolEngine()
{
	if (symEngineInitialized)
	{
		SymCleanup(GetCurrentProcess());

		symEngineInitialized = false;
	}
}


#ifdef _M_X64
#	define MACHINE_TYPE IMAGE_FILE_MACHINE_AMD64
#else
#	define MACHINE_TYPE IMAGE_FILE_MACHINE_I386
#endif

std::stringstream BuildStackTrace(const char* header, PCONTEXT threadCtx, u16 maxFramesCount)
{
	static std::mutex dbghelpMutex;
	std::lock_guard<std::mutex> lock(dbghelpMutex);

	std::stringstream traceResult;
	traceResult << header;


	if (!InitializeSymbolEngine())
	{
		const auto LastErr = GetLastError();
		Msg("!![%s] InitializeSymbolEngine failed with error: [%d], descr: [%s]", __FUNCTION__, LastErr, Debug.error2string(LastErr));
		return traceResult;
	}


	STACKFRAME stackFrame = { 0 };
#ifdef _M_X64
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrPC.Offset = threadCtx->Rip;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = threadCtx->Rsp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = threadCtx->Rbp;
#else
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrPC.Offset = threadCtx->Eip;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = threadCtx->Esp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = threadCtx->Ebp;
#endif
	auto lpstackFrame = reinterpret_cast<LPSTACKFRAME>(&stackFrame);


	u16 count_frames = 0;
	while (count_frames++ <= maxFramesCount) {
		BOOL result = StackWalk(MACHINE_TYPE, GetCurrentProcess(), GetCurrentThread(), lpstackFrame, threadCtx, nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr);

		if (!result || lpstackFrame->AddrPC.Offset == 0)
			break;

		// Module name
		string512 formatBuff = { 0 };
		auto hModule = (HINSTANCE)SymGetModuleBase(GetCurrentProcess(), lpstackFrame->AddrPC.Offset);
		if (hModule && GetModuleFileName(hModule, formatBuff, sizeof(formatBuff)))
			traceResult << "Module: [" << formatBuff << "]";

		// Address
		traceResult << ", AddrPC.Offset: [" << reinterpret_cast<const void*>(lpstackFrame->AddrPC.Offset) << "]";

		// Function info
		BYTE arrSymBuffer[512] = { 0 };
		auto functionInfo = reinterpret_cast<PIMAGEHLP_SYMBOL>(arrSymBuffer);
		functionInfo->SizeOfStruct = sizeof(*functionInfo);
		functionInfo->MaxNameLength = sizeof(arrSymBuffer) - sizeof(*functionInfo) + 1;
		DWORD_PTR dwFunctionOffset = 0;

		result = SymGetSymFromAddr(GetCurrentProcess(), lpstackFrame->AddrPC.Offset, &dwFunctionOffset, functionInfo);

		if (result)
		{
			if (dwFunctionOffset)
				traceResult << ", Fun: [" << functionInfo->Name << "()] + [" << dwFunctionOffset << " byte(s)]";
			else
				traceResult << ", Fun: [" << functionInfo->Name << "()]";
		}

		// Source info
		DWORD dwLineOffset = 0;
		IMAGEHLP_LINE sourceInfo = { 0 };
		sourceInfo.SizeOfStruct = sizeof(sourceInfo);

		result = SymGetLineFromAddr(GetCurrentProcess(), lpstackFrame->AddrPC.Offset, &dwLineOffset, &sourceInfo);

		if (result)
		{
			if (dwLineOffset)
				traceResult << ", File-->Line: [" << sourceInfo.FileName << "-->" << sourceInfo.LineNumber << "] + [" << dwLineOffset << " byte(s)]";
			else
				traceResult << ", File-->Line: [" << sourceInfo.FileName << "-->" << sourceInfo.LineNumber << "]";
		}

		traceResult << "\n";
	}


	DeinitializeSymbolEngine();


	return traceResult;
}


std::stringstream BuildStackTrace(const char* header, u16 maxFramesCount)
{
	CONTEXT currentThreadCtx = { 0 };

	RtlCaptureContext(&currentThreadCtx); // GetThreadContext cann't be used on the current thread 
	currentThreadCtx.ContextFlags = CONTEXT_FULL;

	return BuildStackTrace(header, &currentThreadCtx, maxFramesCount);
}
