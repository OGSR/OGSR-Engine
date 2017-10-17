#pragma once

#include "stacktrace.h"

namespace {

#ifdef _M_X64
extern "C" void* _ReturnAddress(void);
DWORD_PTR GetInstructionPtr() { return (DWORD_PTR)_ReturnAddress(); }
#else
void __declspec(naked, noinline) * __cdecl GetInstructionPtr() { _asm mov eax, [esp] _asm retn }
#endif
}

struct StackTraceInfo {
    static constexpr size_t capacity = 100;
	static constexpr size_t lineCapacity = 4096; // 256;
    char frames[capacity * lineCapacity];
    size_t count;

    char* operator[](const size_t i) { return frames + i * lineCapacity; }
};

inline void BuildStackTrace(EXCEPTION_POINTERS* exPtrs, StackTraceInfo& stackTrace) {
    memset(stackTrace.frames, 0, StackTraceInfo::capacity * StackTraceInfo::lineCapacity);
    auto flags = GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE | GSTSO_PARAMS;
    auto traceDump = GetFirstStackTraceString(flags, exPtrs);
    size_t frameCount = 0;
    while (traceDump) {
        lstrcpy(stackTrace.frames + frameCount * StackTraceInfo::lineCapacity, traceDump);
        frameCount++;
        traceDump = GetNextStackTraceString(flags, exPtrs);
    }
    stackTrace.count = frameCount;
}

inline void BuildStackTrace(StackTraceInfo& stackTrace) {
    // TODO: x64 - incorrect registers values
    CONTEXT context;
    EXCEPTION_POINTERS ex_ptrs;
    context.ContextFlags = CONTEXT_FULL;
    if (GetThreadContext(GetCurrentThread(), &context)) {
#ifndef _M_X64
        context.Eip = (DWORD)GetInstructionPtr();
        __asm mov context.Ebp, ebp
        __asm mov context.Esp, esp
#else
        void* ebp;
        context.Rip = GetInstructionPtr();
        context.Rbp = (DWORD64)&ebp;
        context.Rsp = (DWORD64)&context;
#endif
        ex_ptrs.ContextRecord = &context;
        ex_ptrs.ExceptionRecord = nullptr;
        BuildStackTrace(&ex_ptrs, stackTrace);
    }
}
