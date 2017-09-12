@ECHO OFF
REM Copyrigt (C) 2002 Rene Rivera.
REM Permission to copy, use, modify, sell and distribute this software
REM is granted provided this copyright notice appears in all copies.
REM This software is provided "as is" without express or implied
REM warranty, and with no claim as to its suitability for any purpose.
setlocal
goto Start

REM NOTE: The "setlocal & endlocal" construct is used to reset the errorlevel to 0.
REM NOTE: The "set _error_=" construct is used to set the errorlevel to 1

:Error_Print
REM Output an error message and set the errorlevel to indicate failure.
setlocal
ECHO ###
ECHO ### %1
ECHO ###
ECHO ### You can specify the toolset as the argument, i.e.:
ECHO ###     .\build.bat msvc
ECHO ###
ECHO ### Toolsets supported by this script are: borland, como, gcc, gcc-nocygwin, intel-win32, metrowerks, mingw, msvc, vc7
ECHO ###
set _error_=
endlocal
goto :eof

:Test_Path
REM Tests for the given file(executable) presence in the directories in the PATH
REM environment variable. Additionaly sets FOUND_PATH to the path of the
REM found file.
setlocal & endlocal
setlocal
set test=%~$PATH:1
endlocal
if not errorlevel 1 set FOUND_PATH=%~dp$PATH:1
goto :eof

:Test_Option
REM Tests wether the given string is in the form of an option: "-*"
setlocal & endlocal
setlocal
set test=%1
if not "-" == "%test:~0,1%" set _error_=
endlocal
goto :eof

:Guess_Toolset
REM Try and guess the toolset to bootstrap the build with...
REM Sets BOOST_JAM_TOOLSET to the first found toolset.
REM May also set BOOST_JAM_TOOLSET_ROOT to the
REM location of the found toolset.
setlocal & endlocal
if NOT "_%CWFolder%_" == "__" (
    set BOOST_JAM_TOOLSET=metrowerks
    set BOOST_JAM_TOOLSET_ROOT=%CWFolder%\
    goto :eof )
setlocal & endlocal
call :Test_Path mwcc.exe
if not errorlevel 1 (
    set BOOST_JAM_TOOLSET=metrowerks
    set BOOST_JAM_TOOLSET_ROOT=%FOUND_PATH%..\..\
    goto :eof)
setlocal & endlocal
if NOT "_%MSVCDir%_" == "__" (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=%MSVCDir%\
    goto :eof)
setlocal & endlocal
if EXIST "%ProgramFiles%\Microsoft Visual Studio\VC98\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=%ProgramFiles%\Microsoft Visual Studio\VC98\
    goto :eof)
setlocal & endlocal
if EXIST "C:\Program Files\Microsoft Visual Studio\VC98\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=C:\Program Files\Microsoft Visual Studio\VC98\
    goto :eof)
setlocal & endlocal
if EXIST "%ProgramFiles%\Microsoft Visual C++\VC98\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=%ProgramFiles%\Microsoft Visual C++\VC98\
    goto :eof)
setlocal & endlocal
if EXIST "C:\Program Files\Microsoft Visual C++\VC98\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=C:\Program Files\Microsoft Visual C++\VC98\
    goto :eof)
setlocal & endlocal
if EXIST "%ProgramFiles%\Microsoft Visual Studio .NET\VC7\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=vc7
    set BOOST_JAM_TOOLSET_ROOT=%ProgramFiles%\Microsoft Visual Studio .NET\VC7\
    goto :eof)
setlocal & endlocal
if EXIST "C:\Program Files\Microsoft Visual Studio .NET\VC7\bin\VCVARS32.BAT" (
    set BOOST_JAM_TOOLSET=vc7
    set BOOST_JAM_TOOLSET_ROOT=C:\Program Files\Microsoft Visual Studio .NET\VC7\
    goto :eof)
