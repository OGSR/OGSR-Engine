#
# auto generated makefile for VC6 compiler
#
# usage:
# make
#   brings libraries up to date
# make install
#   brings libraries up to date and copies binaries to your VC6 /lib and /bin directories (recomended)
#

#
# Add additional compiler options here:
#
CXXFLAGS=
#
# Add additional include directories here:
#
INCLUDES=
#
# add additional linker flags here:
#
XLFLAGS=
#
# add additional static-library creation flags here:
#
XSFLAGS=

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF "$(MSVCDIR)" == ""
!ERROR Variable MSVCDIR not set.
!ENDIF


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp

all : main_dir boost_regex_vc6_sss_dir ./vc6/boost_regex_vc6_sss.lib boost_regex_vc6_mss_dir ./vc6/boost_regex_vc6_mss.lib boost_regex_vc6_sssd_dir ./vc6/boost_regex_vc6_sssd.lib boost_regex_vc6_mssd_dir ./vc6/boost_regex_vc6_mssd.lib boost_regex_vc6_mdid_dir ./vc6/boost_regex_vc6_mdid.lib boost_regex_vc6_mdi_dir ./vc6/boost_regex_vc6_mdi.lib boost_regex_vc6_mds_dir ./vc6/boost_regex_vc6_mds.lib boost_regex_vc6_mdsd_dir ./vc6/boost_regex_vc6_mdsd.lib

clean :  boost_regex_vc6_sss_clean boost_regex_vc6_mss_clean boost_regex_vc6_sssd_clean boost_regex_vc6_mssd_clean boost_regex_vc6_mdid_clean boost_regex_vc6_mdi_clean boost_regex_vc6_mds_clean boost_regex_vc6_mdsd_clean

install : all
	copy vc6\boost_regex_vc6_sss.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mss.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_sssd.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_sssd.pdb "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mssd.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mssd.pdb "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdid.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdid.dll "$(MSVCDIR)\bin"
	copy vc6\boost_regex_vc6_mdid.pdb "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdi.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdi.dll "$(MSVCDIR)\bin"
	copy vc6\boost_regex_vc6_mds.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdsd.lib "$(MSVCDIR)\lib"
	copy vc6\boost_regex_vc6_mdsd.pdb "$(MSVCDIR)\lib"

main_dir :
	@if not exist "vc6\$(NULL)" mkdir vc6


########################################################
#
# section for boost_regex_vc6_sss.lib
#
########################################################
vc6/boost_regex_vc6_sss/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_sss/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_sss/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_sss/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_sss/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_sss/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/instances.cpp

vc6/boost_regex_vc6_sss/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_sss/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/regex.cpp

vc6/boost_regex_vc6_sss/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_sss/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_sss/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_sss/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_sss/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sss/boost_regex_vc6_sss.pch -Fo./vc6/boost_regex_vc6_sss/ -Fdvc6/boost_regex_vc6_sss.pdb ../src/winstances.cpp

boost_regex_vc6_sss_dir :
	@if not exist "vc6\boost_regex_vc6_sss\$(NULL)" mkdir vc6\boost_regex_vc6_sss

boost_regex_vc6_sss_clean :
	del vc6\boost_regex_vc6_sss\*.obj
	del vc6\boost_regex_vc6_sss\*.idb
	del vc6\boost_regex_vc6_sss\*.exp
	del vc6\boost_regex_vc6_sss\*.pch

./vc6/boost_regex_vc6_sss.lib : vc6/boost_regex_vc6_sss/c_regex_traits.obj vc6/boost_regex_vc6_sss/c_regex_traits_common.obj vc6/boost_regex_vc6_sss/cpp_regex_traits.obj vc6/boost_regex_vc6_sss/cregex.obj vc6/boost_regex_vc6_sss/fileiter.obj vc6/boost_regex_vc6_sss/instances.obj vc6/boost_regex_vc6_sss/posix_api.obj vc6/boost_regex_vc6_sss/regex.obj vc6/boost_regex_vc6_sss/regex_debug.obj vc6/boost_regex_vc6_sss/regex_synch.obj vc6/boost_regex_vc6_sss/w32_regex_traits.obj vc6/boost_regex_vc6_sss/wide_posix_api.obj vc6/boost_regex_vc6_sss/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_sss.lib $(XSFLAGS)  vc6/boost_regex_vc6_sss/c_regex_traits.obj vc6/boost_regex_vc6_sss/c_regex_traits_common.obj vc6/boost_regex_vc6_sss/cpp_regex_traits.obj vc6/boost_regex_vc6_sss/cregex.obj vc6/boost_regex_vc6_sss/fileiter.obj vc6/boost_regex_vc6_sss/instances.obj vc6/boost_regex_vc6_sss/posix_api.obj vc6/boost_regex_vc6_sss/regex.obj vc6/boost_regex_vc6_sss/regex_debug.obj vc6/boost_regex_vc6_sss/regex_synch.obj vc6/boost_regex_vc6_sss/w32_regex_traits.obj vc6/boost_regex_vc6_sss/wide_posix_api.obj vc6/boost_regex_vc6_sss/winstances.obj

