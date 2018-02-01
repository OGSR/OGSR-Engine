#pragma once

#ifndef NEW_TTAPI

#include "xrCore.h"

// Initializes subsystem
// Returns zero for error, and number of workers on success
u32 XRCORE_API ttapi_Init();

// Destroys subsystem
void XRCORE_API ttapi_Done();

// Return number of workers
u32 XRCORE_API ttapi_GetWorkersCount();

// Adds new task
// No more than TTAPI_HARDCODED_THREADS should be added
void XRCORE_API ttapi_AddWorker(std::function<void()> lpWorkerFunc);

// Runs and wait for all workers to complete job
void XRCORE_API ttapi_RunAllWorkers();

#endif
