// Copyright (C) 2001
// Mac Murrett
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Mac Murrett makes no representations
// about the suitability of this software for any purpose.  It is
// provided "as is" without express or implied warranty.
//
// See http://www.boost.org for most recent version including documentation.

//
// includes
//

#include <abort_exit.h>
#include <console.h>
#include <console_io.h>
#include <misc_io.h>
#include <SIOUX.h>

#include "remote_calls.hpp"


//
// using declarations
//

using std::__file_handle;
using std::__idle_proc;
using std::__io_error;
using std::__no_io_error;
using std::size_t;

using boost::threads::mac::st_remote_call;


//
// prototypes
//

static bool check_console();
static int do_read_console(__file_handle ulHandle, unsigned char *pBuffer, size_t *pCount, __idle_proc pfnIdleProc);
static int do_write_console(__file_handle ulHandle, unsigned char *pBuffer, size_t *pCount, __idle_proc pfnIdleProc);


//
// MSL function replacements
//

// these two functions are called by cin and cout, respectively, as well as by (all?)
//    other functions in MSL that do console I/O.  All that they do is as the remote
//    call manager to ensure that their guts are called at system task time.
int __read_console(__file_handle handle, unsigned char * buffer, size_t * count, __idle_proc idle_proc)
{
    return(st_remote_call(do_read_console, handle, buffer, count, idle_proc));
}

int __write_console(__file_handle handle, unsigned char * buffer, size_t * count, __idle_proc idle_proc)
{
    return(st_remote_call(do_write_console, handle, buffer, count, idle_proc));
}


//
// implementations
//

static bool check_console()
{
    static bool s_bHaveConsole(false);
    static bool s_bWontHaveConsole(false);

    if(s_bHaveConsole)
    {
        return(true);
    }

    if(s_bWontHaveConsole == false)
    {
        __stdio_atexit();

        if(InstallConsole(0) != 0)
        {
            s_bWontHaveConsole = true;
            return(false);
        }
        __console_exit = RemoveConsole;
        s_bHaveConsole = true;
        return(true);
    }

    return(false);
}


int do_read_console(__file_handle /*ulHandle*/, unsigned char *pBuffer, size_t *pCount, __idle_proc /*pfnIdleProc*/)
{
    assert(pCount != NULL);
    assert(pBuffer != NULL || *pCount == 0UL);

    if(check_console() == false)
    {
        return(__io_error);
    }
    std::fflush(stdout);
    long lCount = ReadCharsFromConsole(reinterpret_cast<char *>(pBuffer), static_cast<long>(*pCount));
    *pCount = static_cast<size_t>(lCount);
    if(lCount == -1L)
    {
        return(__io_error);
    }

    return(__no_io_error);
}

int do_write_console(__file_handle /*ulHandle*/, unsigned char *pBuffer, size_t *pCount, __idle_proc /*pfnIdleProc*/)
{
    if(check_console() == false)
    {
        return(__io_error);
    }

    long lCount = WriteCharsToConsole(reinterpret_cast<char *>(pBuffer), static_cast<long>(*pCount));
    *pCount = static_cast<size_t>(lCount);
    if(lCount == -1L)
    {
        return(__io_error);
    }

    return(__no_io_error);
}
