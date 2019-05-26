#pragma once
#include <nvapi.h>

#define NVAPI_MAX_USAGES_PER_GPU  34
class ENGINE_API CNvReader
{
	typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
	typedef int(*NvAPI_Initialize_t)();
	typedef int(*NvAPI_EnumPhysicalGPUs_t)(int **handles, unsigned long *count);
	typedef int(*NvAPI_EnumLogicalGPUs_t)(int **handles, unsigned long *count);
	typedef int(*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);
	typedef int(*NvAPI_PhysicalFromLogical)(int* handle1, int** handle, unsigned long* count);

private:
	NvAPI_QueryInterface_t      NvAPI_QueryInterface;
	NvAPI_Initialize_t          NvAPI_Initialize;
	NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs;
	NvAPI_EnumLogicalGPUs_t     NvAPI_EnumLogicalGPUs;
	NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages;
	NvAPI_PhysicalFromLogical   NvAPI_GPU_PhysicalFromLogical;

	int*	gpuHandlesPh[NVAPI_MAX_PHYSICAL_GPUS];
	int*	gpuHandlesLg[NVAPI_MAX_LOGICAL_GPUS];
	u32		gpuUsages[NVAPI_MAX_USAGES_PER_GPU];
	ULONG	AdapterID;
	u64		AdapterFinal;

private:
	void	InitDeviceInfo();
	void	MakeGPUCount();

public:
	CNvReader();
	~CNvReader();

	u32		GetPercentActive();
	u32		GetGPUCount();

public:
	static bool bSupport;
};

extern ENGINE_API CNvReader* NvData;