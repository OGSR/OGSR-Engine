#pragma once

#ifdef DEBUG
ENGINE_API extern BOOL bDebug;
#else
#define bDebug 0
#endif

#define _RELEASE(x) \
    { \
        if (x) \
        { \
            x->Release(); \
            x = nullptr; \
        } \
    }
#define _SHOW_REF(unused_arg, x) \
    { \
        if (x) \
        { \
            x->AddRef(); \
            Msg("[" __FUNCTION__ "] refCount of [" #x "]: [%u]", x->Release()); \
        } \
    }

// psDeviceFlags
enum
{
    //rsFullscreen = (1ul << 0ul),
    rsClearBB = (1ul << 1ul),
    rsVSync = (1ul << 2ul),
    //rsWireframe = (1ul << 3ul),
    //rsOcclusion = (1ul << 4ul),
    rsStatistic = (1ul << 5ul),
    rsDetails = (1ul << 6ul),
    rsAlwaysActive = (1ul << 7ul),
    //rsConstantFPS = (1ul << 8ul),
    //rsDrawStatic = (1ul << 9ul),
    //rsDrawDynamic = (1ul << 10ul),
    rsDisableObjectsAsCrows = (1ul << 11ul),
    //rsOcclusionDraw = (1ul << 12ul),
    //rsRefresh60hz = (1ul << 13ul),
    rsHWInfo = (1ul << 14ul),
    rsCameraPos = (1ul << 15ul),
    //rsR2 = (1ul << 16ul),
    //rsR3 = (1ul << 17ul),
    //rsR4 = (1ul << 18ul),
    rsExclusiveMode = 1ul << 19ul,
    rsOcclusionDraw = 1ul << 20ul,
};

//. ENGINE_API extern	u32			psCurrentMode		;
ENGINE_API extern u32 psCurrentVidMode[];
ENGINE_API extern Flags32 psDeviceFlags;
