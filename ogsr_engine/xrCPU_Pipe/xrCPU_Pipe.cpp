#include "stdafx.h"
#pragma hdrstop

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) { return TRUE; }

#ifndef _M_X64
extern xrSkin1W xrSkin1W_SSE;
extern xrSkin2W xrSkin2W_SSE;
extern xrSkin3W xrSkin3W_SSE;
extern xrSkin4W xrSkin4W_SSE;
#endif

extern xrSkin1W xrSkin1W_x86;
extern xrSkin2W xrSkin2W_x86;
extern xrSkin3W xrSkin3W_x86;
extern xrSkin4W xrSkin4W_x86;

xrSkin4W* skin4W_func = nullptr; //Оно разве используется где-то?

extern "C" {
	__declspec(dllexport) void	__cdecl	xrBind_PSGP	(xrDispatchTable* T)
	{
#ifdef _M_X64
		T->skin1W = xrSkin1W_x86;
		T->skin2W = xrSkin2W_x86;
		T->skin3W = xrSkin3W_x86;
		T->skin4W = xrSkin4W_x86;
		skin4W_func = xrSkin4W_x86;
#else
		T->skin1W = xrSkin1W_SSE;
		T->skin2W = xrSkin2W_SSE;
		T->skin3W = xrSkin3W_SSE;
		T->skin4W = xrSkin4W_SSE;
		skin4W_func = xrSkin4W_SSE;
#endif

		ttapi_Init(&CPU::ID); //Init helper threads
	}
};
