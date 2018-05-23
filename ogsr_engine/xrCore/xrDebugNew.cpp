#include "stdafx.h"

#include "xrdebug.h"

#include <new.h> // for _set_new_mode
#include <signal.h> // for signals
#include <mutex>

XRCORE_API xrDebug Debug;

static bool error_after_dialog = false;

#ifndef XR_USE_BLACKBOX
#	include "stacktrace_collector.h"
#else
#	include "blackbox\build_stacktrace.h"
#endif
static thread_local StackTraceInfo stackTrace;

void LogStackTrace(const char* header)
{
	//if (IsDebuggerPresent())
	//	return;

	__try
	{
		Log(header);
		Log("*********************************************************************************");
		BuildStackTrace(stackTrace);
		for (size_t i = 0; i < stackTrace.count; ++i)
			Log(stackTrace[i]);
		Log("*********************************************************************************");
	}
	__finally{}
}

void LogStackTrace(const char* header, _EXCEPTION_POINTERS *pExceptionInfo)
{
	//if (IsDebuggerPresent())
	//	return;

	Msg("!![LogStackTrace] ExceptionCode is [%x]", pExceptionInfo->ExceptionRecord->ExceptionCode);
	Log(header);
	__try
	{
		Log("*********************************************************************************");
		auto save = *pExceptionInfo->ContextRecord;
		BuildStackTrace(pExceptionInfo, stackTrace);
		*pExceptionInfo->ContextRecord = save;

		for (size_t i = 0; i < stackTrace.count; ++i)
			Log(stackTrace[i]);
		Log("*********************************************************************************");
	}
	__finally {}
}

LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS *pExceptionInfo)
{
	LogStackTrace(header, pExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
}

void gather_info(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, char* assertion_info)
{
	auto buffer = assertion_info;
	auto endline = "\n";
	auto prefix = "[error]";
	bool extended_description = (description && !argument0 && strchr(description, '\n'));
	for (int i = 0; i < 2; ++i) {
		if (!i)
			buffer += sprintf(buffer, "%sFATAL ERROR%s%s", endline, endline, endline);
		buffer += sprintf(buffer, "%sExpression    : %s%s", prefix, expression, endline);
		buffer += sprintf(buffer, "%sFunction      : %s%s", prefix, function, endline);
		buffer += sprintf(buffer, "%sFile          : %s%s", prefix, file, endline);
		buffer += sprintf(buffer, "%sLine          : %d%s", prefix, line, endline);

		if (extended_description) {
			buffer += sprintf(buffer, "%s%s%s", endline, description, endline);
			if (argument0) {
				if (argument1) {
					buffer += sprintf(buffer, "%s%s", argument0, endline);
					buffer += sprintf(buffer, "%s%s", argument1, endline);
				}
				else
					buffer += sprintf(buffer, "%s%s", argument0, endline);
			}
		}
		else {
			buffer += sprintf(buffer, "%sDescription   : %s%s", prefix, description, endline);
			if (argument0) {
				if (argument1) {
					buffer += sprintf(buffer, "%sArgument 0    : %s%s", prefix, argument0, endline);
					buffer += sprintf(buffer, "%sArgument 1    : %s%s", prefix, argument1, endline);
				}
				else
					buffer += sprintf(buffer, "%sArguments     : %s%s", prefix, argument0, endline);
			}
		}

		buffer += sprintf(buffer, "%s", endline);
		if (!i) {
			Log(assertion_info);
			buffer = assertion_info;
			endline = "\r\n";
			prefix = "";
		}
	}

#ifdef USE_MEMORY_MONITOR
	memory_monitor::flush_each_time(true);
	memory_monitor::flush_each_time(false);
#endif // USE_MEMORY_MONITOR

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
#ifdef USE_OWN_MINI_DUMP
	buffer += sprintf(buffer, "See log file and minidump for detailed information\r\n");
#else
	buffer += sprintf(buffer, "See log file for detailed information\r\n");
#endif
#endif
	LogStackTrace("stack trace:\n");
}

__declspec(noreturn) void xrDebug::do_exit(const std::string &message)
{
	auto wnd = GetActiveWindow();
	if (!wnd)
		wnd = GetForegroundWindow();
	ShowWindow(wnd, SW_FORCEMINIMIZE);

	while (ShowCursor(TRUE) < 0);

	MessageBox(wnd, message.c_str(), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

	TerminateProcess(GetCurrentProcess(), 1);
}

void xrDebug::backend(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, bool &ignore_always)
{
	static std::recursive_mutex CS;
	std::lock_guard<decltype(CS)> lock(CS);

	string4096 assertion_info;
	gather_info(expression, description, argument0, argument1, file, line, function, assertion_info);

	if (handler)
		handler();

	auto game_hwnd = GetActiveWindow();
	if (!game_hwnd)
		game_hwnd = GetForegroundWindow();
	ShowWindow(game_hwnd, SW_FORCEMINIMIZE);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
	if (get_on_dialog())
		get_on_dialog()	(true);

	auto endline = "\r\n";
	auto buffer = assertion_info + xr_strlen(assertion_info);
	buffer += sprintf(buffer, "%sPress OK to abort execution%s", endline, endline);

	while (ShowCursor(TRUE) < 0);

	error_after_dialog = true;

	MessageBox(
		game_hwnd,
		assertion_info,
		"FATAL ERROR",
		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
	);

	DEBUG_INVOKE;

	if (get_on_dialog())
		get_on_dialog()(false);
#else
	DEBUG_INVOKE;
#endif
}

const char* xrDebug::error2string(const DWORD code) const {
	const char* result = nullptr;
	static string1024 desc_storage;

	if (nullptr == result) {
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, code, 0, desc_storage,
			sizeof(desc_storage) - 1, nullptr);
		result = desc_storage;
	}
	return result;
}

void xrDebug::error(const DWORD hr, const char* expr, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, 0, 0, file, line, function, ignore_always);
}