########################################################
#
# section for boost_regex_vc6_mss.lib
#
########################################################
vc6/boost_regex_vc6_mss/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mss/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mss/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mss/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mss/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mss/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mss/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mss/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mss/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mss/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mss/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mss/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mss/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mss/boost_regex_vc6_mss.pch -Fo./vc6/boost_regex_vc6_mss/ -Fdvc6/boost_regex_vc6_mss.pdb ../src/winstances.cpp

boost_regex_vc6_mss_dir :
	@if not exist "vc6\boost_regex_vc6_mss\$(NULL)" mkdir vc6\boost_regex_vc6_mss

boost_regex_vc6_mss_clean :
	del vc6\boost_regex_vc6_mss\*.obj
	del vc6\boost_regex_vc6_mss\*.idb
	del vc6\boost_regex_vc6_mss\*.exp
	del vc6\boost_regex_vc6_mss\*.pch

./vc6/boost_regex_vc6_mss.lib : vc6/boost_regex_vc6_mss/c_regex_traits.obj vc6/boost_regex_vc6_mss/c_regex_traits_common.obj vc6/boost_regex_vc6_mss/cpp_regex_traits.obj vc6/boost_regex_vc6_mss/cregex.obj vc6/boost_regex_vc6_mss/fileiter.obj vc6/boost_regex_vc6_mss/instances.obj vc6/boost_regex_vc6_mss/posix_api.obj vc6/boost_regex_vc6_mss/regex.obj vc6/boost_regex_vc6_mss/regex_debug.obj vc6/boost_regex_vc6_mss/regex_synch.obj vc6/boost_regex_vc6_mss/w32_regex_traits.obj vc6/boost_regex_vc6_mss/wide_posix_api.obj vc6/boost_regex_vc6_mss/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_mss.lib $(XSFLAGS)  vc6/boost_regex_vc6_mss/c_regex_traits.obj vc6/boost_regex_vc6_mss/c_regex_traits_common.obj vc6/boost_regex_vc6_mss/cpp_regex_traits.obj vc6/boost_regex_vc6_mss/cregex.obj vc6/boost_regex_vc6_mss/fileiter.obj vc6/boost_regex_vc6_mss/instances.obj vc6/boost_regex_vc6_mss/posix_api.obj vc6/boost_regex_vc6_mss/regex.obj vc6/boost_regex_vc6_mss/regex_debug.obj vc6/boost_regex_vc6_mss/regex_synch.obj vc6/boost_regex_vc6_mss/w32_regex_traits.obj vc6/boost_regex_vc6_mss/wide_posix_api.obj vc6/boost_regex_vc6_mss/winstances.obj

########################################################
#
# section for boost_regex_vc6_sssd.lib
#
########################################################
vc6/boost_regex_vc6_sssd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_sssd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_sssd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_sssd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_sssd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_sssd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/instances.cpp

vc6/boost_regex_vc6_sssd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_sssd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/regex.cpp

vc6/boost_regex_vc6_sssd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_sssd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_sssd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_sssd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_sssd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc6/boost_regex_vc6_sssd/boost_regex_vc6_sssd.pch -Fo./vc6/boost_regex_vc6_sssd/ -Fdvc6/boost_regex_vc6_sssd.pdb ../src/winstances.cpp

boost_regex_vc6_sssd_dir :
	@if not exist "vc6\boost_regex_vc6_sssd\$(NULL)" mkdir vc6\boost_regex_vc6_sssd

