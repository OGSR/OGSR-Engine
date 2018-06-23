#pragma once

#ifndef XR_USE_BLACKBOX

static constexpr size_t MaxStackTraceDepth = 256u;
static constexpr size_t MaxFrameLength = 1024u; // 512u;

struct StackTraceInfo {
    char frames[MaxStackTraceDepth * (MaxFrameLength + 1)];
    size_t count;

    char* operator[] (const size_t index) {
        return frames + (MaxFrameLength + 1) * index;
    }
};

void BuildStackTrace(StackTraceInfo& stackTrace);

inline void BuildStackTrace(EXCEPTION_POINTERS*, StackTraceInfo& stackTrace) {
    BuildStackTrace(stackTrace);
}

#endif
