/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#include "stdafx.h"

#ifdef XR_USE_BLACKBOX

#include "StackTrace.h"
#include "sym_engine.h"
#include <psapi.h>
#pragma comment(lib, "dbghelp.lib")

/*//////////////////////////////////////////////////////////////////////
File Scope Defines
//////////////////////////////////////////////////////////////////////*/
// The maximum symbol size handled in the module
#define MAX_SYM_SIZE 256
#define BUFF_SIZE 1024
#define SYM_BUFF_SIZE 512

/*//////////////////////////////////////////////////////////////////////
File Scope Global Variables
//////////////////////////////////////////////////////////////////////*/

// The original unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER g_pfnOrigFilt = NULL;

// The array of modules to limit crash handler to
static HMODULE* g_ahMod = NULL;
// The size, in items, of g_ahMod
static UINT g_uiModCount = 0;

// The static buffer returned by various functions. This buffer
// allows data to be transferred without using the stack.
static TCHAR g_szBuff[BUFF_SIZE];

// The static symbol lookup buffer
static BYTE g_stSymbol[SYM_BUFF_SIZE];

// The static source file and line number structure
static IMAGEHLP_LINE g_stLine;

// The stack frame used in walking the stack
static STACKFRAME g_stFrame;

// The flag indicating that the symbol engine has been initialized
static BOOL g_bSymEngInit = FALSE;

// The flag indicating that microsoft symbol server will be used
// static BOOL g_SymServerLookup = TRUE;

/*//////////////////////////////////////////////////////////////////////
File Scope Function Declarations
//////////////////////////////////////////////////////////////////////*/

// The internal function that does all the stack walking
LPCTSTR __stdcall InternalGetStackTraceString(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs);

// Initializes the symbol engine if needed
void InitializeSymbolEngine();

// Cleans up the symbol engine if needed
void DeinitializeSymbolEngine();

/*//////////////////////////////////////////////////////////////////////
Crash Handler Function Implementation
//////////////////////////////////////////////////////////////////////*/

LPCTSTR __stdcall GetFirstStackTraceString(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs) {
    // All the error checking is in the InternalGetStackTraceString
    // function.

    // Initialize the STACKFRAME structure.
    ZeroMemory(&g_stFrame, sizeof(STACKFRAME));

#ifdef _X86_
    g_stFrame.AddrPC.Offset = pExPtrs->ContextRecord->Eip;
    g_stFrame.AddrPC.Mode = AddrModeFlat;
    g_stFrame.AddrStack.Offset = pExPtrs->ContextRecord->Esp;
    g_stFrame.AddrStack.Mode = AddrModeFlat;
    g_stFrame.AddrFrame.Offset = pExPtrs->ContextRecord->Ebp;
    g_stFrame.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
    g_stFrame.AddrPC.Offset = pExPtrs->ContextRecord->Rip;
    g_stFrame.AddrPC.Mode = AddrModeFlat;
    g_stFrame.AddrReturn.Offset = 0;
    g_stFrame.AddrReturn.Mode = AddrModeFlat;
    g_stFrame.AddrStack.Offset = pExPtrs->ContextRecord->Rsp;
    g_stFrame.AddrStack.Mode = AddrModeFlat;
    g_stFrame.AddrFrame.Offset = pExPtrs->ContextRecord->Rbp;
    g_stFrame.AddrFrame.Mode = AddrModeFlat;
#else
    g_stFrame.AddrPC.Offset = (DWORD)pExPtrs->ContextRecord->Fir;
    g_stFrame.AddrPC.Mode = AddrModeFlat;
    g_stFrame.AddrReturn.Offset = (DWORD)pExPtrs->ContextRecord->IntRa;
    g_stFrame.AddrReturn.Mode = AddrModeFlat;
    g_stFrame.AddrStack.Offset = (DWORD)pExPtrs->ContextRecord->IntSp;
    g_stFrame.AddrStack.Mode = AddrModeFlat;
    g_stFrame.AddrFrame.Offset = (DWORD)pExPtrs->ContextRecord->IntFp;
    g_stFrame.AddrFrame.Mode = AddrModeFlat;
#endif

    return InternalGetStackTraceString(dwOpts, pExPtrs);
}

LPCTSTR __stdcall GetNextStackTraceString(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs) {
    // All error checking is in InternalGetStackTraceString.
    // Assume that GetFirstStackTraceString has already initialized the
    // stack frame information.
    return InternalGetStackTraceString(dwOpts, pExPtrs);
}

BOOL __stdcall ReadCurrentProcessMemory(HANDLE, LPCVOID lpBaseAddress, LPVOID lpBuffer, DWORD nSize,
#ifndef _M_X64
                                        LPDWORD lpNumberOfBytesRead)
