#include "stdafx.h"

#include "xrdebug.h"

#include <new.h> // for _set_new_mode
#include <signal.h> // for signals

XRCORE_API xrDebug Debug;
XRCORE_API HWND gGameWindow = nullptr;
XRCORE_API bool ExitFromWinMain = false;

static bool error_after_dialog = false;

#include "stacktrace_collector.h"
#include <sstream>
#include <VersionHelpers.h>
#include <shellapi.h>

static void ShowErrorMessage(const char* msg, const bool show_msg = false)
{
    const bool on_ttapi_thread = (TTAPI && TTAPI->is_pool_thread());

    if (!on_ttapi_thread)
    {
        ShowWindow(gGameWindow, SW_HIDE);

        while (ShowCursor(TRUE) < 0)
            ;
    }

    if (!IsDebuggerPresent())
    {
        if (show_msg && !on_ttapi_thread)
            MessageBox(gGameWindow, msg, "FATAL ERROR", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        else
            ShellExecute(nullptr, "open", logFName, nullptr, nullptr, SW_SHOW);
    }
}

static const char* GetThreadName()
{
    if (IsWindows10OrGreater())
    {
        static HMODULE KernelLib = GetModuleHandle("kernel32.dll");
        using FuncGetThreadDescription = HRESULT (*)(HANDLE, PWSTR*);
        static auto pGetThreadDescription = (FuncGetThreadDescription)GetProcAddress(KernelLib, "GetThreadDescription");

        if (pGetThreadDescription)
        {
            PWSTR wThreadName = nullptr;
            if (SUCCEEDED(pGetThreadDescription(GetCurrentThread(), &wThreadName)))
            {
                if (wThreadName)
                {
                    static string64 ResThreadName{};
                    WideCharToMultiByte(CP_OEMCP, 0, wThreadName, int(wcslen(wThreadName)), ResThreadName, sizeof(ResThreadName), nullptr, nullptr);
                    LocalFree(wThreadName);
                    if (ResThreadName && strlen(ResThreadName))
                        return ResThreadName;
                }
            }
        }
    }
    return "UNKNOWN";
}

void LogStackTrace(const char* header, const bool dump_lua_locals)
{
    __try
    {
        if (auto pCrashHandler = Debug.get_crashhandler())
            pCrashHandler(dump_lua_locals);
        Log("********************************************************************************");
        Msg("!![" __FUNCTION__ "] Thread: [%s]", GetThreadName());
        Log(BuildStackTrace(header));
        Log("********************************************************************************");
    }
    __finally
    {}
}

void LogStackTrace(const char* header, _EXCEPTION_POINTERS* pExceptionInfo, bool dump_lua_locals)
{
    __try
    {
        if (auto pCrashHandler = Debug.get_crashhandler())
            pCrashHandler(dump_lua_locals);
        Log("********************************************************************************");
        Msg("!![" __FUNCTION__ "] Thread: [%s], ExceptionCode: [%x]", GetThreadName(), pExceptionInfo->ExceptionRecord->ExceptionCode);
        auto save = *pExceptionInfo->ContextRecord;
        Log(BuildStackTrace(header, pExceptionInfo->ContextRecord));
        *pExceptionInfo->ContextRecord = save;
        Log("********************************************************************************");
    }
    __finally
    {}
}

LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS* pExceptionInfo)
{
    LogStackTrace(header, pExceptionInfo);

    return EXCEPTION_EXECUTE_HANDLER;
}

