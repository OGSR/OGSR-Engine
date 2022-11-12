#include "stdafx.h"

#if __cpp_lib_stacktrace

#include <stacktrace>

std::string BuildStackTrace(const char* header) { return header + std::to_string(std::stacktrace::current()); }

std::string BuildStackTrace(const char* header, PCONTEXT) { return BuildStackTrace(header); }

#else

#include <sstream>

#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "dbghelp.lib")

constexpr u16 MaxFuncNameLength = 4096, maxFramesCount = 512;

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
#define MACHINE_TYPE IMAGE_FILE_MACHINE_AMD64
#else
#define MACHINE_TYPE IMAGE_FILE_MACHINE_I386
#endif

std::string BuildStackTrace(const char* header, PCONTEXT threadCtx)
{
    static std::mutex dbghelpMutex;
    std::scoped_lock<decltype(dbghelpMutex)> lock(dbghelpMutex);

    std::stringstream traceResult;
    traceResult << header;

    if (!InitializeSymbolEngine())
    {
        const auto LastErr = GetLastError();
        traceResult << "[" << __FUNCTION__ << "] InitializeSymbolEngine failed with error: [" << LastErr << "], descr: [" << Debug.error2string(LastErr) << "]";
        return traceResult.str();
    }

    STACKFRAME stackFrame = {0};
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
    while (count_frames++ <= maxFramesCount)
    {
        BOOL result = StackWalk(MACHINE_TYPE, GetCurrentProcess(), GetCurrentThread(), lpstackFrame, threadCtx, nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr);

        if (!result || lpstackFrame->AddrPC.Offset == 0)
            break;

        // Module name
        IMAGEHLP_MODULE moduleInfo = {0};
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);

        result = SymGetModuleInfo(GetCurrentProcess(), lpstackFrame->AddrPC.Offset, &moduleInfo);

        if (result)
            traceResult << "Module: [" << moduleInfo.ImageName << "]";

        // Address
        traceResult << ", AddrPC.Offset: [" << reinterpret_cast<const void*>(lpstackFrame->AddrPC.Offset) << "]";

        // Function info
        BYTE arrSymBuffer[MaxFuncNameLength] = {0};
        auto functionInfo = reinterpret_cast<PIMAGEHLP_SYMBOL>(&arrSymBuffer);
        functionInfo->SizeOfStruct = sizeof(*functionInfo);
        functionInfo->MaxNameLength = sizeof(arrSymBuffer) - offsetof(IMAGEHLP_SYMBOL, Name);
        DWORD_PTR dwFunctionOffset = 0;

        result = SymGetSymFromAddr(GetCurrentProcess(), lpstackFrame->AddrPC.Offset, &dwFunctionOffset, functionInfo);

        if (result)
        {
            traceResult << ", Fun: [" << functionInfo->Name << "()]";
            if (dwFunctionOffset)
                traceResult << " + [" << dwFunctionOffset << " byte(s)]";
        }

        // Source info
        DWORD dwLineOffset = 0;
        IMAGEHLP_LINE sourceInfo = {0};
        sourceInfo.SizeOfStruct = sizeof(sourceInfo);

        result = SymGetLineFromAddr(GetCurrentProcess(), lpstackFrame->AddrPC.Offset, &dwLineOffset, &sourceInfo);

        if (result)
        {
            traceResult << ", File-->Line: [" << sourceInfo.FileName << "-->" << sourceInfo.LineNumber << "]";
            if (dwLineOffset)
                traceResult << " + [" << dwLineOffset << " byte(s)]";
        }

        traceResult << "\n";
    }

    DeinitializeSymbolEngine();

    return traceResult.str();
}

std::string BuildStackTrace(const char* header)
{
    CONTEXT currentThreadCtx = {0};

    RtlCaptureContext(&currentThreadCtx); // GetThreadContext cann't be used on the current thread
    currentThreadCtx.ContextFlags = CONTEXT_FULL;

    return BuildStackTrace(header, &currentThreadCtx);
}

#endif
