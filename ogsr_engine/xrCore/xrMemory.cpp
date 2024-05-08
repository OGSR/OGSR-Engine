#include "stdafx.h"
#include <psapi.h>
#include "xrsharedmem.h"

#ifndef __SANITIZE_ADDRESS__
#define USE_MIMALLOC
#endif

#ifdef USE_MIMALLOC
#include "..\mimalloc\include\mimalloc-override.h"
#ifdef XRCORE_STATIC
// xrSimpodin: перегрузка операторов new/delete будет действовать только внутри того модуля движка, в котором они перегружены.
// Если движок разбит на модули и операторы перегружены в xrCore.dll, то в других модулях будут использоваться стандартные операторы,
// и если создать объект через new в xrCore, а delete сделать в xrGame - будет ошибка, т.к. объект создали кастомным аллокатором, а удалить пытаемся системным.
// Здесь два варианта решения проблемы: или перегружать операторы в каждом модуле, что не очень рационально,
// или перегружать их только в случае, если движок собирается в один exe файл. Второй вариант мне кажется более рациональным.
#include "..\mimalloc\include\mimalloc-new-delete.h"
#endif
#pragma comment(lib, "mimalloc-static")
#endif

xrMemory Memory;

void xrMemory::_initialize()
{
    SProcessMemInfo memCounters;
    GetProcessMemInfo(memCounters);

    u64 disableMemoryTotalMb = 32000ull;
    if (char* str = strstr(Core.Params, "-smem_disable_limit "))
        sscanf(str + 20, "%llu", &disableMemoryTotalMb);
    bool disableMemoryPool = memCounters.TotalPhysicalMemory > (disableMemoryTotalMb * (1024ull * 1024ull));

    if (disableMemoryPool)
    {
        Msg("--[%s] memory pool disabled due to available memory limit: [%u MB]", __FUNCTION__, disableMemoryTotalMb);
    }

    g_pStringContainer = xr_new<str_container>();
    g_pSharedMemoryContainer = xr_new<smem_container>(disableMemoryPool);
}

void xrMemory::_destroy()
{
    xr_delete(g_pSharedMemoryContainer);
    xr_delete(g_pStringContainer);
}

void xrMemory::mem_compact()
{
#ifndef USE_MIMALLOC
/*
Следующая команда, в целом, не нужна.
Современные аллокаторы достаточно грамотно и когда нужно возвращают память операционной системе.
Эта строчка нужна, скорее всего, в определённых ситуациях, вроде использования файлов отображаемых в память,
которые требуют большие свободные области памяти.
*/
    _heapmin(); //-V530
    HeapCompact(GetProcessHeap(), 0);
#endif

    if (g_pStringContainer)
        g_pStringContainer->clean();
    if (g_pSharedMemoryContainer)
        g_pSharedMemoryContainer->clean();
}

void* xrMemory::mem_alloc(size_t size)
{
    void* ptr = malloc(size);

    return ptr;
}

void xrMemory::mem_free(void* P)
{
    free(P);
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
    void* ptr = realloc(P, size);

    return ptr;
}

u32 xrMemory::mem_usage(u32* pBlocksUsed, u32* pBlocksFree) { return u32(mem_usage_impl(pBlocksUsed, pBlocksFree)); }

void GetProcessMemInfo(SProcessMemInfo& minfo)
{
    std::memset(&minfo, 0, sizeof(SProcessMemInfo));

    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);

    minfo.TotalPhysicalMemory = mem.ullTotalPhys;
    minfo.FreePhysicalMemory = mem.ullAvailPhys;

    minfo.TotalPageFile = mem.ullTotalPageFile - mem.ullTotalPhys;

    if (minfo.TotalPageFile > 0l)
    {
        // эта херня погоду показывает на самом деле. надо найти способ как получить свободный размер файла подкачки
        minfo.FreePageFile = mem.ullAvailPageFile > mem.ullAvailPhys ? mem.ullAvailPageFile - mem.ullAvailPhys : mem.ullAvailPageFile;
    }

    minfo.MemoryLoad = mem.dwMemoryLoad;

    PROCESS_MEMORY_COUNTERS pc;
    std::memset(&pc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
    pc.cb = sizeof(pc);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pc, sizeof(pc)))
    {
        minfo.WorkingSetSize = pc.WorkingSetSize;
        minfo.PeakWorkingSetSize = pc.PeakWorkingSetSize;

        minfo.PagefileUsage = pc.PagefileUsage;
        minfo.PeakPagefileUsage = pc.PeakPagefileUsage;
    }

#ifdef USE_MIMALLOC
    Log("####################[+MIMALLOC+]####################");
    mi_stats_print_out(
        [](const char* msg, void*) {
            std::string str{msg};
            xr_string_utils::rtrim(str);
            Log(str);
        },
        nullptr);
    Log("####################[-MIMALLOC-]####################");
#endif
}

size_t mem_usage_impl(u32* pBlocksUsed, u32* pBlocksFree)
{
    static bool no_memory_usage = !strstr(Core.Params, "-memory_usage");
    if (no_memory_usage)
        return 0;

    _HEAPINFO hinfo;
    int heapstatus;
    hinfo._pentry = nullptr;
    size_t total = 0;
    u32 blocks_free = 0;
    u32 blocks_used = 0;
    while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
    {
        if (hinfo._useflag == _USEDENTRY)
        {
            total += hinfo._size;
            blocks_used += 1;
        }
        else
        {
            blocks_free += 1;
        }
    }
    if (pBlocksFree)
        *pBlocksFree = 1024 * blocks_free;
    if (pBlocksUsed)
        *pBlocksUsed = 1024 * blocks_used;

    switch (heapstatus)
    {
    case _HEAPEMPTY: break;
    case _HEAPEND: break;
    case _HEAPBADPTR: Msg("!![%s] bad pointer to heap", __FUNCTION__); break;
    case _HEAPBADBEGIN: Msg("!![%s] bad start of heap", __FUNCTION__); break;
    case _HEAPBADNODE: Msg("!![%s] bad node in heap", __FUNCTION__); break;
    }
    return total;
}