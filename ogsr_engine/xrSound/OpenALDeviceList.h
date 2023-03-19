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
    union ESndProps
    {
        struct
        {
            u16 selected : 1;
            u16 eax : 3;
            u16 efx : 1;
            u16 xram : 1;
            u16 eax_unwanted : 1;

            u16 unused : 9;
        };
        u16 storage;
    };
    ESndProps props{};
    ALDeviceDesc(LPCSTR nm, int mn, int mj)
    {
        xr_strcpy(name, nm);
        minor_ver = mn;
        major_ver = mj;
        props.storage = 0;
        props.eax_unwanted = true;
    }
};

class ALDeviceList
{
private:
    xr_vector<ALDeviceDesc> m_devices;
    string256 m_defaultDeviceName{};
    void Enumerate();

public:
    ALDeviceList();
    ~ALDeviceList();

    u32 GetNumDevices() { return m_devices.size(); }
    const ALDeviceDesc& GetDeviceDesc(u32 index) { return m_devices[index]; }
    LPCSTR GetDeviceName(u32 index);
    void GetDeviceVersion(u32 index, int* major, int* minor);
    void SelectBestDevice();
    bool IS_OpenAL_Soft = false;
};