#else
                                        SIZE_T* lpNumberOfBytesRead)
#endif
{
    return ReadProcessMemory(GetCurrentProcess(), lpBaseAddress, lpBuffer, nSize,
                             lpNumberOfBytesRead);
}

// The internal function that does all the stack walking
LPCTSTR __stdcall InternalGetStackTraceString(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs) {
    if (IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)) == TRUE) {
        SetLastError(ERROR_INVALID_ADDRESS);
        return nullptr;
    }

    // The value that is returned
    LPCTSTR szRet;
    // A temporary variable for all to use. This variable saves
    // stack space.
    DWORD dwTemp;
    // The module base address. I look this up right after the stack
    // walk to ensure that the module is valid.
    DWORD dwModBase;

    HANDLE hProcess = (HANDLE)GetCurrentProcessId();

    __try {
        // Initialize the symbol engine in case it isn't initialized.
        InitializeSymbolEngine();

#ifdef _M_X64
#define CH_MACHINE IMAGE_FILE_MACHINE_AMD64
#else
#define CH_MACHINE IMAGE_FILE_MACHINE_I386
#endif
        // Note:  If the source file and line number functions are used,
        //        StackWalk can cause an access violation.
        BOOL bSWRet =
            StackWalk(CH_MACHINE, hProcess, GetCurrentThread(), &g_stFrame, pExPtrs->ContextRecord,
                      (PREAD_PROCESS_MEMORY_ROUTINE)ReadCurrentProcessMemory,
                      SymFunctionTableAccess, SymGetModuleBase, NULL);

        if ((bSWRet == FALSE) || (g_stFrame.AddrFrame.Offset == 0)) {
            szRet = nullptr;
            __leave;
        }

        // Before I get too carried away and start calculating
        // everything, I need to double-check that the address returned
        // by StackWalk really exists. I've seen cases in which
        // StackWalk returns TRUE but the address doesn't belong to
        // a module in the process.
        dwModBase = SymGetModuleBase(hProcess, g_stFrame.AddrPC.Offset);
        if (dwModBase == 0) {
            //szRet = nullptr;
			//__leave;
			sprintf_s(g_szBuff, 1023, " [no module info for 0x%d] ", g_stFrame.AddrPC.Offset);
			szRet = g_szBuff;
        }

        int iCurr = 0;

// At a minimum, put in the address.
#ifdef _M_X64
        iCurr += wsprintf(g_szBuff + iCurr, _T("0x%016I64X"), g_stFrame.AddrPC.Offset);
#else
        // iCurr += wsprintf(g_szBuff + iCurr, _T("%04X:%08I64X"), pExPtrs->ContextRecord->SegCs,
        // g_stFrame.AddrPC.Offset);
        iCurr += wsprintf(g_szBuff + iCurr, _T("%04X:%08X"), pExPtrs->ContextRecord->SegCs,
                          g_stFrame.AddrPC.Offset);
#endif

        // Output the parameters?
        if ((dwOpts & GSTSO_PARAMS) == GSTSO_PARAMS) {
            iCurr += wsprintf(g_szBuff + iCurr,
#ifdef _M_X64
                              _T(" (0x%016I64X 0x%016I64X 0x%016I64X 0x%016I64X)"),
#else
                              _T(" (0x%08X 0x%08X 0x%08X 0x%08X)"),
#endif
                              g_stFrame.Params[0], g_stFrame.Params[1], g_stFrame.Params[2],
                              g_stFrame.Params[3]);
        }
        // Output the module name.
        if ((dwOpts & GSTSO_MODULE) == GSTSO_MODULE) {
            iCurr += wsprintf(g_szBuff + iCurr, _T(" "));
            iCurr += GetModuleBaseName(GetCurrentProcess(), (HINSTANCE)dwModBase, g_szBuff + iCurr,
                                       BUFF_SIZE - iCurr);
        }

        // ASSERT(iCurr < (BUFF_SIZE - MAX_PATH));
        DWORD_PTR dwDisp;
        // Output the symbol name?
        if ((dwOpts & GSTSO_SYMBOL) == GSTSO_SYMBOL) {
            // Start looking up the exception address.
            PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_stSymbol;
            ZeroMemory(pSym, SYM_BUFF_SIZE);
            pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
            pSym->MaxNameLength = SYM_BUFF_SIZE - sizeof(IMAGEHLP_SYMBOL);

            if (SymGetSymFromAddr(hProcess, g_stFrame.AddrPC.Offset, &dwDisp, pSym) == TRUE) {
                iCurr += wsprintf(g_szBuff + iCurr, _T(", "));
                // Copy no more symbol information than there's room for.
                dwTemp = lstrlen(pSym->Name);
                if (dwTemp > (DWORD)(BUFF_SIZE - iCurr - (MAX_SYM_SIZE + 50))) {
                    lstrcpyn(g_szBuff + iCurr, pSym->Name, BUFF_SIZE - iCurr - 1);
                    // Gotta leave now
                    szRet = g_szBuff;
                    __leave;
                } else {
                    if (dwDisp > 0) {
                        // iCurr += wsprintf(g_szBuff + iCurr, _T("%s()"), pSym->Name);
                        iCurr +=
                            wsprintf(g_szBuff + iCurr, _T("%s()"), pSym->Name);
                    } else {
                        iCurr += wsprintf(g_szBuff + iCurr, _T("%s"), pSym->Name);
                    }
                }
            } else {
                // If the symbol wasn't found, the source file and line
                // number won't be found either, so leave now.
                szRet = g_szBuff;
                __leave;
            }
        }

        // ASSERT(iCurr < (BUFF_SIZE - MAX_PATH));

        // Output the source file and line number information?
        if ((dwOpts & GSTSO_SRCLINE) == GSTSO_SRCLINE) {
            ZeroMemory(&g_stLine, sizeof(IMAGEHLP_LINE));
            g_stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

            if (SymGetLineFromAddr(hProcess, g_stFrame.AddrPC.Offset, (PDWORD)&dwDisp, &g_stLine) ==
                TRUE) {
                iCurr += wsprintf(g_szBuff + iCurr, _T(", "));

                // Copy no more of the source file and line number
                // information than there's room for.
                dwTemp = lstrlen(g_stLine.FileName);
                if (dwTemp > (DWORD)(BUFF_SIZE - iCurr - (MAX_PATH + 50))) {
                    lstrcpyn(g_szBuff + iCurr, g_stLine.FileName, BUFF_SIZE - iCurr - 1);
                    // Gotta leave now
                    szRet = g_szBuff;
                    __leave;
                } else {
                        iCurr += wsprintf(g_szBuff + iCurr, _T("%s, line %d"), g_stLine.FileName, g_stLine.LineNumber);
                }
            }
        }
        szRet = g_szBuff;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // int err = GetLastError();
        // char buf[1024];
        // sprintf(buf, "error %d", err);
        // MessageBoxA(0, buf, "Flux Engine", 0);
        // ASSERT(!"Crashed in InternalGetStackTraceString");
		Log("!!FATAL: inner exception in  InternalGetStackTraceString");
        szRet = NULL;
    }
    return szRet;
}