void gather_info(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, int line, const char* function,
                 char* assertion_info)
{
    auto buffer = assertion_info;
    auto endline = "\n";
    auto prefix = "[error]";
    bool extended_description = (description && !argument0 && strchr(description, '\n'));
    for (int i = 0; i < 2; ++i)
    {
        if (!i)
            buffer += sprintf(buffer, "%sFATAL ERROR%s%s", endline, endline, endline);
        buffer += sprintf(buffer, "%sExpression    : %s%s", prefix, expression, endline);
        buffer += sprintf(buffer, "%sFunction      : %s%s", prefix, function, endline);
        buffer += sprintf(buffer, "%sFile          : %s%s", prefix, file, endline);
        buffer += sprintf(buffer, "%sLine          : %d%s", prefix, line, endline);

        if (extended_description)
        {
            buffer += sprintf(buffer, "%s%s%s", endline, description, endline);
            /*if (argument0) { //Этот код не выполнится. См. условие extended_description
                if (argument1) {
                    buffer += sprintf(buffer, "%s%s", argument0, endline);
                    buffer += sprintf(buffer, "%s%s", argument1, endline);
                }
                else
                    buffer += sprintf(buffer, "%s%s", argument0, endline);
            }*/
        }
        else
        {
            buffer += sprintf(buffer, "%sDescription   : %s%s", prefix, description, endline);
            if (argument0)
            {
                if (argument1)
                {
                    buffer += sprintf(buffer, "%sArgument 0    : %s%s", prefix, argument0, endline);
                    buffer += sprintf(buffer, "%sArgument 1    : %s%s", prefix, argument1, endline);
                }
                else
                    buffer += sprintf(buffer, "%sArguments     : %s%s", prefix, argument0, endline);
            }
        }

        buffer += sprintf(buffer, "%s", endline);
        if (!i)
        {
            Log(assertion_info);
            buffer = assertion_info;
            endline = "\r\n";
            prefix = "";
        }
    }

#ifdef USE_OWN_MINI_DUMP
    buffer += sprintf(buffer, "See log file and minidump for detailed information\r\n");
#else
    buffer += sprintf(buffer, "See log file for detailed information\r\n");
#endif
    LogStackTrace("!!stack trace:\n");
}

void xrDebug::do_exit(const std::string& message)
{
    ShowErrorMessage(message.c_str(), true);

    if (!IsDebuggerPresent())
        quick_exit(EXIT_SUCCESS);
    else
        DEBUG_INVOKE;
}

void xrDebug::backend(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, int line, const char* function)
{
    static std::recursive_mutex CS;
    std::scoped_lock<decltype(CS)> lock(CS);

    string4096 assertion_info;
    gather_info(expression, description, argument0, argument1, file, line, function, assertion_info);

    /* KRodin: у меня этот способ не работает - происходит исключение внутри функции save_mini_dump(). Если сильно надо будет тут получать минидампы - придумать другой способ.
    #ifdef USE_OWN_MINI_DUMP
        if ( !IsDebuggerPresent() )
            save_mini_dump(nullptr);
    #endif
    */
    auto endline = "\r\n";
    auto buffer = assertion_info + xr_strlen(assertion_info);
    buffer += sprintf(buffer, "%sPress OK to abort execution%s", endline, endline);

    error_after_dialog = true;

    ShowErrorMessage(assertion_info);

    if (!IsDebuggerPresent())
        quick_exit(EXIT_SUCCESS);
    else
        DEBUG_INVOKE;
}

const char* xrDebug::DXerror2string(const HRESULT code) const { return error2string(code); }

const char* xrDebug::error2string(const DWORD code) const
{
    static string1024 desc_storage;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, desc_storage, sizeof(desc_storage) - 1, 0);
    return desc_storage;
}

void xrDebug::error(const HRESULT hr, const char* expr, const char* file, int line, const char* function) { backend(DXerror2string(hr), expr, 0, 0, file, line, function); }

void xrDebug::error(const HRESULT hr, const char* expr, const char* e2, const char* file, int line, const char* function)
{
    backend(DXerror2string(hr), expr, e2, 0, file, line, function);
}

void xrDebug::fail(const char* e1, const char* file, int line, const char* function) { backend("assertion failed", e1, 0, 0, file, line, function); }

void xrDebug::fail(const char* e1, const std::string& e2, const char* file, int line, const char* function) { backend(e1, e2.c_str(), 0, 0, file, line, function); }

