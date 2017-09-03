#include "stdafx.h"
#pragma hdrstop

#include "cpuid.h"
#ifndef _EDITOR
#include <intrin.h>
#include <VersionHelpers.h>
#endif

/***
*
* int _cpuid (_p_info *pinfo)
*
* Entry:
*
*   pinfo: pointer to _p_info, NULL is not allowed!
*
* Exit:
*
*   Returns int with capablity bit set.
*
****************************************************/
#ifdef _EDITOR
int _cpuid(_processor_info *pinfo)
{
	ZeroMemory(pinfo, sizeof(_processor_info));

	pinfo->feature = _CPU_FEATURE_SSE;
	return pinfo->feature;
}
#else

#undef _CPUID_DEBUG

int _cpuid(_processor_info *pinfo)
{
	_processor_info&	P = *pinfo;
	ZeroMemory(&P, sizeof(_processor_info));

	int cpinfo[4];
	// detect cpu vendor
	__cpuid(cpinfo, 0);
	bool is_one_core = !!(cpinfo[0] < 4);

	memcpy(P.v_name, &(cpinfo[1]), sizeof(int));
	memcpy(P.v_name + sizeof(int), &(cpinfo[3]), sizeof(int));
	memcpy(P.v_name + 2 * sizeof(int), &(cpinfo[2]), sizeof(int));

	// detect cpu model
	__cpuid(cpinfo, 0x80000002);
	memcpy(P.model_name, cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000003);
	memcpy(P.model_name + 16, cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000004);
	memcpy(P.model_name + 32, cpinfo, sizeof(cpinfo));

	// detect cpu main features
	__cpuid(cpinfo, 1);

	P.stepping = cpinfo[0] & 0xf;
	P.model = (u8)((cpinfo[0] >> 4) & 0xf) | ((u8)((cpinfo[0] >> 16) & 0xf) << 4);
	P.family = (u8)((cpinfo[0] >> 8) & 0xf) | ((u8)((cpinfo[0] >> 20) & 0xff) << 4);

	/*	if (cpinfo[3] & (1 << 23))
	P.feature |= _CPU_FEATURE_MMX;*/
	if (cpinfo[3] & (1 << 25))
		P.feature |= _CPU_FEATURE_SSE;
	if (cpinfo[3] & (1 << 26))
		P.feature |= _CPU_FEATURE_SSE2;
	if (cpinfo[2] & 0x1)
		P.feature |= _CPU_FEATURE_SSE3;
	if (cpinfo[2] & (1 << 19))
		P.feature |= _CPU_FEATURE_SSE41;
	if (cpinfo[2] & (1 << 20))
		P.feature |= _CPU_FEATURE_SSE42;
	if (cpinfo[2] & (1 << 9))
		P.feature |= _CPU_FEATURE_SSSE3;
	if (cpinfo[2] & (1 << 3))
		P.feature |= _CPU_FEATURE_MWAIT;
	if (cpinfo[2] & (1 << 28))
		P.feature |= _CPU_FEATURE_AVX;
	if (cpinfo[3] & (1 << 28))
		P.feature |= _CPU_FEATURE_HTT;

	// and check 3DNow! support
	/*	__cpuid(cpinfo, 0x80000001);
	if (cpinfo[3] & (1 << 31))
	P.feature |= _CPU_FEATURE_3DNOW;*/

	P.os_support = P.feature;

	// get version of OS
	/*	DWORD dwMajorVersion = 0;
	DWORD dwVersion = 0;
	dwVersion = GetVersion();

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));*/

	if (!IsWindowsVistaOrGreater())		// XP don't support SSE3+ instruction sets
	{
		P.os_support &= ~_CPU_FEATURE_SSE3;
		P.os_support &= ~_CPU_FEATURE_SSE41;
		P.os_support &= ~_CPU_FEATURE_SSE42;
		P.os_support &= ~_CPU_FEATURE_AVX;
	}

	u8 lpid_width = 0;
	u8 max_number_of_IDs_for_lprocessors = (cpinfo[1] >> 16) & 0xff;
	u8 max_number_of_IDs_for_cores = 1;
	if (!is_one_core)
	{
		int cpinfo_1[4];
		__cpuid(cpinfo_1, 4);
		max_number_of_IDs_for_cores = ((cpinfo_1[0] >> 26) & 0x3F) + 1;
	}
	u8 SMTIDPerCoreMaxCnt = (u8)floor(max_number_of_IDs_for_lprocessors / max_number_of_IDs_for_cores);
	if (SMTIDPerCoreMaxCnt > 1)
	{
		// HTT enabled
		u8 i = 0;
		// NearestPo2(numEntries) is the nearest power of 2 integer that is not less than numEntries
		// The most significant bit of (numEntries * 2 -1) matches the above definition
		u8 max_matches = SMTIDPerCoreMaxCnt * 2 - 1;

		// BSR emulation (let's find most significant bit)
		for (i = (8 * sizeof(u8)); i > 0; i--) {
			if ((max_matches & (1 << (i - 1))) != 0)
				break;
		}
		lpid_width = i;
	}

	// Calculate available processors
	DWORD pa_mask_save, sa_mask_stub, pa_mask_test, proc_count = 0;

	GetProcessAffinityMask(GetCurrentProcess(), &pa_mask_save, &sa_mask_stub);

	pa_mask_test = pa_mask_save;
	while (pa_mask_test) {
		if (pa_mask_test & 0x01)
			++proc_count;
		pa_mask_test >>= 1;
	}

	// All logical processors
	P.n_threads = proc_count;

	// easy case, HT is not possible at all
	if (lpid_width == 0) {
		P.affinity_mask = pa_mask_save;
		P.n_cores = proc_count;
		return P.feature;
	}

	// create APIC ID list
	DWORD dwAPIC_IDS[256], dwNums[256], n_cpu = 0, n_avail = 0, dwAPIC_ID, ta_mask;

	pa_mask_test = pa_mask_save;
	while (pa_mask_test) {
		if (pa_mask_test & 0x01) {
			// Switch thread to specific CPU
			ta_mask = (1 << n_cpu);
			SetThreadAffinityMask(GetCurrentThread(), ta_mask);
			Sleep(100);
			// get APIC ID
			/*			__asm {
			mov		eax, 01h
			cpuid
			shr		ebx, 24
			and ebx, 0FFh
			mov		DWORD PTR[dwAPIC_ID], ebx
			}*/
			__cpuid(cpinfo, 1);
			dwAPIC_ID = (cpinfo[1] >> 24) & 0xff;

#ifdef _CPUID_DEBUG
			char mask[255];
			_itoa_s(dwAPIC_ID, mask, 2);
			printf("APID_ID #%2.2u = 0x%2.2X (%08.8sb)\n", n_avail, dwAPIC_ID, mask);
#endif // _CPUID_DEBUG

			// search for the APIC_ID with the same base
			BOOL bFound = FALSE;
			for (DWORD i = 0; i < n_avail; ++i)
				if ((dwAPIC_ID >> lpid_width) == (dwAPIC_IDS[i] >> lpid_width)) {
					bFound = TRUE;
					break;
				}
			if (!bFound) {
				// add unique core
				dwNums[n_avail] = n_cpu;
				dwAPIC_IDS[n_avail] = dwAPIC_ID;
				++n_avail;
			}
		}
		// pick the next logical processor
		++n_cpu;
		pa_mask_test >>= 1;
	}

	// restore original saved affinity mask
	SetThreadAffinityMask(GetCurrentThread(), pa_mask_save);
	Sleep(100);

	// Create recommended mask
	DWORD ta_rec_mask = 0;
	for (DWORD i = 0; i < n_avail; ++i)
		ta_rec_mask |= (1 << dwNums[i]);

	P.affinity_mask = ta_rec_mask;
	P.n_cores = n_avail;

	return P.feature;
}
#endif
