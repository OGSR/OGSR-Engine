# Microsoft Developer Studio Project File - Name="unit_test_framework" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=unit_test_framework - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "unit_test_framework.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "unit_test_framework.mak" CFG="unit_test_framework - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "unit_test_framework - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unit_test_framework - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "unit_test_framework - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "unit_test_framework - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "c:\temp\boost_test_library\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "unit_test_framework - Win32 Release"
# Name "unit_test_framework - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\execution_monitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\test_tools.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_log.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_monitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_parameters.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_result.cpp
# End Source File
# Begin Source File

SOURCE=..\src\unit_test_suite.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "detail"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\boost\test\detail\class_properties.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\nullstream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\unit_test_config.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\unit_test_monitor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\unit_test_parameters.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\wrap_stringstream.hpp
# End Source File
# End Group
# Begin Group "included"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\boost\test\included\unit_test_framework.hpp
# End Source File
# End Group
# Begin Group "extentions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\boost\test\auto_unit_test.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\unit_test_suite_ex.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\boost\test\execution_monitor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\detail\floating_point_comparison.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\test_tools.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\unit_test.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\unit_test_log.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\unit_test_result.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\boost\test\unit_test_suite.hpp
# End Source File
# End Group
# End Target
# End Project
