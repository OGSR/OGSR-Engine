#pragma once

#include <al.h>
#include <alc.h>

static constexpr const char* AL_GENERIC_HARDWARE = "Generic Hardware";
static constexpr const char* AL_GENERIC_SOFTWARE = "Generic Software";
static constexpr const char* AL_SOFT = "OpenAL Soft";

struct ALDeviceDesc{
	xr_string			name;
	int					minor_ver;
	int					major_ver;
	struct{
		u8				selected	:1;
		u8				xram		:1;
		u8				eax			:1;
		u8				efx			:1;
		u8				eax_unwanted:1;
	};
	ALDeviceDesc(LPCSTR nm, int mn, int mj) { name = nm; minor_ver = mn; major_ver = mj; selected = false; xram = false; eax = false; efx = false; eax_unwanted = true; }
};

class ALDeviceList
{
private:
	xr_vector<ALDeviceDesc>	m_devices;
	xr_string			m_defaultDeviceName;
	int					m_defaultDeviceIndex;
	int					m_filterIndex;
	void				Enumerate				();
public:
						ALDeviceList			();
						~ALDeviceList			();

	bool IS_OpenAL_Soft = false; //Чтобы можно было менять dll-ку без необходимости пересборки движка.
	int					GetNumDevices			()				{return m_devices.size();}
	const ALDeviceDesc&	GetDeviceDesc			(int index)		{return m_devices[index];}
	const xr_string&	GetDeviceName			(int index)		{return m_devices[index].name;}
	void				GetDeviceVersion		(int index, int *major, int *minor);
	int					GetDefaultDevice		()				{return m_defaultDeviceIndex;};
	void				FilterDevicesMinVer		(int major, int minor);
	void				FilterDevicesMaxVer		(int major, int minor);
	void				FilterDevicesXRAMOnly	();
	void				ResetFilters			();
	int					GetFirstFilteredDevice	();
	int					GetNextFilteredDevice	();
	void				SelectBestDevice		();
};

