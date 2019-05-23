//////////////////////////////////////////
// Desc:   GPU Info
// Model:  AMD
// Author: ForserX
//////////////////////////////////////////
// Oxygen Engine (2016-2019)
//////////////////////////////////////////

#include "stdafx.h"
#include "AMDGPUTransferee.h"

CAMDReader AMDData;
#define AMDVENDORID 1002

bool CAMDReader::bAMDSupportADL = false;
void* CAMDReader::lpBuffer = nullptr;
static HINSTANCE hDLL;
static HINSTANCE hDLL_AGS;

CAMDReader::CAMDReader() : activity({ 0 }), AdapterID(-1), AdapterAGSInfo(0), ADLGpuCount(0)
{
	hDLL = LoadLibraryA("atiadlxx.dll");
	hDLL_AGS = LoadLibraryA("amd_ags_x64.dll");

	if (!bAMDSupportADL && hDLL)
	{
		bAMDSupportADL = true;

		Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(hDLL, "ADL_Main_Control_Create");
		GetAdapter_NumberOfAdapters = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hDLL, "ADL_Adapter_NumberOfAdapters_Get");
		GetAdapter_AdapterInfo = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hDLL, "ADL_Adapter_AdapterInfo_Get");
		GetAdapter_Active = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL_Adapter_Active_Get");
		GetOverdrive5_CurrentActivity = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hDLL, "ADL_Overdrive5_CurrentActivity_Get");
		Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(hDLL, "ADL_Main_Control_Destroy");

		Main_Control_Create(MemoryAllocator, 1);
		InitDeviceInfo();

		activity.iSize = sizeof(ADLPMActivity);
	}

	if (bAMDSupportADL && hDLL_AGS)
	{
		GetAGSCrossfireGPUCount = (AGS_GPU_COUNT_GET)GetProcAddress(hDLL_AGS, "agsGetCrossfireGPUCount");
		AGSCrossfireDeinit = (AGS_DEINIT)GetProcAddress(hDLL_AGS, "agsDeInit");
		AGSCrossfireInit = (AGS_INIT)GetProcAddress(hDLL_AGS, "agsInit");

		// 5.2 and later
		if (!GetAGSCrossfireGPUCount)
		{
			GetAGSCrossfireGPUCountExt = (AGS_DX11EXT)GetProcAddress(hDLL_AGS, "agsDriverExtensionsDX11_CreateDevice");
			AGSCrossfireGPUExtDestroy = (AGS_DX11EXTDestroy)GetProcAddress(hDLL_AGS, "agsDriverExtensionsDX11_DestroyDevice");
		}

		MakeGPUCount();
	}
}

CAMDReader::~CAMDReader()
{
	if (Main_Control_Destroy) Main_Control_Destroy();
	MemoryDeallocator();
	if (hDLL) FreeLibrary(hDLL);
	if (hDLL_AGS) FreeLibrary(hDLL_AGS);
}

void CAMDReader::InitDeviceInfo()
{
	GetAdapter_NumberOfAdapters(&AdapterADLInfo);
	auto lpAdapterInfo = std::make_unique<AdapterInfo[]>(AdapterADLInfo);
	RtlZeroMemory(lpAdapterInfo.get(), sizeof(AdapterInfo) * AdapterADLInfo);

	// Get the AdapterInfo structure for all adapters in the system
	GetAdapter_AdapterInfo(lpAdapterInfo.get(), sizeof(AdapterInfo) * AdapterADLInfo);

	for (size_t i = 0; i < (size_t)AdapterADLInfo; i++)
	{
		int adapterActive = 0;
		auto& adapterInfo = lpAdapterInfo[i];
		GetAdapter_Active(adapterInfo.iAdapterIndex, &adapterActive);
		if (adapterActive && adapterInfo.iVendorID == AMDVENDORID)
		{
			if (AdapterID == -1)
				AdapterID = adapterInfo.iAdapterIndex;
			ADLGpuCount++;
		}
	}
}

void CAMDReader::MakeGPUCount()
{
	AGSContext* ags = nullptr;
	AGSGPUInfo gpuInfo = {};
	AGSConfiguration* config = nullptr;
	AGSReturnCode status = AGSCrossfireInit(&ags, config, &gpuInfo);

	if (status != AGS_SUCCESS)
	{
		Msg("! AGS: Initialization failed (%d)", status);
		return;
	}

	if (GetAGSCrossfireGPUCount)
	{
		// FX: Old style for Win7 and lazy users
		// But, it's just a beautiful 
		status = GetAGSCrossfireGPUCount(ags, &AdapterAGSInfo);
		AdapterAGSInfo = AdapterAGSInfo ? AdapterAGSInfo : ADLGpuCount;
		Msg("~~[AGS] Used old ags driver...");
	}
	else
	{
		AGSDX11DeviceCreationParams creationParams = {};
		creationParams.SDKVersion = 7; // Skip debug output errors. crossfireGPUCount need only
		creationParams.FeatureLevels = 45312; // 11.1

		AGSDX11ExtensionParams extensionParams = {};
		// FX: Enable AFR without requiring a driver profile
		extensionParams.crossfireMode = AGS_CROSSFIRE_MODE_EXPLICIT_AFR;
		extensionParams.uavSlot = 7;
		AGSDX11ReturnedParams returnedParams = {};

		GetAGSCrossfireGPUCountExt(ags, &creationParams, &extensionParams, &returnedParams);
		AdapterAGSInfo = returnedParams.crossfireGPUCount ? returnedParams.crossfireGPUCount : ADLGpuCount;

		AGSCrossfireGPUExtDestroy(ags, nullptr, nullptr, nullptr, nullptr);
	}

	if (status != AGS_SUCCESS)
		Msg("!![AGS] Error! Unable to get CrossFire GPU count (%d)", status);
	else
		Msg("--[AGS] CrossFire GPU count: [%d]", AdapterAGSInfo);

	AGSCrossfireDeinit(ags);
}

u32 CAMDReader::GetPercentActive()
{
	GetOverdrive5_CurrentActivity(AdapterID, &activity);
	return activity.iActivityPercent;
}

u32 CAMDReader::GetGPUCount()
{
	return hDLL_AGS ? u32(AdapterAGSInfo) : u32(ADLGpuCount);
}