boost_regex_vc6_sssd_clean :
	del vc6\boost_regex_vc6_sssd\*.obj
	del vc6\boost_regex_vc6_sssd\*.idb
	del vc6\boost_regex_vc6_sssd\*.exp
	del vc6\boost_regex_vc6_sssd\*.pch

./vc6/boost_regex_vc6_sssd.lib : vc6/boost_regex_vc6_sssd/c_regex_traits.obj vc6/boost_regex_vc6_sssd/c_regex_traits_common.obj vc6/boost_regex_vc6_sssd/cpp_regex_traits.obj vc6/boost_regex_vc6_sssd/cregex.obj vc6/boost_regex_vc6_sssd/fileiter.obj vc6/boost_regex_vc6_sssd/instances.obj vc6/boost_regex_vc6_sssd/posix_api.obj vc6/boost_regex_vc6_sssd/regex.obj vc6/boost_regex_vc6_sssd/regex_debug.obj vc6/boost_regex_vc6_sssd/regex_synch.obj vc6/boost_regex_vc6_sssd/w32_regex_traits.obj vc6/boost_regex_vc6_sssd/wide_posix_api.obj vc6/boost_regex_vc6_sssd/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_sssd.lib $(XSFLAGS)  vc6/boost_regex_vc6_sssd/c_regex_traits.obj vc6/boost_regex_vc6_sssd/c_regex_traits_common.obj vc6/boost_regex_vc6_sssd/cpp_regex_traits.obj vc6/boost_regex_vc6_sssd/cregex.obj vc6/boost_regex_vc6_sssd/fileiter.obj vc6/boost_regex_vc6_sssd/instances.obj vc6/boost_regex_vc6_sssd/posix_api.obj vc6/boost_regex_vc6_sssd/regex.obj vc6/boost_regex_vc6_sssd/regex_debug.obj vc6/boost_regex_vc6_sssd/regex_synch.obj vc6/boost_regex_vc6_sssd/w32_regex_traits.obj vc6/boost_regex_vc6_sssd/wide_posix_api.obj vc6/boost_regex_vc6_sssd/winstances.obj

########################################################
#
# section for boost_regex_vc6_mssd.lib
#
########################################################
vc6/boost_regex_vc6_mssd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mssd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mssd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mssd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mssd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mssd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mssd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mssd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mssd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mssd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mssd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mssd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mssd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mssd/boost_regex_vc6_mssd.pch -Fo./vc6/boost_regex_vc6_mssd/ -Fdvc6/boost_regex_vc6_mssd.pdb ../src/winstances.cpp

boost_regex_vc6_mssd_dir :
	@if not exist "vc6\boost_regex_vc6_mssd\$(NULL)" mkdir vc6\boost_regex_vc6_mssd

boost_regex_vc6_mssd_clean :
	del vc6\boost_regex_vc6_mssd\*.obj
	del vc6\boost_regex_vc6_mssd\*.idb
	del vc6\boost_regex_vc6_mssd\*.exp
	del vc6\boost_regex_vc6_mssd\*.pch

./vc6/boost_regex_vc6_mssd.lib : vc6/boost_regex_vc6_mssd/c_regex_traits.obj vc6/boost_regex_vc6_mssd/c_regex_traits_common.obj vc6/boost_regex_vc6_mssd/cpp_regex_traits.obj vc6/boost_regex_vc6_mssd/cregex.obj vc6/boost_regex_vc6_mssd/fileiter.obj vc6/boost_regex_vc6_mssd/instances.obj vc6/boost_regex_vc6_mssd/posix_api.obj vc6/boost_regex_vc6_mssd/regex.obj vc6/boost_regex_vc6_mssd/regex_debug.obj vc6/boost_regex_vc6_mssd/regex_synch.obj vc6/boost_regex_vc6_mssd/w32_regex_traits.obj vc6/boost_regex_vc6_mssd/wide_posix_api.obj vc6/boost_regex_vc6_mssd/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_mssd.lib $(XSFLAGS)  vc6/boost_regex_vc6_mssd/c_regex_traits.obj vc6/boost_regex_vc6_mssd/c_regex_traits_common.obj vc6/boost_regex_vc6_mssd/cpp_regex_traits.obj vc6/boost_regex_vc6_mssd/cregex.obj vc6/boost_regex_vc6_mssd/fileiter.obj vc6/boost_regex_vc6_mssd/instances.obj vc6/boost_regex_vc6_mssd/posix_api.obj vc6/boost_regex_vc6_mssd/regex.obj vc6/boost_regex_vc6_mssd/regex_debug.obj vc6/boost_regex_vc6_mssd/regex_synch.obj vc6/boost_regex_vc6_mssd/w32_regex_traits.obj vc6/boost_regex_vc6_mssd/wide_posix_api.obj vc6/boost_regex_vc6_mssd/winstances.obj

