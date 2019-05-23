#pragma once
#include <adl_sdk.h>
#include <amd_ags.h>

class ENGINE_API CAMDReader
{
	typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
	typedef int(*ADL_ADAPTER_ACTIVE_GET) (int, int*);
	typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
	typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
	typedef int(*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int iAdapterIndex, ADLPMActivity *lpActivity);
	typedef int(*ADL_MAIN_CONTROL_DESTROY)();

	typedef AGSReturnCode(*AGS_GPU_COUNT_GET)(AGSContext* context, int* numGPUs);
	typedef AGSReturnCode(*AGS_DEINIT)(AGSContext* context);
	typedef AGSReturnCode(*AGS_INIT)(AGSContext** context, const AGSConfiguration* config, AGSGPUInfo* gpuInfo);

	// AGS: 5.2-5.3
	typedef AGSReturnCode(*AGS_DX11EXT)(AGSContext* context, const AGSDX11DeviceCreationParams* creationParams, const AGSDX11ExtensionParams* extensionParams, AGSDX11ReturnedParams* returnedParams);
	typedef AGSReturnCode(*AGS_DX11EXTDestroy)(AGSContext* context, ID3D11Device* device, unsigned int* deviceReferences, ID3D11DeviceContext* immediateContext, unsigned int* immediateContextReferences);

private:
	// Memory allocation function
	static void* __stdcall MemoryAllocator(int iSize)
	{
		lpBuffer = xr_malloc(iSize);
		return lpBuffer;
	}

	// Optional Memory de-allocation function
	static void MemoryDeallocator()
	{
		if (lpBuffer)
		{
			xr_free(lpBuffer);
			lpBuffer = nullptr;
		}
	}

private:
	ADL_MAIN_CONTROL_CREATE					Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY				Main_Control_Destroy = nullptr;

	ADL_ADAPTER_ADAPTERINFO_GET				GetAdapter_AdapterInfo;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET		GetAdapter_NumberOfAdapters;
	ADL_ADAPTER_ACTIVE_GET					GetAdapter_Active;
	ADL_OVERDRIVE5_CURRENTACTIVITY_GET		GetOverdrive5_CurrentActivity;

	AGS_DX11EXT								GetAGSCrossfireGPUCountExt;
	AGS_GPU_COUNT_GET						GetAGSCrossfireGPUCount;
	AGS_DEINIT								AGSCrossfireDeinit;
	AGS_INIT								AGSCrossfireInit;
	AGS_DX11EXTDestroy						AGSCrossfireGPUExtDestroy;

	int				AdapterID;
	int				AdapterADLInfo;
	int				ADLGpuCount;
	int				AdapterAGSInfo;
	static void*	lpBuffer;
	ADLPMActivity	activity;

private:
	void	InitDeviceInfo	();
	void	MakeGPUCount	();

public:
			CAMDReader		();
			~CAMDReader		();

	u32		GetPercentActive();
	u32		GetGPUCount		();

public:
	static bool bAMDSupportADL;
};

extern ENGINE_API CAMDReader AMDData;