void xrDebug::fail(const char* e1, const char* e2, const char* file, int line, const char* function) { backend(e1, e2, 0, 0, file, line, function); }

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* file, int line, const char* function) { backend(e1, e2, e3, 0, file, line, function); }

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, int line, const char* function)
{
    backend(e1, e2, e3, e4, file, line, function);
}

void __cdecl xrDebug::fatal(const char* file, int line, const char* function, const char* F, ...)
{
    string4096 strBuf;
    va_list args;
    va_start(args, F);
    std::vsnprintf(strBuf, sizeof(strBuf), F, args);
    va_end(args);

    backend("FATAL ERROR", strBuf, nullptr, nullptr, file, line, function);
}

void xrDebug::on_exception_in_thread()
{
    if (!IsDebuggerPresent())
    {
        ShellExecute(nullptr, "open", logFName, nullptr, nullptr, SW_SHOW);

        quick_exit(EXIT_SUCCESS);
    }
}

static int out_of_memory_handler(size_t size)
{
    Memory.mem_compact();
    size_t process_heap = mem_usage_impl(nullptr, nullptr);
    u32 eco_strings = g_pStringContainer->stat_economy();
    u32 eco_smem = g_pSharedMemoryContainer->stat_economy();
    Msg("* [x-ray]: process heap[%d K]", process_heap / 1024);
    Msg("* [x-ray]: economy: strings[%d K], smem[%d K]", eco_strings / 1024, eco_smem);
    FATAL("Out of memory. Memory request: [%d K]", size / 1024);
    return 1;
}

#ifdef USE_OWN_MINI_DUMP
#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "dbghelp.lib")

static void save_mini_dump(_EXCEPTION_POINTERS* pExceptionInfo)
{
    __try
    {
        string_path szDumpPath;
        string64 t_stemp;

        timestamp(t_stemp);
        strcpy_s(szDumpPath, Core.ApplicationName);
        strcat_s(szDumpPath, "_");
        strcat_s(szDumpPath, Core.UserName);
        strcat_s(szDumpPath, "_");
        strcat_s(szDumpPath, t_stemp);
        strcat_s(szDumpPath, ".mdmp");

        __try
        {
            if (FS.path_exist("$logs$"))
                FS.update_path(szDumpPath, "$logs$", szDumpPath);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            string_path temp;
            strcpy_s(temp, szDumpPath);
            strcpy_s(szDumpPath, "logs/");
            strcat_s(szDumpPath, temp);
        }

        // create the file
        auto hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            // try to place into current directory
            MoveMemory(szDumpPath, szDumpPath + 5, strlen(szDumpPath));
            hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }
        if (hFile != INVALID_HANDLE_VALUE)
        {
            _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

            ExInfo.ThreadId = ::GetCurrentThreadId();
            ExInfo.ExceptionPointers = pExceptionInfo;
            ExInfo.ClientPointers = NULL;

            // write the dump
            auto dump_flags = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory);

            BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, nullptr, nullptr);
            if (bOK)
                Msg("--Saved dump file to [%s]", szDumpPath);
            else
                Msg("!!Failed to save dump file to [%s] (error [%s])", szDumpPath, Debug.error2string(GetLastError()));

            ::CloseHandle(hFile);
        }
        else
        {
            Msg("!!Failed to create dump file [%s] (error [%s])", szDumpPath, Debug.error2string(GetLastError()));
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Log("Exception catched in function [" __FUNCTION__ "]");
    }
}
#endif

static void format_message(char* buffer, const size_t& buffer_size)
{
    __try
    {
        auto error_code = GetLastError();
        if (error_code == ERROR_SUCCESS)
        {
            *buffer = 0;
            return;
        }

        void* message = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&message, 0, nullptr);

        sprintf(buffer, "[error][%8d] : [%s]", error_code, (char*)message);
        LocalFree(message);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Log("Exception catched in function [" __FUNCTION__ "]");
    }
}

static LONG WINAPI UnhandledFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
    if (!error_after_dialog)
    {
        string1024 error_message;
        format_message(error_message, sizeof(error_message));
        if (*error_message)
            Msg("\n%s", error_message);

        LogStackTrace("!!Unhandled exception stack trace:\n", pExceptionInfo, true);

        ShowErrorMessage("Fatal error occured\n\nPress OK to abort program execution");
    }