########################################################
#
# section for boost_regex_vc6_mdid.lib
#
########################################################
vc6/boost_regex_vc6_mdid/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mdid/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mdid/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mdid/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mdid/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mdid/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mdid/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mdid/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mdid/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mdid/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mdid/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mdid/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mdid/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdid/boost_regex_vc6_mdid.pch -Fo./vc6/boost_regex_vc6_mdid/ -Fdvc6/boost_regex_vc6_mdid.pdb ../src/winstances.cpp

boost_regex_vc6_mdid_dir :
	@if not exist "vc6\boost_regex_vc6_mdid\$(NULL)" mkdir vc6\boost_regex_vc6_mdid

boost_regex_vc6_mdid_clean :
	del vc6\boost_regex_vc6_mdid\*.obj
	del vc6\boost_regex_vc6_mdid\*.idb
	del vc6\boost_regex_vc6_mdid\*.exp
	del vc6\boost_regex_vc6_mdid\*.pch

./vc6/boost_regex_vc6_mdid.lib : vc6/boost_regex_vc6_mdid/c_regex_traits.obj vc6/boost_regex_vc6_mdid/c_regex_traits_common.obj vc6/boost_regex_vc6_mdid/cpp_regex_traits.obj vc6/boost_regex_vc6_mdid/cregex.obj vc6/boost_regex_vc6_mdid/fileiter.obj vc6/boost_regex_vc6_mdid/instances.obj vc6/boost_regex_vc6_mdid/posix_api.obj vc6/boost_regex_vc6_mdid/regex.obj vc6/boost_regex_vc6_mdid/regex_debug.obj vc6/boost_regex_vc6_mdid/regex_synch.obj vc6/boost_regex_vc6_mdid/w32_regex_traits.obj vc6/boost_regex_vc6_mdid/wide_posix_api.obj vc6/boost_regex_vc6_mdid/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc6/boost_regex_vc6_mdid.pdb" /debug /machine:I386 /out:"vc6/boost_regex_vc6_mdid.dll" /implib:"vc6/boost_regex_vc6_mdid.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc6/boost_regex_vc6_mdid/c_regex_traits.obj vc6/boost_regex_vc6_mdid/c_regex_traits_common.obj vc6/boost_regex_vc6_mdid/cpp_regex_traits.obj vc6/boost_regex_vc6_mdid/cregex.obj vc6/boost_regex_vc6_mdid/fileiter.obj vc6/boost_regex_vc6_mdid/instances.obj vc6/boost_regex_vc6_mdid/posix_api.obj vc6/boost_regex_vc6_mdid/regex.obj vc6/boost_regex_vc6_mdid/regex_debug.obj vc6/boost_regex_vc6_mdid/regex_synch.obj vc6/boost_regex_vc6_mdid/w32_regex_traits.obj vc6/boost_regex_vc6_mdid/wide_posix_api.obj vc6/boost_regex_vc6_mdid/winstances.obj

########################################################
#
# section for boost_regex_vc6_mdi.lib
#
########################################################
vc6/boost_regex_vc6_mdi/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mdi/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mdi/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mdi/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mdi/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mdi/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mdi/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mdi/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mdi/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mdi/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mdi/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mdi/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mdi/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdi/boost_regex_vc6_mdi.pch -Fo./vc6/boost_regex_vc6_mdi/ -Fdvc6/boost_regex_vc6_mdi.pdb ../src/winstances.cpp

boost_regex_vc6_mdi_dir :
	@if not exist "vc6\boost_regex_vc6_mdi\$(NULL)" mkdir vc6\boost_regex_vc6_mdi