setlocal & endlocal
call :Test_Path cl.exe
if not errorlevel 1 (
    set BOOST_JAM_TOOLSET=msvc
    set BOOST_JAM_TOOLSET_ROOT=%FOUND_PATH%..\
    goto :eof)
setlocal & endlocal
call :Test_Path vcvars32.bat
if not errorlevel 1 (
    set BOOST_JAM_TOOLSET=msvc
    call "%FOUND_PATH%VCVARS32.BAT"
    set BOOST_JAM_TOOLSET_ROOT=%MSVCDir%\
    goto :eof)
setlocal & endlocal
if EXIST "C:\Borland\BCC55\Bin\bcc32.exe" (
    set BOOST_JAM_TOOLSET=borland
    set BOOST_JAM_TOOLSET_ROOT=C:\Borland\BCC55\
    goto :eof)
setlocal & endlocal
call :Test_Path bcc32.exe
if not errorlevel 1 (
    set BOOST_JAM_TOOLSET=borland
    set BOOST_JAM_TOOLSET_ROOT=%FOUND_PATH%..\
    goto :eof)
setlocal & endlocal
call :Test_Path icl.exe
if not errorlevel 1 (
    set BOOST_JAM_TOOLSET=intel-win32
    set BOOST_JAM_TOOLSET_ROOT=%FOUND_PATH%..\
    goto :eof)
setlocal & endlocal
call :Error_Print "Could not find a suitable toolset."
goto :eof

:Start
set BOOST_JAM_TOOLSET=

REM If no arguments guess the toolset;
REM or if first argument is an option guess the toolser;
REM otherwise the argument is the toolset to use.
if "_%1_" == "__" (
    call :Guess_Toolset
) else (
    call :Test_Option %1
    if not errorlevel 1 (
        call :Guess_Toolset
    ) else (
        setlocal & endlocal
        set BOOST_JAM_TOOLSET=%1
        shift
    )
)
if errorlevel 1 goto Finish

REM Setup the toolset command and options. This bit of code
REM needs to be flexible enough to handle both when
REM the toolset was guessed at and found, or when the toolset
REM was indicated in the command arguments.
REM NOTE: The strange multiple "if ?? == _toolset_" tests are that way
REM because in BAT variables are subsituted only once during a single
REM command. A complete "if ... ( commands ) else ( commands )"
REM is a single command, even though it's in multiple lines here.
if "_%BOOST_JAM_TOOLSET%_" == "_metrowerks_" (
    if NOT "_%CWFolder%_" == "__" (
        set BOOST_JAM_TOOLSET_ROOT=%CWFolder%\) )
if "_%BOOST_JAM_TOOLSET%_" == "_metrowerks_" (
    if not "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
        set PATH=%PATH%;%BOOST_JAM_TOOLSET_ROOT%Other Metrowerks Tools\Command Line Tools)
    set BOOST_JAM_CC=mwcc -runtime staticsingle -DNT
    set BOOST_JAM_OPT_JAM=-o bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe
    set BOOST_JAM_OPT_MKJAMBASE=-o bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe
    set BOOST_JAM_OPT_YYACC=-o bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_msvc_" (
    if NOT "_%MSVCDir%_" == "__" (
        set BOOST_JAM_TOOLSET_ROOT=%MSVCDir%\) )
if "_%BOOST_JAM_TOOLSET%_" == "_msvc_" (
    if EXIST "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT" (
        call "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT" ) )
if "_%BOOST_JAM_TOOLSET%_" == "_msvc_" (
    if not "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
        set PATH=%PATH%;%BOOST_JAM_TOOLSET_ROOT%bin)
    set BOOST_JAM_CC="%BOOST_JAM_TOOLSET_ROOT%bin\cl.exe" /nologo /GZ /Zi /ML -DNT -DYYDEBUG /LIBPATH:"%LIB%" kernel32.lib
    set BOOST_JAM_OPT_JAM=/Febootstrap.%BOOST_JAM_TOOLSET%\jam0
    set BOOST_JAM_OPT_MKJAMBASE=/Febootstrap.%BOOST_JAM_TOOLSET%\mkjambase0
    set BOOST_JAM_OPT_YYACC=/Febootstrap.%BOOST_JAM_TOOLSET%\yyacc0
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_vc7_" (
    if NOT "_%MSVCDir%_" == "__" (
        set BOOST_JAM_TOOLSET_ROOT=%MSVCDir%\) )
