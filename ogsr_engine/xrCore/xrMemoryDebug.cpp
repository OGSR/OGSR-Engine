#include "stdafx.h"

#ifdef USE_MEMORY_VALIDATOR

#include "xrMemoryDebug.h"

std::set<void*, std::less<void*>, PointerAllocator<void*>> gPointerRegistry;
std::mutex gPointerRegistryProtector;

void RegisterPointer(void* ptr)
{
    if (gModulesLoaded && ptr != nullptr)
    {
        gPointerRegistryProtector.lock();
        gPointerRegistry.insert(ptr);
        gPointerRegistryProtector.unlock();
    }
}

void UnregisterPointer(void* ptr)
{
    if (gModulesLoaded && ptr != nullptr)
    {
        gPointerRegistryProtector.lock();
        size_t elemErased = gPointerRegistry.erase(ptr);
        gPointerRegistryProtector.unlock();

        R_ASSERT2(elemErased == 1, "Pointer is not registered, heap can be corruped");
    }
}

#endif
