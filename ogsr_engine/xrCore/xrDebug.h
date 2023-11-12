#pragma once

#define DEBUG_INVOKE __debugbreak()

class XRCORE_API xrDebug
{
private:
    using crashhandler = void(bool);
    crashhandler* handler = nullptr;

public:
    void _initialize();

    crashhandler* get_crashhandler() const { return handler; };
    void set_crashhandler(crashhandler* handler) { this->handler = handler; };

    const char* DXerror2string(const HRESULT code) const;
    const char* error2string(const DWORD code) const;

    void fail(const char* e1, const char* file, int line, const char* function);
    void fail(const char* e1, const std::string& e2, const char* file, int line, const char* function);
    void fail(const char* e1, const char* e2, const char* file, int line, const char* function);
    void fail(const char* e1, const char* e2, const char* e3, const char* file, int line, const char* function);
    void fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, int line, const char* function);
    void error(const HRESULT code, const char* e1, const char* file, int line, const char* function);
    void error(const HRESULT code, const char* e1, const char* e2, const char* file, int line, const char* function);
    void _cdecl fatal(const char* file, int line, const char* function, const char* F, ...);
    void backend(const char* reason, const char* expression, const char* argument0, const char* argument1, const char* file, int line, const char* function);
    static void do_exit(const std::string& message);

    void on_exception_in_thread();
};

// warning
// this function can be used for debug purposes only
IC std::string __cdecl make_string(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp[4096];
    vsprintf(temp, format, args);

    return (temp);
}

extern XRCORE_API xrDebug Debug;
extern XRCORE_API HWND gGameWindow;
extern XRCORE_API bool ExitFromWinMain;

XRCORE_API void LogStackTrace(const char* header, const bool dump_lua_locals = true);
XRCORE_API void LogStackTrace(const char* header, _EXCEPTION_POINTERS* pExceptionInfo, bool dump_lua_locals = false);

// KRodin: отладочный хак для получения стека вызовов, при вызове проблемного кода внутри __try {...}
// Использовать примерно так:
// __except(ExceptStackTrace("stack trace:\n")) {...}
XRCORE_API LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS* pExceptionInfo);
#define ExceptStackTrace(str) DbgLogExceptionFilter(str, GetExceptionInformation())

#include "xrDebug_macros.h"
