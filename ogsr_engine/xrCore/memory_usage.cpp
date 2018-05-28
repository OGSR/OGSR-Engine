#include "stdafx.h"
#include <psapi.h>

void GetProcessMemInfo(SProcessMemInfo& minfo)
{
	std::memset(&minfo, 0, sizeof(SProcessMemInfo));

	MEMORYSTATUSEX mem;
	mem.dwLength = sizeof(mem);
	GlobalMemoryStatusEx(&mem);

	minfo.TotalPhysicalMemory = mem.ullTotalPhys;
	minfo.FreePhysicalMemory = mem.ullAvailPhys;
	minfo.TotalVirtualMemory = mem.ullTotalVirtual;
	minfo.MemoryLoad = mem.dwMemoryLoad;

	//--------------------------------------------------------------------
	PROCESS_MEMORY_COUNTERS pc;
	std::memset(&pc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
	pc.cb = sizeof(pc);
	if (GetProcessMemoryInfo(GetCurrentProcess(), &pc, sizeof(pc)))
	{
		minfo.PeakWorkingSetSize = pc.PeakWorkingSetSize;
		minfo.WorkingSetSize = pc.WorkingSetSize;
		minfo.PagefileUsage = pc.PagefileUsage;
		minfo.PeakPagefileUsage = pc.PeakPagefileUsage;
	}
}


u32	mem_usage_impl	(u32* pBlocksUsed, u32* pBlocksFree)
{
	static bool no_memory_usage = !!strstr(Core.Params, "-no_memory_usage");
	if (no_memory_usage)
		return		0;

	_HEAPINFO		hinfo;
	int				heapstatus;
	hinfo._pentry = nullptr;
	size_t	total = 0;
	u32	blocks_free = 0;
	u32	blocks_used = 0;
	while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
	{
		if (hinfo._useflag == _USEDENTRY) {
			total += hinfo._size;
			blocks_used += 1;
		}
		else {
			blocks_free += 1;
		}
	}
	if (pBlocksFree)	*pBlocksFree = 1024 * (u32)blocks_free;
	if (pBlocksUsed)	*pBlocksUsed = 1024 * (u32)blocks_used;

	switch (heapstatus)
	{
	case _HEAPEMPTY:
		break;
	case _HEAPEND:
		break;
	case _HEAPBADPTR:
		FATAL("bad pointer to heap");
		break;
	case _HEAPBADBEGIN:
		FATAL("bad start of heap");
		break;
	case _HEAPBADNODE:
		FATAL("bad node in heap");
		break;
	}
	return (u32)total;
}

u32		xrMemory::mem_usage		(u32* pBlocksUsed, u32* pBlocksFree)
{
	return mem_usage_impl(pBlocksUsed, pBlocksFree);
}
