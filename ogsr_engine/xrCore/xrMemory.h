#pragma once

//#define USE_MEMORY_VALIDATOR

#ifdef USE_MEMORY_VALIDATOR

#include <source_location>

#include "xrMemoryDebug.h"

extern BOOL g_enable_memory_debug;

#endif

class XRCORE_API xrMemory
{
public:
    xrMemory() = default;
    void _initialize();
    static void _destroy();

    u32 stat_calls{};

    static u32 mem_usage(u32* pBlocksUsed = nullptr, u32* pBlocksFree = nullptr);
    void mem_compact();

    void* mem_alloc(size_t size);
    void* mem_realloc(void* p, size_t size);
    void mem_free(void* p);

    void*(WINAPIV* mem_copy)(void*, const void*, size_t) = std::memcpy;
    void*(WINAPIV* mem_fill)(void*, int, size_t) = std::memset;
};
extern XRCORE_API xrMemory Memory;

#undef ZeroMemory
#undef CopyMemory
#undef FillMemory
#define ZeroMemory(a, b) std::memset(a, 0, b)
#define CopyMemory(a, b, c) std::memcpy(a, b, c)
#define FillMemory(a, b, c) std::memset(a, c, b)

#ifdef USE_MEMORY_VALIDATOR

inline void* xr_malloc(size_t size, const std::source_location& loc = std::source_location::current())
{
    void* ptr = Memory.mem_alloc(size);
    if (g_enable_memory_debug)
    {
        std::string id = std::string{"raw: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name();
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, false, size});
    }
    return ptr;
}

inline void* xr_realloc(void* p, size_t size, const std::source_location& loc = std::source_location::current())
{
    PointerRegistryRelease(p, loc);
    void* ptr = Memory.mem_realloc(p, size);
    if (g_enable_memory_debug)
    {
        std::string id = std::string{"raw: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name();
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, false, size});
    }
    return ptr;
}

inline void xr_mfree(void* p, const std::source_location& loc = std::source_location::current())
{
    PointerRegistryRelease(p, loc);
    Memory.mem_free(p);
}


// generic "C"-like allocations/deallocations
template <class T>
IC T* xr_alloc(size_t count, const bool is_container = false, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(count * sizeof(T)));
    if (g_enable_memory_debug)
    {
        std::string id = std::string{is_container ? "xr_allocator: " : "xr_alloc: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name() + " (" +
            typeid(T).name() + ")";
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, is_container, count * sizeof(T)});
    }
    return ptr;
}

template <class T>
IC void xr_free(T*& P, const std::source_location& loc = std::source_location::current())
{
    if (P)
    {
        PointerRegistryRelease((void*)P, loc);
        Memory.mem_free((void*)P);
        P = nullptr;
    }
}


template <typename T>
void registerClass(T* ptr, const std::source_location& loc)
{
    if (g_enable_memory_debug)
    {
        std::string id = std::string{"xr_new: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name() + " (" + typeid(T).name() + ")";
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), true, false, sizeof(T)});
    }
}

template <typename T>
T* xr_new(const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T();
}

template <typename T, typename A0>
T* xr_new(A0&& a0, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0));
}

template <typename T, typename A0, typename A1>
T* xr_new(A0&& a0, A1&& a1, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0), std::forward<A1>(a1));
}

template <typename T, typename A0, typename A1, typename A2>
T* xr_new(A0&& a0, A1&& a1, A2&& a2, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2));
}

template <typename T, typename A0, typename A1, typename A2, typename A3>
T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3));
}

template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4>
T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4));
}

template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4, A5&& a5, const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    registerClass(ptr, loc);
    return new (ptr) T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5));
}


template <class T>
IC void xr_delete(T*& ptr, const std::source_location& loc = std::source_location::current())
{
    if (ptr)
    {
        if constexpr (std::is_polymorphic_v<T>)
        {
            void* _real_ptr = dynamic_cast<void*>(ptr);

            ptr->~T();
            PointerRegistryRelease(_real_ptr, loc, true);
            Memory.mem_free(_real_ptr);
        }
        else
        {
            ptr->~T();
            PointerRegistryRelease(ptr, loc, true);
            Memory.mem_free(ptr);
        }
        ptr = nullptr;
    }
}

#else

#define xr_malloc(size) Memory.mem_alloc(size)
#define xr_realloc(p, size) Memory.mem_realloc(p, size)
#define xr_mfree(p) Memory.mem_free(p)


// generic "C"-like allocations/deallocations
template <class T>
IC T* xr_alloc(size_t count)
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(count * sizeof(T)));
    return ptr;
}

template <class T>
IC void xr_free(T*& P)
{
    if (P)
    {
        Memory.mem_free((void*)P);
        P = nullptr;
    }
}


template <typename T, typename... Args>
T* xr_new(Args&&... args)
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    return new (ptr) T(std::forward<Args>(args)...);
}


template <class T>
IC void xr_delete(T*& ptr)
{
    if (ptr)
    {
        if constexpr (std::is_polymorphic_v<T>)
        {
            void* _real_ptr = dynamic_cast<void*>(ptr);
            ptr->~T();
            Memory.mem_free(_real_ptr);
        }
        else
        {
            ptr->~T();
            Memory.mem_free(ptr);
        }
        ptr = nullptr;
    }
}

#endif

XRCORE_API size_t mem_usage_impl(u32* pBlocksUsed, u32* pBlocksFree);

struct SProcessMemInfo
{
    u64 PeakWorkingSetSize;
    u64 WorkingSetSize;

    u64 PeakPagefileUsage;
    u64 PagefileUsage;

    u64 TotalPhysicalMemory;
    u64 FreePhysicalMemory;
    u64 TotalPageFile;
    u64 FreePageFile;

    u32 MemoryLoad;
};
XRCORE_API void GetProcessMemInfo(SProcessMemInfo& minfo);
