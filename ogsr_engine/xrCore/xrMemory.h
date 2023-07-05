#pragma once

class XRCORE_API xrMemory
{
public:
    xrMemory() = default;
    void _initialize();
    void _destroy();

    u32 stat_calls{};

    u32 mem_usage(u32* pBlocksUsed = nullptr, u32* pBlocksFree = nullptr);
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

// generic "C"-like allocations/deallocations
template <class T>
IC T* xr_alloc(u32 count)
{
    return (T*)Memory.mem_alloc(count * sizeof(T));
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
#define xr_malloc Memory.mem_alloc
#define xr_realloc Memory.mem_realloc

template <typename T, typename... Args>
T* xr_new(Args&&... args)
{
    T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
    return new (ptr) T(std::forward<Args>(args)...);
}

template <bool _is_pm, typename T>
struct xr_special_free
{
    IC void operator()(T*& ptr)
    {
        void* _real_ptr = dynamic_cast<void*>(ptr);
        ptr->~T();
        Memory.mem_free(_real_ptr);
    }
};

template <typename T>
struct xr_special_free<false, T>
{
    IC void operator()(T*& ptr)
    {
        ptr->~T();
        Memory.mem_free(ptr);
    }
};

template <class T>
IC void xr_delete(T*& ptr)
{
    if (ptr)
    {
        xr_special_free<std::is_polymorphic_v<T>, T>()(ptr);
        ptr = nullptr;
    }
}

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
