// xrCPU_Pipe.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#pragma hdrstop

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

extern "C" {
#pragma todo("KRodin: вернуть сюда обычные (не ассемблерные) функции для x64!")
	__declspec(dllexport) void	__cdecl	xrBind_PSGP	(xrDispatchTable* T)
	{
		// generic
#ifndef _M_X64
		T->skin1W = xrSkin1W_SSE;
		T->skin2W = xrSkin2W_SSE;
		T->skin3W = xrSkin3W_SSE;
		T->skin4W = xrSkin4W_SSE;

 //KRodin: а какой вообще смысл проверять, есть поддержка SSE или нет? У нас ведь даже компилятору разрешено юзать SEE, значит, на процессорах без SSE движок даже не запустится.
		if (CPU::ID.hasSSE())
			skin4W_func = xrSkin4W_SSE;
#endif

		// Init helper threads
		ttapi_Init(&CPU::ID);

		/*if (ttapi_GetWorkersCount() > 1) {
			// We can use threading
			T->skin4W = xrSkin4W_thread;
		}*/
	}
};
