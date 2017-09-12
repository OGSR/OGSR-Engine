// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#if defined(_WIN32)
# ifdef __MWERKS__
#  pragma ANSI_strict off
# endif
# include <windows.h>
# ifdef __MWERKS__
#  pragma ANSI_strict reset
# endif

extern "C" BOOL WINAPI DllMain ( HINSTANCE hInst, DWORD wDataSeg, LPVOID lpvReserved );

# ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable:4297)
extern "C" void structured_exception_translator(unsigned int, EXCEPTION_POINTERS*)
{
    throw;
}
#  pragma warning(pop)
# endif

BOOL WINAPI DllMain(
    HINSTANCE,  //hDllInst
    DWORD fdwReason,
    LPVOID  // lpvReserved
    )
{
# ifdef BOOST_MSVC
    _set_se_translator(structured_exception_translator);
# endif
    (void)fdwReason; // warning suppression.

    return 1;
}
#endif // _WIN32