if "_%BOOST_JAM_TOOLSET%_" == "_vc7_" (
    if EXIST "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT" (
        call "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT" ) )
if "_%BOOST_JAM_TOOLSET%_" == "_vc7_" (
    if not "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
        set PATH=%PATH%;%BOOST_JAM_TOOLSET_ROOT%bin)
    set BOOST_JAM_CC="%BOOST_JAM_TOOLSET_ROOT%bin\cl.exe" /nologo /GZ /Zi /ML -DNT -DYYDEBUG /LIBPATH:"%LIB%" kernel32.lib
    set BOOST_JAM_OPT_JAM=/Febootstrap.%BOOST_JAM_TOOLSET%\jam0
    set BOOST_JAM_OPT_MKJAMBASE=/Febootstrap.%BOOST_JAM_TOOLSET%\mkjambase0
    set BOOST_JAM_OPT_YYACC=/Febootstrap.%BOOST_JAM_TOOLSET%\yyacc0
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_borland_" (
    if "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
        call :Test_Path bcc32.exe ) )
if "_%BOOST_JAM_TOOLSET%_" == "_borland_" (
    if "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
        if not errorlevel 1 (
            set BOOST_JAM_TOOLSET_ROOT=%FOUND_PATH%..\) ) )
if "_%BOOST_JAM_TOOLSET%_" == "_borland_" (
    if not "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (set PATH=%PATH%;%BOOST_JAM_TOOLSET_ROOT%Bin)
    set BOOST_JAM_CC=bcc32 -WC -w- -q "-I%BOOST_JAM_TOOLSET_ROOT%Include" "-L%BOOST_JAM_TOOLSET_ROOT%Lib" /DNT -nbootstrap.%BOOST_JAM_TOOLSET%
    set BOOST_JAM_OPT_JAM=-ejam0
    set BOOST_JAM_OPT_MKJAMBASE=-emkjambasejam0
    set BOOST_JAM_OPT_YYACC=-eyyacc0
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_como_" (
    set BOOST_JAM_CC=como
    set BOOST_JAM_OPT_JAM=-o bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe
    set BOOST_JAM_OPT_MKJAMBASE=-o bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe
    set BOOST_JAM_OPT_YYACC=-o bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_gcc_" (
    set BOOST_JAM_CC=gcc
    set BOOST_JAM_OPT_JAM=-o bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe
    set BOOST_JAM_OPT_MKJAMBASE=-o bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe
    set BOOST_JAM_OPT_YYACC=-o bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_gcc-nocygwin_" (
    set BOOST_JAM_CC=gcc -mno-cygwin
    set BOOST_JAM_OPT_JAM=-o bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe
    set BOOST_JAM_OPT_MKJAMBASE=-o bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe
    set BOOST_JAM_OPT_YYACC=-o bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_intel-win32_" (
    set BOOST_JAM_CC=icl /nologo
    set BOOST_JAM_OPT_JAM=/Febootstrap.%BOOST_JAM_TOOLSET%\jam0
    set BOOST_JAM_OPT_MKJAMBASE=/Febootstrap.%BOOST_JAM_TOOLSET%\mkjambase0
    set BOOST_JAM_OPT_YYACC=/Febootstrap.%BOOST_JAM_TOOLSET%\yyacc0
    set _known_=1
)
if "_%BOOST_JAM_TOOLSET%_" == "_mingw_" (
    set BOOST_JAM_CC=gcc -DMINGW
    set BOOST_JAM_OPT_JAM=-o bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe
    set BOOST_JAM_OPT_MKJAMBASE=-o bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe
    set BOOST_JAM_OPT_YYACC=-o bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe
    set _known_=1
)
if "_%_known_%_" == "__" (
    call :Error_Print "Unknown toolset: %BOOST_JAM_TOOLSET%"
)
if errorlevel 1 goto Finish

