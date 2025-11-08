/*
 * Copyright (c) 2005, Creative Labs Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdafx.h"

#include "OpenALDeviceList.h"

//#include <objbase.h>

ALDeviceList::ALDeviceList()
{
    //snd_device_id = u32(-1);

    Enumerate();
}

/*
 * Exit call
 */
ALDeviceList::~ALDeviceList()
{
    for (int i = 0; snd_devices_token[i].name; i++)
    {
        xr_free(snd_devices_token[i].name);
    }
    xr_free(snd_devices_token);
    snd_devices_token = nullptr;
}

void ALDeviceList::IterateDevicesList(const char* devices, bool enumerateAllPresent)
{
    // go through device list (each device terminated with a single NULL, list terminated with double NULL)
    while (*devices)
    {
        ALCdevice* device{};
        // alcOpenDevice can fail without any visible reason. Just try several times
        for (u32 i{}; i < 100; ++i)
        {
            device = alcOpenDevice(devices);
            if (device != nullptr)
                break;
            else
                Sleep(1);
        }

        if (device)
        {
            if (ALCcontext* context = alcCreateContext(device, nullptr))
            {
                // alcMakeContextCurrent(context); // оно тут вроде б как не нужно, а ломает рестарт налету

                const bool is_al_soft = strstr(devices, AL_SOFT);

                // if new actual device name isn't already in the list, then add it...
                pcstr actualDeviceName = alcGetString(device, is_al_soft && enumerateAllPresent ? ALC_ALL_DEVICES_SPECIFIER : ALC_DEVICE_SPECIFIER);

                if (actualDeviceName != nullptr && xr_strlen(actualDeviceName) > 0)
                {
                    int major, minor;

                    alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(int), &major);
                    alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(int), &minor);

                    m_devices.emplace_back(actualDeviceName, minor, major, is_al_soft);
                }
                alcDestroyContext(context);
            }
            alcCloseDevice(device);
        }
        else
        {
            FATAL("Can't create sound device [%s]. Error: [%s]", devices, (LPCSTR)alGetString(alGetError()));
        }
        devices += xr_strlen(devices) + 1;
    }
}

void ALDeviceList::Enumerate()
{
    Msg("SOUND: OpenAL: enumerate devices...");

    // have a set of vectors storing the device list, selection status, spec version #, and XRAM support status
    m_devices.clear();

    // CoUninitialize(); // ???

    // grab function pointers for 1.0-API functions, and if successful proceed to enumerate all devices

    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
    {
        const char* devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

        IterateDevicesList(devices, true);

        xr_strcpy(m_defaultDeviceName, alcGetString(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER));
    }
    else if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT"))
    {
        const char* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);  

        IterateDevicesList(devices, false);

        xr_strcpy(m_defaultDeviceName, alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
    }
    else
        Msg("!!SOUND: OpenAL: EnumerationExtension NOT Present");

    u32 _cnt = GetNumDevices();

    const char* prefix = "OpenAL Soft on "; // to just cut off this long prefix in token names
    bool need_to_trim_prefix = true;

    for (u32 i = 0; i < _cnt; ++i)
    {
        char* str = m_devices[i].name;

        if (!strstr(str, prefix))
        {
            need_to_trim_prefix = false;
            break;
        }
    }

    // make token
    snd_devices_token = xr_alloc<xr_token>(_cnt + 1);

    snd_devices_token[_cnt].id = -1;
    snd_devices_token[_cnt].name = nullptr;

    for (u32 i = 0; i < _cnt; ++i)
    {
        char* str = m_devices[i].name;

        if (need_to_trim_prefix && strstr(str, prefix))
        {
            str += strlen(prefix);
        }

        snd_devices_token[i].id = i;
        snd_devices_token[i].name = xr_strdup(str);
    }

    if (need_to_trim_prefix && strstr(m_defaultDeviceName, prefix))
    {
        xr_strcpy(m_defaultDeviceName, m_defaultDeviceName + strlen(prefix));
    }

    Msg("~~SOUND: OpenAL: Default sound device name is [%s], device name size: [%u]", m_defaultDeviceName, strlen(m_defaultDeviceName));

    if (0 != GetNumDevices())
    {
        Msg("SOUND: OpenAL: All available devices:");

        for (u32 j = 0; j < GetNumDevices(); j++)
        {
            ALDeviceDesc al_device_desc = GetDeviceDesc(j);

            Msg("%d. %s (full name [%s]). al_soft [%d]", j + 1,
                snd_devices_token[j].name,
                al_device_desc.name, al_device_desc.is_al_soft);
        }
    }
    else
        Log("!!SOUND: OpenAL: No devices available.");

    //CoInitializeEx(NULL, COINIT_MULTITHREADED); // ???
}

void ALDeviceList::SelectBestDeviceId(const char* system_default_device) const
{
    if (GetNumDevices() == 0)
    {
        Msg("!!SOUND: Can't select device. List empty");
        snd_device_id = u32(-1);
    }
    else
    {
        if (snd_device_id == u32(-1) || snd_device_id >= GetNumDevices() || psSoundFlags.test(ss_UseDefaultDevice))
        {            
            R_ASSERT(GetNumDevices() != 0);

            // select best
            u32 new_device_id = 0; // first

            //if (snd_device_id == u32(-1) || psSoundFlags.test(ss_UseDefaultDevice))
            {
                for (int i = 0; snd_devices_token[i].name; i++)
                {
                    // check openAL default device first
                    if (m_defaultDeviceName[0] && _stricmp(m_defaultDeviceName, snd_devices_token[i].name) == 0)
                    {
                        new_device_id = i;
                        break;
                    }

                    // check OS system default device too
                    if (system_default_device && _stricmp(system_default_device, snd_devices_token[i].name) == 0)
                    {
                        new_device_id = i;
                        break;
                    }
                }
            }

            snd_device_id = new_device_id;
        }

        Msg("--SOUND: Selected device is [%s]", snd_devices_token[snd_device_id].name);
    }
}

/*
 * Returns the major and minor version numbers for a device at a specified index in the complete list
 */
void ALDeviceList::GetDeviceVersion(u32 index, int* major, int* minor) const
{
    *major = m_devices[index].major_ver;
    *minor = m_devices[index].minor_ver;
}
