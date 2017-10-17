#pragma once

#define DEBUG_INFO __FILE__,__LINE__,__FUNCTION__

//KRodin: Добавил ASSERT как в скриптах, с поддержкой форматирования строки и неограниченным кол-вом аргументов.
#define FATAL(...) Debug.fatal(DEBUG_INFO, __VA_ARGS__)
#define ASSERT_FMT(expr, ...) do { if (!(expr)) FATAL(__VA_ARGS__);} while(0)

#define CHECK_OR_EXIT(expr,message)	do {if (!(expr)) ::Debug.do_exit(message);} while (0)

#define R_ASSERT(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,DEBUG_INFO,ignore_always);} while(0)
#define R_ASSERT2(expr,e2)			do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#define R_ASSERT3(expr,e2,e3)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO,ignore_always);} while(0)
#define R_ASSERT4(expr,e2,e3)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,e4,DEBUG_INFO,ignore_always);} while(0)
#define R_CHK(expr)					do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO,ignore_always);} while(0)
#define R_CHK2(expr,e2)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,e2,DEBUG_INFO,ignore_always);} while(0)

#ifdef VERIFY
#	undef VERIFY
#endif

#ifdef DEBUG
#	define NODEFAULT				FATAL("nodefault reached")
#	define VERIFY(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,DEBUG_INFO,ignore_always);} while(0)
#	define VERIFY2(expr, e2)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#	define VERIFY3(expr, e2, e3)	do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO,ignore_always);} while(0)
#	define VERIFY4(expr, e2, e3, e4)do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,e4,DEBUG_INFO,ignore_always);} while(0)
#	define CHK_DX(expr)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO,ignore_always);} while(0)
#else
#	define NODEFAULT __assume(0)
#	define VERIFY(expr)				do {} while (0)
#	define VERIFY2(expr, e2)		do {} while (0)
#	define VERIFY3(expr, e2, e3)	do {} while (0)
#	define VERIFY4(expr, e2, e3, e4)do {} while (0)
#	define CHK_DX(a) a
#endif

//pragma todo:
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
//
#define STATIC_CHECK(expr, msg) static_assert(expr, #msg)