echo ###
echo ### Using '%BOOST_JAM_TOOLSET%' toolset.
echo ###

set YYACC_SOURCES=yyacc.c
set MKJAMBASE_SOURCES=mkjambase.c
set BJAM_SOURCES=
set BJAM_SOURCES=%BJAM_SOURCES% command.c compile.c execnt.c execunix.c execvms.c expand.c
set BJAM_SOURCES=%BJAM_SOURCES% filent.c fileos2.c fileunix.c filevms.c glob.c hash.c
set BJAM_SOURCES=%BJAM_SOURCES% hdrmacro.c headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c
set BJAM_SOURCES=%BJAM_SOURCES% newstr.c option.c parse.c pathunix.c pathvms.c regexp.c
set BJAM_SOURCES=%BJAM_SOURCES% rules.c scan.c search.c subst.c timestamp.c variable.c modules.c
set BJAM_SOURCES=%BJAM_SOURCES% strings.c filesys.c builtins.c pwd.c

@echo ON
rd /S /Q bootstrap.%BOOST_JAM_TOOLSET%
md bootstrap.%BOOST_JAM_TOOLSET%
@if not exist jamgram.y goto Bootstrap_GrammarPrep
@if not exist jamgramtab.h goto Bootstrap_GrammarPrep
@goto Skip_GrammarPrep
:Bootstrap_GrammarPrep
%BOOST_JAM_CC% %BOOST_JAM_OPT_YYACC% %YYACC_SOURCES%
@if not exist ".\bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe" goto Skip_GrammarPrep
".\bootstrap.%BOOST_JAM_TOOLSET%\yyacc0.exe" jamgram.y jamgramtab.h jamgram.yy
:Skip_GrammarPrep
@if not exist jamgram.c goto Bootstrap_GrammarBuild
@if not exist jamgram.h goto Bootstrap_GrammarBuild
@goto Skip_GrammarBuild
:Bootstrap_GrammarBuild
@ECHO OFF
@setlocal & endlocal
@call :Test_Path yacc.exe
@if not errorlevel 1 (set YACC=yacc -d)
@setlocal & endlocal
@call :Test_Path bison.exe
@if not errorlevel 1 ( if "_%YACC%_" == "__" (set YACC=bison -y -d --yacc) )
@if "_%YACC%_" == "__" goto Skip_GrammarBuild
@echo ON
%YACC% jamgram.y
rename y.tab.c jamgram.c
rename y.tab.h jamgram.h
:Skip_GrammarBuild
@echo ON
@if exist jambase.c goto Skip_Jambase
%BOOST_JAM_CC% %BOOST_JAM_OPT_MKJAMBASE% %MKJAMBASE_SOURCES%
@if not exist ".\bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe" goto Skip_Jambase
".\bootstrap.%BOOST_JAM_TOOLSET%\mkjambase0.exe" jambase.c Jambase
:Skip_Jambase
%BOOST_JAM_CC% %BOOST_JAM_OPT_JAM% %BJAM_SOURCES%
@if not exist ".\bootstrap.%BOOST_JAM_TOOLSET%\jam0.exe" goto Skip_Jam
.\bootstrap.%BOOST_JAM_TOOLSET%\jam0 -f build.jam --toolset=%BOOST_JAM_TOOLSET% "--toolset-root=%BOOST_JAM_TOOLSET_ROOT% " %1 %2 %3 %4 %5 %6 %7 %8 %9
:Skip_Jam

:Finish