boost_regex_vc6_mdi_clean :
	del vc6\boost_regex_vc6_mdi\*.obj
	del vc6\boost_regex_vc6_mdi\*.idb
	del vc6\boost_regex_vc6_mdi\*.exp
	del vc6\boost_regex_vc6_mdi\*.pch

./vc6/boost_regex_vc6_mdi.lib : vc6/boost_regex_vc6_mdi/c_regex_traits.obj vc6/boost_regex_vc6_mdi/c_regex_traits_common.obj vc6/boost_regex_vc6_mdi/cpp_regex_traits.obj vc6/boost_regex_vc6_mdi/cregex.obj vc6/boost_regex_vc6_mdi/fileiter.obj vc6/boost_regex_vc6_mdi/instances.obj vc6/boost_regex_vc6_mdi/posix_api.obj vc6/boost_regex_vc6_mdi/regex.obj vc6/boost_regex_vc6_mdi/regex_debug.obj vc6/boost_regex_vc6_mdi/regex_synch.obj vc6/boost_regex_vc6_mdi/w32_regex_traits.obj vc6/boost_regex_vc6_mdi/wide_posix_api.obj vc6/boost_regex_vc6_mdi/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc6/boost_regex_vc6_mdi.pdb" /debug /machine:I386 /out:"vc6/boost_regex_vc6_mdi.dll" /implib:"vc6/boost_regex_vc6_mdi.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc6/boost_regex_vc6_mdi/c_regex_traits.obj vc6/boost_regex_vc6_mdi/c_regex_traits_common.obj vc6/boost_regex_vc6_mdi/cpp_regex_traits.obj vc6/boost_regex_vc6_mdi/cregex.obj vc6/boost_regex_vc6_mdi/fileiter.obj vc6/boost_regex_vc6_mdi/instances.obj vc6/boost_regex_vc6_mdi/posix_api.obj vc6/boost_regex_vc6_mdi/regex.obj vc6/boost_regex_vc6_mdi/regex_debug.obj vc6/boost_regex_vc6_mdi/regex_synch.obj vc6/boost_regex_vc6_mdi/w32_regex_traits.obj vc6/boost_regex_vc6_mdi/wide_posix_api.obj vc6/boost_regex_vc6_mdi/winstances.obj

########################################################
#
# section for boost_regex_vc6_mds.lib
#
########################################################
vc6/boost_regex_vc6_mds/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mds/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mds/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mds/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mds/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mds/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mds/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mds/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mds/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mds/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mds/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mds/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mds/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mds/boost_regex_vc6_mds.pch -Fo./vc6/boost_regex_vc6_mds/ -Fdvc6/boost_regex_vc6_mds.pdb ../src/winstances.cpp

boost_regex_vc6_mds_dir :
	@if not exist "vc6\boost_regex_vc6_mds\$(NULL)" mkdir vc6\boost_regex_vc6_mds

boost_regex_vc6_mds_clean :
	del vc6\boost_regex_vc6_mds\*.obj
	del vc6\boost_regex_vc6_mds\*.idb
	del vc6\boost_regex_vc6_mds\*.exp
	del vc6\boost_regex_vc6_mds\*.pch

./vc6/boost_regex_vc6_mds.lib : vc6/boost_regex_vc6_mds/c_regex_traits.obj vc6/boost_regex_vc6_mds/c_regex_traits_common.obj vc6/boost_regex_vc6_mds/cpp_regex_traits.obj vc6/boost_regex_vc6_mds/cregex.obj vc6/boost_regex_vc6_mds/fileiter.obj vc6/boost_regex_vc6_mds/instances.obj vc6/boost_regex_vc6_mds/posix_api.obj vc6/boost_regex_vc6_mds/regex.obj vc6/boost_regex_vc6_mds/regex_debug.obj vc6/boost_regex_vc6_mds/regex_synch.obj vc6/boost_regex_vc6_mds/w32_regex_traits.obj vc6/boost_regex_vc6_mds/wide_posix_api.obj vc6/boost_regex_vc6_mds/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_mds.lib $(XSFLAGS)  vc6/boost_regex_vc6_mds/c_regex_traits.obj vc6/boost_regex_vc6_mds/c_regex_traits_common.obj vc6/boost_regex_vc6_mds/cpp_regex_traits.obj vc6/boost_regex_vc6_mds/cregex.obj vc6/boost_regex_vc6_mds/fileiter.obj vc6/boost_regex_vc6_mds/instances.obj vc6/boost_regex_vc6_mds/posix_api.obj vc6/boost_regex_vc6_mds/regex.obj vc6/boost_regex_vc6_mds/regex_debug.obj vc6/boost_regex_vc6_mds/regex_synch.obj vc6/boost_regex_vc6_mds/w32_regex_traits.obj vc6/boost_regex_vc6_mds/wide_posix_api.obj vc6/boost_regex_vc6_mds/winstances.obj

