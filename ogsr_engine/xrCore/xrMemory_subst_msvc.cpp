#include "stdafx.h"
#pragma hdrstop

#include "xrMemory_pure.h"
#include "xrMemory.h"

#ifndef	__BORLANDC__

#ifndef DEBUG_MEMORY_MANAGER
#	define	debug_mode 0
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
	XRCORE_API void*	g_globalCheckAddr = NULL;
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
	extern void save_stack_trace	();
#endif // DEBUG_MEMORY_MANAGER

#ifdef PURE_ALLOC
bool	g_use_pure_alloc		= false;
#endif // PURE_ALLOC

void*	xrMemory::mem_alloc		(size_t size
#	ifdef DEBUG_MEMORY_NAME
								 , const char* _name
#	endif // DEBUG_MEMORY_NAME
								 )
{
	stat_calls++;

	return malloc(size);
}

void	xrMemory::mem_free		(void* P)
{
	stat_calls++;

	free(P);
}

extern BOOL	g_bDbgFillMemory	;

void*	xrMemory::mem_realloc	(void* P, size_t size
#ifdef DEBUG_MEMORY_NAME
								 , const char* _name
#endif // DEBUG_MEMORY_NAME
								 )
{
	stat_calls++;

	return realloc(P, size);
}

#endif // __BORLANDC__