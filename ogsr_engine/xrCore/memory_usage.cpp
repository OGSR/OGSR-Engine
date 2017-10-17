#include "stdafx.h"
#include <malloc.h>

XRCORE_API void vminfo (size_t *_free, size_t *reserved, size_t *committed) {
	MEMORY_BASIC_INFORMATION memory_info;
	memory_info.BaseAddress = nullptr;
	*_free = *reserved = *committed = 0;
	while (VirtualQuery (memory_info.BaseAddress, &memory_info, sizeof (memory_info))) {
		switch (memory_info.State) {
		case MEM_FREE:
			*_free		+= memory_info.RegionSize;
			break;
		case MEM_RESERVE:
			*reserved	+= memory_info.RegionSize;
			break;
		case MEM_COMMIT:
			*committed += memory_info.RegionSize;
			break;
		}
		memory_info.BaseAddress = (char *) memory_info.BaseAddress + memory_info.RegionSize;
	}
}

XRCORE_API void log_vminfo	()
{
	size_t  w_free, w_reserved, w_committed;
	vminfo	(&w_free, &w_reserved, &w_committed);
	Msg		(
		"* [win32]: free[%d K], reserved[%d K], committed[%d K]",
		w_free/1024,
		w_reserved/1024,
		w_committed/1024
	);
}


u32	mem_usage_impl	(u32* pBlocksUsed, u32* pBlocksFree)
{
	static bool no_memory_usage = !!strstr(GetCommandLine(), "-no_memory_usage");
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
