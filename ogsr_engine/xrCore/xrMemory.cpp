#include "stdafx.h"
#include <psapi.h>
#include "xrsharedmem.h"
#ifdef USE_MEMORY_VALIDATOR
#include "xrMemoryDebug.h"
#endif


// Additional 16 bytes of memory almost like in original xr_aligned_offset_malloc
// But for DEBUG we don't need this if we want to find memory problems
#ifdef DEBUG
static constexpr size_t reserved = 0;
#else
static constexpr size_t reserved = 16;
#endif


xrMemory Memory;

void xrMemory::_initialize() {
  stat_calls = 0;

  g_pStringContainer       = xr_new<str_container>();
  g_pSharedMemoryContainer = xr_new<smem_container>();
}

void xrMemory::_destroy() {
  xr_delete( g_pSharedMemoryContainer );
  xr_delete( g_pStringContainer );
}

void xrMemory::mem_compact() {
  _heapmin(); //-V530
  HeapCompact( GetProcessHeap(), 0 );
  if ( g_pStringContainer )
    g_pStringContainer->clean();
  if ( g_pSharedMemoryContainer )
    g_pSharedMemoryContainer->clean();
}


void* xrMemory::mem_alloc( size_t size ) {
  stat_calls++;

  void* ptr = malloc( size + reserved );
#ifdef USE_MEMORY_VALIDATOR
  RegisterPointer( ptr );
#endif
  return ptr;
}

void xrMemory::mem_free( void* P ) {
  stat_calls++;

#ifdef USE_MEMORY_VALIDATOR
  UnregisterPointer( P );
#endif
  free( P );
}

void* xrMemory::mem_realloc( void* P, size_t size ) {
  stat_calls++;

#ifdef USE_MEMORY_VALIDATOR
  UnregisterPointer( P );
#endif
  void* ptr = realloc( P, size + reserved );
#ifdef USE_MEMORY_VALIDATOR
  RegisterPointer( ptr );
#endif
  return ptr;
}


void GetProcessMemInfo( SProcessMemInfo& minfo ) {
  std::memset( &minfo, 0, sizeof( SProcessMemInfo ) );

  MEMORYSTATUSEX mem;
  mem.dwLength = sizeof( mem );
  GlobalMemoryStatusEx( &mem );

  minfo.TotalPhysicalMemory = mem.ullTotalPhys;
  minfo.FreePhysicalMemory  = mem.ullAvailPhys;
  minfo.TotalVirtualMemory  = mem.ullTotalVirtual;
  minfo.MemoryLoad          = mem.dwMemoryLoad;

  PROCESS_MEMORY_COUNTERS pc;
  std::memset( &pc, 0, sizeof( PROCESS_MEMORY_COUNTERS ) );
  pc.cb = sizeof( pc );
  if ( GetProcessMemoryInfo( GetCurrentProcess(), &pc, sizeof( pc ) ) ) {
    minfo.PeakWorkingSetSize = pc.PeakWorkingSetSize;
    minfo.WorkingSetSize     = pc.WorkingSetSize;
    minfo.PagefileUsage      = pc.PagefileUsage;
    minfo.PeakPagefileUsage  = pc.PeakPagefileUsage;
  }
}

size_t mem_usage_impl( u32* pBlocksUsed, u32* pBlocksFree ) {
  static bool no_memory_usage = !!strstr( Core.Params, "-no_memory_usage" );
  if ( no_memory_usage )
    return 0;

  _HEAPINFO hinfo;
  int heapstatus;
  hinfo._pentry   = nullptr;
  size_t total    = 0;
  u32 blocks_free = 0;
  u32 blocks_used = 0;
  while ( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK ) {
    if ( hinfo._useflag == _USEDENTRY ) {
      total += hinfo._size;
      blocks_used += 1;
    } else {
      blocks_free += 1;
    }
  }
  if ( pBlocksFree )
    *pBlocksFree = 1024 * blocks_free;
  if ( pBlocksUsed )
    *pBlocksUsed = 1024 * blocks_used;

  switch ( heapstatus ) {
  case _HEAPEMPTY:
    break;
  case _HEAPEND:
    break;
  case _HEAPBADPTR:
    Msg( "!![%s] bad pointer to heap", __FUNCTION__ );
    break;
  case _HEAPBADBEGIN:
    Msg( "!![%s] bad start of heap", __FUNCTION__ );
    break;
  case _HEAPBADNODE:
    Msg( "!![%s] bad node in heap", __FUNCTION__ );
    break;
  }
  return total;
}

u32 xrMemory::mem_usage( u32* pBlocksUsed, u32* pBlocksFree ) {
  return u32( mem_usage_impl( pBlocksUsed, pBlocksFree ) );
}