#ifdef USE_OWN_MINI_DUMP
    save_mini_dump(pExceptionInfo);
#endif

    return EXCEPTION_EXECUTE_HANDLER;
}

static void _terminate() //Вызывается при std::terminate()
{
    Debug.backend("<no expression>", "Unexpected application termination", nullptr, nullptr, DEBUG_INFO);
}

static void handler_base(const char* reason_string) { Debug.backend("error handler is invoked!", reason_string, nullptr, nullptr, DEBUG_INFO); }

static void invalid_parameter_handler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t reserved)
{
    string4096 expression_;
    string4096 function_;
    string4096 file_;
    size_t converted_chars = 0;

    if (expression)
        wcstombs_s(&converted_chars, expression_, sizeof(expression_), expression, (wcslen(expression) + 1) * 2 * sizeof(char));
    else
        strcpy_s(expression_, "");

    if (function)
        wcstombs_s(&converted_chars, function_, sizeof(function_), function, (wcslen(function) + 1) * 2 * sizeof(char));
    else
        strcpy_s(function_, __FUNCTION__);

    if (file)
        wcstombs_s(&converted_chars, file_, sizeof(file_), file, (wcslen(file) + 1) * 2 * sizeof(char));
    else
    {
        line = __LINE__;
        strcpy_s(file_, __FILE__);
    }

    Debug.backend("error handler is invoked!", expression_, nullptr, nullptr, file_, line, function_);
}

static void std_out_of_memory_handler() { handler_base("std: out of memory"); }

static void pure_call_handler() { handler_base("pure virtual function call"); }

static void abort_handler(int signal) { handler_base("application is aborting"); }

static void floating_point_handler(int signal) { handler_base("floating point error"); }

static void illegal_instruction_handler(int signal) { handler_base("illegal instruction"); }

static void termination_handler(int signal) { handler_base("termination with exit code 3"); }

/*static void segment_violation( int signal ) {
  handler_base( "Segment violation error" );
}*/

// http://qaru.site/questions/441696/what-actions-do-i-need-to-take-to-get-a-crash-dump-in-all-error-scenarios
static BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    if (!hKernel32)
        return FALSE;
    void* pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
    if (!pOrgEntry)
        return FALSE;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0                xor         eax,eax
    // C2 04 00             ret         4
    constexpr unsigned char szExecute[] = {0x33, 0xC0, 0xC2, 0x04, 0x00};
#elif _M_X64
    // 33 C0                xor         eax,eax
    // C3                   ret
    constexpr unsigned char szExecute[] = {0x33, 0xC0, 0xC3};
#else
#error "The following code only works for x86 and x64!"
#endif

    SIZE_T bytesWritten = 0;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
    return bRet;
}

void xrDebug::_initialize()
{
    std::atexit([] { R_ASSERT(ExitFromWinMain, "Unexpected application exit!"); });

    std::set_terminate(_terminate);

    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    signal(SIGABRT, abort_handler);
    signal(SIGABRT_COMPAT, abort_handler);
    signal(SIGFPE, floating_point_handler);
    signal(SIGILL, illegal_instruction_handler);
    // signal(SIGSEGV, segment_violation);
    signal(SIGINT, 0);
    signal(SIGTERM, termination_handler);

    _set_invalid_parameter_handler(&invalid_parameter_handler);

    _set_new_mode(1);
    _set_new_handler(&out_of_memory_handler);
    std::set_new_handler(&std_out_of_memory_handler);

    _set_purecall_handler(&pure_call_handler);

    ::SetUnhandledExceptionFilter(UnhandledFilter);

    PreventSetUnhandledExceptionFilter();

    // Выключаем окно "Прекращена работа программы...". У нас своё окно для сообщений об ошибках есть.
    // SetErrorMode(GetErrorMode() | SEM_NOGPFAULTERRORBOX);
}
