#pragma once

#define DEBUG_INVOKE __debugbreak()

typedef	void		crashhandler		(void);
typedef	void		on_dialog			(bool before);

class XRCORE_API	xrDebug
{
private:
	crashhandler*	handler = nullptr;
	on_dialog*		m_on_dialog = nullptr;

public:
	void			_initialize			(const bool &dedicated);
	
	crashhandler*	get_crashhandler() const { return handler; };
	void			set_crashhandler	(crashhandler* handler)	{ this->handler = handler; };

	on_dialog*		get_on_dialog() const { return m_on_dialog;	}
	void			set_on_dialog		(on_dialog* on_dialog)		{ m_on_dialog = on_dialog;	}

	const char* error2string(const DWORD code) const;

	void			fail				(const char *e1, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const std::string &e2, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always);
	void			error				(const DWORD code, const char* e1, const char *file, int line, const char *function, bool &ignore_always);
	void			error				(const DWORD code, const char* e1, const char* e2, const char *file, int line, const char *function, bool &ignore_always);
	void _cdecl		fatal				(const char *file, int line, const char *function, const char* F,...);
	void			backend				(const char* reason, const char* expression, const char *argument0, const char *argument1, const char* file, int line, const char *function, bool &ignore_always);
	__declspec(noreturn) static void	do_exit(const std::string &message);
};

// warning
// this function can be used for debug purposes only
IC	std::string __cdecl	make_string		(const char* format,...)
{
	va_list		args;
	va_start	(args,format);

	char		temp[4096];
	vsprintf	(temp,format,args);

	return		(temp);
}

extern XRCORE_API xrDebug Debug;

XRCORE_API void LogStackTrace(const char* header);
XRCORE_API void LogStackTrace(const char* header, _EXCEPTION_POINTERS *pExceptionInfo);

// KRodin: отладочный хак для получения стека вызовов, при вызове проблемного кода внутри __try {...}
// Использовать примерно так:
// __except(ExceptStackTrace("stack trace:\n")) {...}
XRCORE_API LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS *pExceptionInfo);
#define ExceptStackTrace(str) DbgLogExceptionFilter(str, GetExceptionInformation())

#include "xrDebug_macros.h"
