// xrCPU_Pipe.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#pragma hdrstop

//#pragma comment(lib,"xr_3DA")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern xrSkin1W			xrSkin1W_SSE;
extern xrSkin2W			xrSkin2W_SSE;
extern xrSkin3W			xrSkin3W_SSE;
extern xrSkin4W			xrSkin4W_SSE;

extern xrSkin4W			xrSkin4W_thread;

xrSkin4W* skin4W_func = NULL;
/*
extern xrPLC_calc3		PLC_calc3_x86;
extern xrPLC_calc3		PLC_calc3_SSE;
*/
extern "C" {
	__declspec(dllexport) void	__cdecl	xrBind_PSGP	(xrDispatchTable* T, _processor_info* ID)
	{
		// generic
		T->skin1W = xrSkin1W_SSE;
		T->skin2W = xrSkin2W_SSE;
		T->skin3W = xrSkin3W_SSE;
		T->skin4W = xrSkin4W_SSE;

		// SSE
		if (ID->feature & _CPU_FEATURE_SSE) {
			skin4W_func = xrSkin4W_SSE;
//			T->PLC_calc3 = PLC_calc3_SSE;
		}

		// Init helper threads
		ttapi_Init(ID);

		if (ttapi_GetWorkersCount() > 1) {
			// We can use threading
//			T->skin4W = xrSkin4W_thread;
		}
	}
};