########################################################
#
# section for boost_regex_vc6_mdsd.lib
#
########################################################
vc6/boost_regex_vc6_mdsd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/c_regex_traits.cpp

vc6/boost_regex_vc6_mdsd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/c_regex_traits_common.cpp

vc6/boost_regex_vc6_mdsd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/cpp_regex_traits.cpp

vc6/boost_regex_vc6_mdsd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/cregex.cpp

vc6/boost_regex_vc6_mdsd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/fileiter.cpp

vc6/boost_regex_vc6_mdsd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/instances.cpp

vc6/boost_regex_vc6_mdsd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/posix_api.cpp

vc6/boost_regex_vc6_mdsd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/regex.cpp

vc6/boost_regex_vc6_mdsd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/regex_debug.cpp

vc6/boost_regex_vc6_mdsd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/regex_synch.cpp

vc6/boost_regex_vc6_mdsd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/w32_regex_traits.cpp

vc6/boost_regex_vc6_mdsd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/wide_posix_api.cpp

vc6/boost_regex_vc6_mdsd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6/boost_regex_vc6_mdsd/boost_regex_vc6_mdsd.pch -Fo./vc6/boost_regex_vc6_mdsd/ -Fdvc6/boost_regex_vc6_mdsd.pdb ../src/winstances.cpp

boost_regex_vc6_mdsd_dir :
	@if not exist "vc6\boost_regex_vc6_mdsd\$(NULL)" mkdir vc6\boost_regex_vc6_mdsd

boost_regex_vc6_mdsd_clean :
	del vc6\boost_regex_vc6_mdsd\*.obj
	del vc6\boost_regex_vc6_mdsd\*.idb
	del vc6\boost_regex_vc6_mdsd\*.exp
	del vc6\boost_regex_vc6_mdsd\*.pch

./vc6/boost_regex_vc6_mdsd.lib : vc6/boost_regex_vc6_mdsd/c_regex_traits.obj vc6/boost_regex_vc6_mdsd/c_regex_traits_common.obj vc6/boost_regex_vc6_mdsd/cpp_regex_traits.obj vc6/boost_regex_vc6_mdsd/cregex.obj vc6/boost_regex_vc6_mdsd/fileiter.obj vc6/boost_regex_vc6_mdsd/instances.obj vc6/boost_regex_vc6_mdsd/posix_api.obj vc6/boost_regex_vc6_mdsd/regex.obj vc6/boost_regex_vc6_mdsd/regex_debug.obj vc6/boost_regex_vc6_mdsd/regex_synch.obj vc6/boost_regex_vc6_mdsd/w32_regex_traits.obj vc6/boost_regex_vc6_mdsd/wide_posix_api.obj vc6/boost_regex_vc6_mdsd/winstances.obj
	link -lib /nologo /out:vc6/boost_regex_vc6_mdsd.lib $(XSFLAGS)  vc6/boost_regex_vc6_mdsd/c_regex_traits.obj vc6/boost_regex_vc6_mdsd/c_regex_traits_common.obj vc6/boost_regex_vc6_mdsd/cpp_regex_traits.obj vc6/boost_regex_vc6_mdsd/cregex.obj vc6/boost_regex_vc6_mdsd/fileiter.obj vc6/boost_regex_vc6_mdsd/instances.obj vc6/boost_regex_vc6_mdsd/posix_api.obj vc6/boost_regex_vc6_mdsd/regex.obj vc6/boost_regex_vc6_mdsd/regex_debug.obj vc6/boost_regex_vc6_mdsd/regex_synch.obj vc6/boost_regex_vc6_mdsd/w32_regex_traits.obj vc6/boost_regex_vc6_mdsd/wide_posix_api.obj vc6/boost_regex_vc6_mdsd/winstances.obj



