#pragma once

#include <al.h>
#include <alc.h>

#define AL_GENERIC_HARDWARE "Generic Hardware"
#define AL_GENERIC_SOFTWARE "Generic Software"
#define AL_SOFT "OpenAL Soft"

struct ALDeviceDesc
{
    string256 name;

    int minor_ver;
    int major_ver;

    bool is_al_soft{};

    ALDeviceDesc(LPCSTR nm, int mn, int mj, bool al_soft)
    {
        xr_strcpy(name, nm);
        minor_ver = mn;
        major_ver = mj;
        is_al_soft = al_soft;
    }
};

class ALDeviceList
{
private:
    string512 m_defaultDeviceName{};

    xr_vector<ALDeviceDesc> m_devices;

    void Enumerate();
    void IterateDevicesList(const char* devices, bool enumerateAllPresent);

    void GetDeviceVersion(u32 index, int* major, int* minor) const;

public:
    ALDeviceList();
    ~ALDeviceList();

    u32 GetNumDevices() const { return m_devices.size(); }
    const ALDeviceDesc& GetDeviceDesc(u32 index) const { return m_devices[index]; }

    void SelectBestDeviceId(const char* system_default_device = nullptr) const;
};
