# Microsoft Developer Studio Project File - Name="test6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test6.mak" CFG="test6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test6 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test6 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/boost/libs/numeric/ublas/test6", ZDCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test6 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
dCPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\.." /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /Zm1000 /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "test6 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
dCPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\.." /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /Zm1000 /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "test6 - Win32 Release"
# Name "test6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\test6.cpp
# End Source File
# Begin Source File

SOURCE=.\test61.cpp
# End Source File
# Begin Source File

SOURCE=.\test62.cpp
# End Source File
# Begin Source File

SOURCE=.\test63.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\banded.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\blas.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\concepts.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\config.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\duff.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\exception.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\functional.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\hermitian.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\io.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\iterator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\matrix.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\matrix_assign.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\matrix_expression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\matrix_proxy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\matrix_sparse.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\storage.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\storage_sparse.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\symmetric.hpp
# End Source File
# Begin Source File

SOURCE=.\test6.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\traits.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\triangular.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\vector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\vector_assign.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\vector_expression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\vector_proxy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\boost\numeric\ublas\vector_sparse.hpp
# End Source File
# End Group
# End Target
# End Project
