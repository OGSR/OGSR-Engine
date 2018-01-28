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

#pragma warning(push)
#pragma warning(disable:4995)
#include <objbase.h>
#pragma warning(pop)


/* 
 * Init call
 */
ALDeviceList::ALDeviceList()
{
	m_defaultDeviceIndex		= -1;
	Enumerate();
}

/* 
 * Exit call
 */
ALDeviceList::~ALDeviceList()
{
}

void ALDeviceList::Enumerate()
{
	char *devices;
	int major, minor, index;
	const char *actualDeviceName;

	Msg("[OpenAL] enumerate devices...");
	// have a set of vectors storing the device list, selection status, spec version #, and XRAM support status
	// -- empty all the lists and reserve space for 10 devices
	m_devices.clear();
	
	CoUninitialize();
	// grab function pointers for 1.0-API functions, and if successful proceed to enumerate all devices
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT")) 
	{
		Msg("[OpenAL] EnumerationExtension Present");

		devices = (char *)alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

		m_defaultDeviceName	= (char *)alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
		Msg("[OpenAL] system default SndDevice name is [%s]", m_defaultDeviceName.c_str());
		
		index = 0;
		// go through device list (each device terminated with a single NULL, list terminated with double NULL)
		while (*devices != NULL) 
		{
			ALCdevice *device		= alcOpenDevice(devices);
			if (device) 
			{
				ALCcontext *context = alcCreateContext(device, NULL);
				if (context) 
				{
					alcMakeContextCurrent(context);
					// if new actual device name isn't already in the list, then add it...
					actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);

					if ( (actualDeviceName != nullptr) && strlen(actualDeviceName) > 0 ) 
					{
						alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(int), &major);
						alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(int), &minor);
						m_devices.push_back(ALDeviceDesc(actualDeviceName, minor, major));
						
						if(!IS_OpenAL_Soft)
							IS_OpenAL_Soft = !stricmp(m_devices.back().name.c_str(), AL_SOFT);

						if (IS_OpenAL_Soft)
						{
							m_devices.back().efx = alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX");
							m_devices.back().xram = alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "EAX_RAM");
						}
						else
						{
							m_devices.back().xram = (alIsExtensionPresent("EAX-RAM") == TRUE);
							m_devices.back().eax = (alIsExtensionPresent("EAX2.0") == TRUE);
						}

						// KD: disable unwanted eax flag to force eax on all devices
						m_devices.back().eax_unwanted	= 0;/*((0==xr_strcmp(actualDeviceName,AL_GENERIC_HARDWARE))||
														   (0==xr_strcmp(actualDeviceName,AL_GENERIC_SOFTWARE)));*/
						++index;
					}
					alcDestroyContext(context);
				}else
					Msg("[OpenAL] cant create context for [%s]",device);
				alcCloseDevice(device);
			}else
				Msg("[OpenAL] cant open device [%s]",devices);

			devices += xr_strlen(devices) + 1;
		}
	}else
		Msg("[OpenAL] EnumerationExtension NOT Present");

	// KRodin: по умолчанию почему-то устанавливается девайс Generic Software и из-за этого у меня эхо в наушниках.
	// Неплохо бы сделать в меню возможность выбора девайса (в ЗП так сделано, если память не изменяет).
	// Пока же просто включаю использование OpenAL Soft принудительно.
	// Надо ещё подумать, что делать с системами, у которых Generic Hardware.
	// Может вообще всегда устанавливать принудительное использование OpenAL Soft ?
	// Наверное так и сделаю:
	if (IS_OpenAL_Soft)
	{
		m_defaultDeviceName = AL_SOFT;
		Msg("[OpenAL] default SndDevice name set to [%s]", m_defaultDeviceName.c_str());
	}

	ResetFilters();

	if(0!=GetNumDevices())
		Msg("[OpenAL] All available devices:");

	int majorVersion, minorVersion;
	for (int i = 0; i < GetNumDevices(); i++)
	{
		GetDeviceVersion		(i, &majorVersion, &minorVersion);
		Msg	("    %d. %s, Spec Version %d.%d %s", 
			i+1, 
			GetDeviceName(i).c_str(), 
			majorVersion, 
			minorVersion,
			(GetDeviceName(i)==m_defaultDeviceName)? "(default)":"" );
	}
	if (!strstr(GetCommandLine(), "-editor"))
		CoInitializeEx (NULL, COINIT_MULTITHREADED);
}

void ALDeviceList::SelectBestDevice()
{
	m_defaultDeviceIndex	= -1;
	int best_majorVersion	= -1;
	int best_minorVersion	= -1;
	int majorVersion, minorVersion;
	for (int i = 0; i < GetNumDevices(); i++)
	{
		if( m_defaultDeviceName!=GetDeviceName(i) )continue;

		GetDeviceVersion		(i, &majorVersion, &minorVersion);
		if( (majorVersion>best_majorVersion) ||
			(majorVersion==best_majorVersion && minorVersion>best_minorVersion) )
		{
			best_majorVersion		= majorVersion;
			best_minorVersion		= minorVersion;
			m_defaultDeviceIndex	= i;
		}
	}
	if(m_defaultDeviceIndex==-1)
	{ // not selected
		R_ASSERT(GetNumDevices()!=0);
		m_defaultDeviceIndex = 0; //first
	};

	if(GetNumDevices()==0)
		Msg("[OpenAL] SelectBestDevice: list empty");
	else
		Msg("[OpenAL] SelectBestDevice is [%s %d.%d]",GetDeviceName(m_defaultDeviceIndex).c_str(),best_majorVersion,best_minorVersion);
}

/*
 * Returns the major and minor version numbers for a device at a specified index in the complete list
 */
void ALDeviceList::GetDeviceVersion(int index, int *major, int *minor)
{
	*major = m_devices[index].major_ver;
	*minor = m_devices[index].minor_ver;
	return;
}

/* 
 * Deselects devices which don't have the specified minimum version
 */
void ALDeviceList::FilterDevicesMinVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < m_devices.size(); i++) {
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor < major) || ((dMajor == major) && (dMinor < minor))) 
			m_devices[i].selected = false;
	}
}

/* 
 * Deselects devices which don't have the specified maximum version
 */
void ALDeviceList::FilterDevicesMaxVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < m_devices.size(); i++) {
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor > major) || ((dMajor == major) && (dMinor > minor))) {
			m_devices[i].selected = false;
		}
	}
}

/* 
 * Deselects devices which don't have XRAM support
 */
void ALDeviceList::FilterDevicesXRAMOnly()
{
	for (unsigned int i = 0; i < m_devices.size(); i++) {		
		if (m_devices[i].xram == false) {
			m_devices[i].selected = false;
		}
	}
}

/*
 * Resets all filtering, such that all devices are in the list
 */
void ALDeviceList::ResetFilters()
{
	for (int i = 0; i < GetNumDevices(); i++)
		m_devices[i].selected = true;
	m_filterIndex = 0;
}

/*
 * Gets index of first filtered device
 */
int ALDeviceList::GetFirstFilteredDevice()
{
	int i = 0;
	for (; i < GetNumDevices(); i++) {
		if (m_devices[i].selected == true)
			break;
	}
	m_filterIndex = i + 1;
	return i;
}

/*
 * Gets index of next filtered device
 */
int ALDeviceList::GetNextFilteredDevice()
{
	int i = m_filterIndex;
	for (; i < GetNumDevices(); i++) {
		if (m_devices[i].selected == true)
			break;
	}
	m_filterIndex = i + 1;
	return i;
}