void xrDebug::error(const DWORD hr, const char* expr, const char* e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, e2, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *file, int line, const char *function, bool &ignore_always)
{
	backend("assertion failed", e1, 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const std::string &e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2.c_str(), 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, e3, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, e3, e4, file, line, function, ignore_always);
}

void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F, ...)
{
	va_list args;
	va_start(args, F);
	int buf_len = std::vsnprintf(nullptr, 0, F, args);
	auto strBuf = std::make_unique<char[]>(buf_len + 1);
	std::vsnprintf(strBuf.get(), buf_len + 1, F, args);

	bool ignore_always = true;

	backend("FATAL ERROR", strBuf.get(), nullptr, nullptr, file, line, function, ignore_always);
}

int out_of_memory_handler	(size_t size)
{
	Memory.mem_compact		();
	u32						process_heap	= mem_usage_impl(nullptr, nullptr);
	u32						eco_strings		= g_pStringContainer->stat_economy			();
	u32						eco_smem		= g_pSharedMemoryContainer->stat_economy	();
	Msg						("* [x-ray]: process heap[%d K]", process_heap/1024);
	Msg						("* [x-ray]: economy: strings[%d K], smem[%d K]",eco_strings/1024,eco_smem);
	Debug.fatal				(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
	return					1;
}

#ifdef USE_OWN_MINI_DUMP
#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "Version.lib")

typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

HMODULE LoadDebugHlp()
{
	auto hDll = GetModuleHandle("dbghelp.dll");
	if (hDll)
		return hDll;

	string_path szDbgHelpPath;

	if (GetModuleFileName(nullptr, szDbgHelpPath, _MAX_PATH))
	{
		char *pSlash = strchr(szDbgHelpPath, '\\');
		if (pSlash)
		{
			strcpy(pSlash + 1, "DBGHELP.DLL");
			// alpet: проверка версии необходима, т.к. старый файл очень часто лажает с выводом стека вызовов
			DWORD nope;
			DWORD size = GetFileVersionInfoSize(szDbgHelpPath, &nope);
			if (size > 0)
			{
				LPVOID ver_data = xr_malloc(size);
				if (GetFileVersionInfo(szDbgHelpPath, NULL, size, ver_data))
				{
					VS_FIXEDFILEINFO *info = nullptr;
					UINT len;
					VerQueryValue(ver_data, "\\", (LPVOID*)&info, &len);
					if (info && info->dwFileVersionMS >= 6)
						hDll = ::LoadLibrary(szDbgHelpPath);
					else
						Log("!#ERROR: dbghelp.dll version is old for this build.");
				}

				xr_free(ver_data);
			}
		}
	}

	if (!hDll)
	{
		// load any version we can
		hDll = ::LoadLibrary("DBGHELP.DLL");
	}

	return hDll;
}

void save_mini_dump(_EXCEPTION_POINTERS *pExceptionInfo)
{
	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	LPCTSTR szResult = nullptr;
	auto hDll = LoadDebugHlp();

	if (hDll)
	{
		auto pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
		if (pDump)
		{
			string_path	szDumpPath;
			string_path	szScratch;
			string64	t_stemp;

			timestamp(t_stemp);
			strcpy(szDumpPath, Core.ApplicationName);
			strcat(szDumpPath, "_");
			strcat(szDumpPath, Core.UserName);
			strcat(szDumpPath, "_");
			strcat(szDumpPath, t_stemp);
			strcat(szDumpPath, ".mdmp");

			__try {
				if (FS.path_exist("$logs$"))
					FS.update_path(szDumpPath, "$logs$", szDumpPath);
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				string_path	temp;
				strcpy(temp, szDumpPath);
				strcpy(szDumpPath, "logs/");
				strcat(szDumpPath, temp);
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

				BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, nullptr, nullptr);
				if (bOK)
				{
					sprintf(szScratch, "Saved dump file to '%s'", szDumpPath);
					szResult = szScratch;
					//					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					sprintf(szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError());
					szResult = szScratch;
				}
				::CloseHandle(hFile);
			}
			else
			{
				sprintf(szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError());
				szResult = szScratch;
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}
}
#endif

void format_message	(char* buffer, const u32 &buffer_size)
{
    LPVOID message;
    DWORD error_code = GetLastError(); 

	if (!error_code) {
		*buffer	= 0;
		return;
	}

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&message,
        0,
		nullptr
	);

	sprintf		(buffer,"[error][%8d]    : %s",error_code, (char*)&message);
    LocalFree	(message);
}


LONG WINAPI UnhandledFilter(_EXCEPTION_POINTERS *pExceptionInfo)
{
	if (!error_after_dialog /*&& !IsDebuggerPresent()*/)
	{
		auto pCrashHandler = Debug.get_crashhandler();
		if (pCrashHandler != nullptr)
		{
			pCrashHandler();
		}

		string1024 error_message;
		format_message(error_message, sizeof(error_message));
		if (*error_message)
			Msg("\n%s", error_message);

		LogStackTrace("Unhandled exception stack trace:\n", pExceptionInfo);

		auto wnd = GetActiveWindow();
		if (!wnd)
			wnd = GetForegroundWindow();
		ShowWindow(wnd, SW_FORCEMINIMIZE);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
		while (ShowCursor(TRUE) < 0);

		if (Debug.get_on_dialog())
			Debug.get_on_dialog()(true);

		MessageBox(wnd, "Fatal error occured\n\nPress OK to abort program execution", "FATAL ERROR", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

		if (Debug.get_on_dialog())
			Debug.get_on_dialog()(false);
#endif
	}

#ifdef USE_OWN_MINI_DUMP
	save_mini_dump(pExceptionInfo);
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}


void _terminate()
{
	string4096 assertion_info;

	gather_info(
		"<no expression>",
		"Unexpected application termination",
		nullptr,
		nullptr,
		__FILE__,
		__LINE__,
		__FUNCTION__,
		assertion_info
	);

	auto endline = "\r\n";
	auto buffer = assertion_info + xr_strlen(assertion_info);
	buffer += sprintf(buffer, "Press OK to abort execution%s", endline);

	auto wnd = GetActiveWindow();
	if (!wnd)
		wnd = GetForegroundWindow();
	ShowWindow(wnd, SW_FORCEMINIMIZE);

	while (ShowCursor(TRUE) < 0);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
	MessageBox(
		wnd,
		assertion_info,
		"FATAL ERROR",
		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
	);
#endif

	exit(-1);
}

void debug_on_thread_spawn()
{
	std::set_terminate(_terminate);
}

static void handler_base(const char* reason_string)
{
	bool ignore_always = false;
	Debug.backend(
		"error handler is invoked!",
		reason_string,
		nullptr,
		nullptr,
		DEBUG_INFO,
		ignore_always
	);
}

static void invalid_parameter_handler(
	const wchar_t *expression,
	const wchar_t *function,
	const wchar_t *file,
	unsigned int line,
	uintptr_t reserved
)
{
	bool ignore_always = false;

	string4096 expression_;
	string4096 function_;
	string4096 file_;
	size_t converted_chars = 0;

	if (expression)
		wcstombs_s(
			&converted_chars,
			expression_,
			sizeof(expression_),
			expression,
			(wcslen(expression) + 1) * 2 * sizeof(char)
		);
	else
		strcpy_s(expression_, "");

	if (function)
		wcstombs_s(
			&converted_chars,
			function_,
			sizeof(function_),
			function,
			(wcslen(function) + 1) * 2 * sizeof(char)
		);
	else
		strcpy_s(function_, __FUNCTION__);

	if (file)
		wcstombs_s(
			&converted_chars,
			file_,
			sizeof(file_),
			file,
			(wcslen(file) + 1) * 2 * sizeof(char)
		);
	else {
		line = __LINE__;
		strcpy_s(file_, __FILE__);
	}

	Debug.backend(
		"error handler is invoked!",
		expression_,
		nullptr,
		nullptr,
		file_,
		line,
		function_,
		ignore_always
	);
}

static void std_out_of_memory_handler()
{
	handler_base("std: out of memory");
}

static void pure_call_handler()
{
	handler_base("pure virtual function call");
}

/* //Не используется
static void unexpected_handler()
{
	handler_base					("unexpected program termination");
}
*/

static void abort_handler(int signal)
{
	handler_base("application is aborting");
}

static void floating_point_handler(int signal)
{
	handler_base("floating point error");
}

static void illegal_instruction_handler(int signal)
{
	handler_base("illegal instruction");
}

//	static void storage_access_handler		(int signal)
//	{
//		handler_base					("illegal storage access");
//	}

static void termination_handler(int signal)
{
	handler_base("termination with exit code 3");
}

void xrDebug::_initialize(const bool &dedicated)
{
	debug_on_thread_spawn();

	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	signal(SIGABRT, abort_handler);
	signal(SIGABRT_COMPAT, abort_handler);
	signal(SIGFPE, floating_point_handler);
	signal(SIGILL, illegal_instruction_handler);
	signal(SIGINT, 0);
	//		signal							(SIGSEGV,		storage_access_handler);
	signal(SIGTERM, termination_handler);

	_set_invalid_parameter_handler(&invalid_parameter_handler);

	_set_new_mode(1);
	_set_new_handler(&out_of_memory_handler);
	std::set_new_handler(&std_out_of_memory_handler);

	_set_purecall_handler(&pure_call_handler);

/* // should be if we use exceptions
	std::set_unexpected(_terminate);
*/
	::SetUnhandledExceptionFilter(UnhandledFilter);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
	// Выключаем окно "Прекращена работа программы...". У нас своё окно для сообщений об ошибках есть.
	auto prevMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetErrorMode(prevMode | SEM_NOGPFAULTERRORBOX);
#endif
}