#undef min
#include <algorithm>

BOOL __stdcall GetFirstStackTraceStringVB(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs, LPTSTR szBuff,
                                          UINT uiSize) {
    // ASSERT(IsBadWritePtr(szBuff, uiSize) == FALSE);
    if (IsBadWritePtr(szBuff, uiSize) == TRUE) {
        return FALSE;
    }

    LPCTSTR szRet;

    __try {
        szRet = GetFirstStackTraceString(dwOpts, pExPtrs);
        if (NULL == szRet) {
            __leave;
        }
        lstrcpyn(szBuff, szRet, std::min((UINT)lstrlen(szRet) + 1, uiSize));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        szRet = NULL;
    }
    return szRet != NULL;
}

BOOL __stdcall GetNextStackTraceStringVB(DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs, LPTSTR szBuff,
                                         UINT uiSize) {
    // ASSERT(IsBadWritePtr(szBuff, uiSize) == FALSE);
    if (IsBadWritePtr(szBuff, uiSize) == TRUE)
        return FALSE;

    LPCTSTR szRet;
    __try {
        szRet = GetNextStackTraceString(dwOpts, pExPtrs);
        if (NULL == szRet)
            __leave;

        lstrcpyn(szBuff, szRet, std::min((UINT)lstrlen(szRet) + 1, uiSize));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        szRet = NULL;
    }
    return szRet != NULL;
}

// Initializes the symbol engine if needed
void InitializeSymbolEngine() {
    // static char const ms_symsrv[] = "http://msdl.microsoft.com/download/symbols";
    if (!g_bSymEngInit) {
        // Set up the symbol engine.
        DWORD dwOpts = SymGetOptions();
        // Turn on line loading and deferred loading.
        SymSetOptions(dwOpts | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_ANYTHING | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);

        HANDLE hProcess = (HANDLE)GetCurrentProcessId();
        SymInitialize(hProcess, NULL, TRUE);
        // if (g_SymServerLookup)
        //{
        //    SymSetSearchPath(hProcess, ms_symsrv);
        //}

        g_bSymEngInit = TRUE;
    }
}

// Cleans up the symbol engine if needed
void DeinitializeSymbolEngine() {
    if (g_bSymEngInit) {
        SymCleanup((HANDLE)GetCurrentProcessId());
        g_bSymEngInit = FALSE;
    }
}

#endif
