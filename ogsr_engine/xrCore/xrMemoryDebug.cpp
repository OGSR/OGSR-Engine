#include "stdafx.h"

#ifdef USE_MEMORY_VALIDATOR

#include "xrMemoryDebug.h"

static std::map<void*, PointerInfo, std::less<>, PointerAllocator<std::pair<void* const, PointerInfo>>> gPointerRegistry;
static std::recursive_mutex gPointerRegistryProtector;

BOOL g_enable_memory_debug = TRUE;
static BOOL g_enable_double_free_check = g_enable_memory_debug;

void PointerRegistryAdd(void* ptr, PointerInfo&& info)
{
    if (gModulesLoaded && g_enable_memory_debug && ptr != nullptr)
    {
        std::scoped_lock lock(gPointerRegistryProtector);

        gPointerRegistry.emplace(ptr, std::move(info));
    }
}

void PointerRegistryRelease(const void* ptr, const std::source_location& loc, const bool is_class)
{
    if (ExitFromWinMain)
        return;

    if (gModulesLoaded && g_enable_memory_debug && ptr != nullptr)
    {
        std::scoped_lock lock(gPointerRegistryProtector);

        if (const auto search = gPointerRegistry.find(ptr); search != gPointerRegistry.end())
        {
            if (search->second.is_class && !is_class)
            {
                Msg("!![%s]Wrong release call [%s.%u (%s)]! xr_free called for class ?. ID [%s]", __FUNCTION__, loc.file_name(), loc.line(), loc.function_name(),
                    search->second.identity.c_str());
            }

            gPointerRegistry.erase(search);
        }
        else if (g_enable_double_free_check)
        {
            Msg("!![%s] ptr [%p] not found in gPointerRegistry! Potential double-free! Called from: [%s.%u (%s)]", __FUNCTION__, ptr, loc.file_name(), loc.line(),
                loc.function_name());
        }
    }
}

void PointerRegistryClear()
{
    if (gModulesLoaded)
    {
        std::scoped_lock lock(gPointerRegistryProtector);
        gPointerRegistry.clear();

        g_enable_double_free_check = FALSE;
    }
}

void PointerRegistryDump(float thresholdInKb)
{
    if (gModulesLoaded)
    {
        std::scoped_lock lock(gPointerRegistryProtector);

        // cnt,size
        std::vector<std::pair<const char*, std::tuple<size_t, size_t>>, PointerAllocator<std::pair<const char*, std::tuple<size_t, size_t>>>> tmp;

        size_t cnt = gPointerRegistry.size();
        size_t size = 0;

        for (const auto& pair : gPointerRegistry)
        {
            if (const auto search = std::find_if(tmp.begin(), tmp.end(), [&pair](const auto& v) { return !strcmp(v.first, pair.second.identity.c_str()); }); search != tmp.end())
            {
                std::get<0>(search->second) += 1;
                std::get<1>(search->second) += pair.second.size;
            }
            else
            {
                tmp.emplace_back(pair.second.identity.c_str(), std::make_tuple(1, pair.second.size));
            }

            size += pair.second.size;
        }

        Msg("! xrMemory: instance count [%u]. total size [%.1f Kb]", cnt, static_cast<float>(size) / 1024.f);
        Msg("! xrMemory: dump (large that [%f Kb]):", thresholdInKb);

        std::sort(tmp.begin(), tmp.end(), [](const auto& a, const auto& b) { return std::get<1>(a.second) > std::get<1>(b.second); });

        for (const auto& [name, typle] : tmp)
        {
            const float total_size = static_cast<float>(std::get<1>(typle)) / 1024.f;
            if (total_size > thresholdInKb)
            {
                Msg(" total size:[%.3f Kb], instance count:[%u], id: %s", total_size, std::get<0>(typle), name);
            }
        }
    }
}

void PointerRegistryInfo()
{
    if (gModulesLoaded)
    {
        std::scoped_lock lock(gPointerRegistryProtector);

        size_t cnt = gPointerRegistry.size();
        size_t size = 0;

        for (const auto& pair : gPointerRegistry)
        {
            size += pair.second.size;
        }

        Msg("! xrMemory: instance count [%u]. total size [%.1f Kb]", cnt, static_cast<float>(size) / 1024.f);
    }
}

#endif
