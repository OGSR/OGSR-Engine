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

all : main_dir boost_regex_vc7_sss_dir ./vc7/boost_regex_vc7_sss.lib boost_regex_vc7_mss_dir ./vc7/boost_regex_vc7_mss.lib boost_regex_vc7_sssd_dir ./vc7/boost_regex_vc7_sssd.lib boost_regex_vc7_mssd_dir ./vc7/boost_regex_vc7_mssd.lib boost_regex_vc7_mdid_dir ./vc7/boost_regex_vc7_mdid.lib boost_regex_vc7_mdi_dir ./vc7/boost_regex_vc7_mdi.lib boost_regex_vc7_mds_dir ./vc7/boost_regex_vc7_mds.lib boost_regex_vc7_mdsd_dir ./vc7/boost_regex_vc7_mdsd.lib

clean :  boost_regex_vc7_sss_clean boost_regex_vc7_mss_clean boost_regex_vc7_sssd_clean boost_regex_vc7_mssd_clean boost_regex_vc7_mdid_clean boost_regex_vc7_mdi_clean boost_regex_vc7_mds_clean boost_regex_vc7_mdsd_clean

install : all
	copy vc7\boost_regex_vc7_sss.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mss.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_sssd.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_sssd.pdb "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mssd.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mssd.pdb "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdid.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdid.dll "$(MSVCDIR)\bin"
	copy vc7\boost_regex_vc7_mdid.pdb "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdi.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdi.dll "$(MSVCDIR)\bin"
	copy vc7\boost_regex_vc7_mds.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdsd.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex_vc7_mdsd.pdb "$(MSVCDIR)\lib"

main_dir :
	@if not exist "vc7\$(NULL)" mkdir vc7


########################################################
#
# section for boost_regex_vc7_sss.lib
#
########################################################
vc7/boost_regex_vc7_sss/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_sss/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_sss/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_sss/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_sss/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_sss/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/instances.cpp

vc7/boost_regex_vc7_sss/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_sss/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/regex.cpp

vc7/boost_regex_vc7_sss/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_sss/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_sss/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_sss/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_sss/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sss/boost_regex_vc7_sss.pch -Fo./vc7/boost_regex_vc7_sss/ -Fdvc7/boost_regex_vc7_sss.pdb ../src/winstances.cpp

boost_regex_vc7_sss_dir :
	@if not exist "vc7\boost_regex_vc7_sss\$(NULL)" mkdir vc7\boost_regex_vc7_sss

boost_regex_vc7_sss_clean :
	del vc7\boost_regex_vc7_sss\*.obj
	del vc7\boost_regex_vc7_sss\*.idb
	del vc7\boost_regex_vc7_sss\*.exp
	del vc7\boost_regex_vc7_sss\*.pch

./vc7/boost_regex_vc7_sss.lib : vc7/boost_regex_vc7_sss/c_regex_traits.obj vc7/boost_regex_vc7_sss/c_regex_traits_common.obj vc7/boost_regex_vc7_sss/cpp_regex_traits.obj vc7/boost_regex_vc7_sss/cregex.obj vc7/boost_regex_vc7_sss/fileiter.obj vc7/boost_regex_vc7_sss/instances.obj vc7/boost_regex_vc7_sss/posix_api.obj vc7/boost_regex_vc7_sss/regex.obj vc7/boost_regex_vc7_sss/regex_debug.obj vc7/boost_regex_vc7_sss/regex_synch.obj vc7/boost_regex_vc7_sss/w32_regex_traits.obj vc7/boost_regex_vc7_sss/wide_posix_api.obj vc7/boost_regex_vc7_sss/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_sss.lib $(XSFLAGS)  vc7/boost_regex_vc7_sss/c_regex_traits.obj vc7/boost_regex_vc7_sss/c_regex_traits_common.obj vc7/boost_regex_vc7_sss/cpp_regex_traits.obj vc7/boost_regex_vc7_sss/cregex.obj vc7/boost_regex_vc7_sss/fileiter.obj vc7/boost_regex_vc7_sss/instances.obj vc7/boost_regex_vc7_sss/posix_api.obj vc7/boost_regex_vc7_sss/regex.obj vc7/boost_regex_vc7_sss/regex_debug.obj vc7/boost_regex_vc7_sss/regex_synch.obj vc7/boost_regex_vc7_sss/w32_regex_traits.obj vc7/boost_regex_vc7_sss/wide_posix_api.obj vc7/boost_regex_vc7_sss/winstances.obj

########################################################
#
# section for boost_regex_vc7_mss.lib
#
########################################################
vc7/boost_regex_vc7_mss/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mss/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mss/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mss/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mss/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mss/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mss/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mss/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mss/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mss/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mss/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mss/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mss/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mss/boost_regex_vc7_mss.pch -Fo./vc7/boost_regex_vc7_mss/ -Fdvc7/boost_regex_vc7_mss.pdb ../src/winstances.cpp

boost_regex_vc7_mss_dir :
	@if not exist "vc7\boost_regex_vc7_mss\$(NULL)" mkdir vc7\boost_regex_vc7_mss

boost_regex_vc7_mss_clean :
	del vc7\boost_regex_vc7_mss\*.obj
	del vc7\boost_regex_vc7_mss\*.idb
	del vc7\boost_regex_vc7_mss\*.exp
	del vc7\boost_regex_vc7_mss\*.pch

./vc7/boost_regex_vc7_mss.lib : vc7/boost_regex_vc7_mss/c_regex_traits.obj vc7/boost_regex_vc7_mss/c_regex_traits_common.obj vc7/boost_regex_vc7_mss/cpp_regex_traits.obj vc7/boost_regex_vc7_mss/cregex.obj vc7/boost_regex_vc7_mss/fileiter.obj vc7/boost_regex_vc7_mss/instances.obj vc7/boost_regex_vc7_mss/posix_api.obj vc7/boost_regex_vc7_mss/regex.obj vc7/boost_regex_vc7_mss/regex_debug.obj vc7/boost_regex_vc7_mss/regex_synch.obj vc7/boost_regex_vc7_mss/w32_regex_traits.obj vc7/boost_regex_vc7_mss/wide_posix_api.obj vc7/boost_regex_vc7_mss/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_mss.lib $(XSFLAGS)  vc7/boost_regex_vc7_mss/c_regex_traits.obj vc7/boost_regex_vc7_mss/c_regex_traits_common.obj vc7/boost_regex_vc7_mss/cpp_regex_traits.obj vc7/boost_regex_vc7_mss/cregex.obj vc7/boost_regex_vc7_mss/fileiter.obj vc7/boost_regex_vc7_mss/instances.obj vc7/boost_regex_vc7_mss/posix_api.obj vc7/boost_regex_vc7_mss/regex.obj vc7/boost_regex_vc7_mss/regex_debug.obj vc7/boost_regex_vc7_mss/regex_synch.obj vc7/boost_regex_vc7_mss/w32_regex_traits.obj vc7/boost_regex_vc7_mss/wide_posix_api.obj vc7/boost_regex_vc7_mss/winstances.obj

########################################################
#
# section for boost_regex_vc7_sssd.lib
#
########################################################
vc7/boost_regex_vc7_sssd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_sssd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_sssd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_sssd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_sssd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_sssd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/instances.cpp

vc7/boost_regex_vc7_sssd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_sssd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/regex.cpp

vc7/boost_regex_vc7_sssd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_sssd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_sssd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_sssd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_sssd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c  $(CXXFLAGS) -Fpvc7/boost_regex_vc7_sssd/boost_regex_vc7_sssd.pch -Fo./vc7/boost_regex_vc7_sssd/ -Fdvc7/boost_regex_vc7_sssd.pdb ../src/winstances.cpp

boost_regex_vc7_sssd_dir :
	@if not exist "vc7\boost_regex_vc7_sssd\$(NULL)" mkdir vc7\boost_regex_vc7_sssd

boost_regex_vc7_sssd_clean :
	del vc7\boost_regex_vc7_sssd\*.obj
	del vc7\boost_regex_vc7_sssd\*.idb
	del vc7\boost_regex_vc7_sssd\*.exp
	del vc7\boost_regex_vc7_sssd\*.pch

./vc7/boost_regex_vc7_sssd.lib : vc7/boost_regex_vc7_sssd/c_regex_traits.obj vc7/boost_regex_vc7_sssd/c_regex_traits_common.obj vc7/boost_regex_vc7_sssd/cpp_regex_traits.obj vc7/boost_regex_vc7_sssd/cregex.obj vc7/boost_regex_vc7_sssd/fileiter.obj vc7/boost_regex_vc7_sssd/instances.obj vc7/boost_regex_vc7_sssd/posix_api.obj vc7/boost_regex_vc7_sssd/regex.obj vc7/boost_regex_vc7_sssd/regex_debug.obj vc7/boost_regex_vc7_sssd/regex_synch.obj vc7/boost_regex_vc7_sssd/w32_regex_traits.obj vc7/boost_regex_vc7_sssd/wide_posix_api.obj vc7/boost_regex_vc7_sssd/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_sssd.lib $(XSFLAGS)  vc7/boost_regex_vc7_sssd/c_regex_traits.obj vc7/boost_regex_vc7_sssd/c_regex_traits_common.obj vc7/boost_regex_vc7_sssd/cpp_regex_traits.obj vc7/boost_regex_vc7_sssd/cregex.obj vc7/boost_regex_vc7_sssd/fileiter.obj vc7/boost_regex_vc7_sssd/instances.obj vc7/boost_regex_vc7_sssd/posix_api.obj vc7/boost_regex_vc7_sssd/regex.obj vc7/boost_regex_vc7_sssd/regex_debug.obj vc7/boost_regex_vc7_sssd/regex_synch.obj vc7/boost_regex_vc7_sssd/w32_regex_traits.obj vc7/boost_regex_vc7_sssd/wide_posix_api.obj vc7/boost_regex_vc7_sssd/winstances.obj

########################################################
#
# section for boost_regex_vc7_mssd.lib
#
########################################################
vc7/boost_regex_vc7_mssd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mssd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mssd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mssd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mssd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mssd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mssd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mssd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mssd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mssd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mssd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mssd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mssd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mssd/boost_regex_vc7_mssd.pch -Fo./vc7/boost_regex_vc7_mssd/ -Fdvc7/boost_regex_vc7_mssd.pdb ../src/winstances.cpp

boost_regex_vc7_mssd_dir :
	@if not exist "vc7\boost_regex_vc7_mssd\$(NULL)" mkdir vc7\boost_regex_vc7_mssd

boost_regex_vc7_mssd_clean :
	del vc7\boost_regex_vc7_mssd\*.obj
	del vc7\boost_regex_vc7_mssd\*.idb
	del vc7\boost_regex_vc7_mssd\*.exp
	del vc7\boost_regex_vc7_mssd\*.pch

./vc7/boost_regex_vc7_mssd.lib : vc7/boost_regex_vc7_mssd/c_regex_traits.obj vc7/boost_regex_vc7_mssd/c_regex_traits_common.obj vc7/boost_regex_vc7_mssd/cpp_regex_traits.obj vc7/boost_regex_vc7_mssd/cregex.obj vc7/boost_regex_vc7_mssd/fileiter.obj vc7/boost_regex_vc7_mssd/instances.obj vc7/boost_regex_vc7_mssd/posix_api.obj vc7/boost_regex_vc7_mssd/regex.obj vc7/boost_regex_vc7_mssd/regex_debug.obj vc7/boost_regex_vc7_mssd/regex_synch.obj vc7/boost_regex_vc7_mssd/w32_regex_traits.obj vc7/boost_regex_vc7_mssd/wide_posix_api.obj vc7/boost_regex_vc7_mssd/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_mssd.lib $(XSFLAGS)  vc7/boost_regex_vc7_mssd/c_regex_traits.obj vc7/boost_regex_vc7_mssd/c_regex_traits_common.obj vc7/boost_regex_vc7_mssd/cpp_regex_traits.obj vc7/boost_regex_vc7_mssd/cregex.obj vc7/boost_regex_vc7_mssd/fileiter.obj vc7/boost_regex_vc7_mssd/instances.obj vc7/boost_regex_vc7_mssd/posix_api.obj vc7/boost_regex_vc7_mssd/regex.obj vc7/boost_regex_vc7_mssd/regex_debug.obj vc7/boost_regex_vc7_mssd/regex_synch.obj vc7/boost_regex_vc7_mssd/w32_regex_traits.obj vc7/boost_regex_vc7_mssd/wide_posix_api.obj vc7/boost_regex_vc7_mssd/winstances.obj

########################################################
#
# section for boost_regex_vc7_mdid.lib
#
########################################################
vc7/boost_regex_vc7_mdid/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mdid/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mdid/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mdid/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mdid/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mdid/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mdid/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mdid/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mdid/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mdid/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mdid/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mdid/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mdid/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdid/boost_regex_vc7_mdid.pch -Fo./vc7/boost_regex_vc7_mdid/ -Fdvc7/boost_regex_vc7_mdid.pdb ../src/winstances.cpp

boost_regex_vc7_mdid_dir :
	@if not exist "vc7\boost_regex_vc7_mdid\$(NULL)" mkdir vc7\boost_regex_vc7_mdid

boost_regex_vc7_mdid_clean :
	del vc7\boost_regex_vc7_mdid\*.obj
	del vc7\boost_regex_vc7_mdid\*.idb
	del vc7\boost_regex_vc7_mdid\*.exp
	del vc7\boost_regex_vc7_mdid\*.pch

./vc7/boost_regex_vc7_mdid.lib : vc7/boost_regex_vc7_mdid/c_regex_traits.obj vc7/boost_regex_vc7_mdid/c_regex_traits_common.obj vc7/boost_regex_vc7_mdid/cpp_regex_traits.obj vc7/boost_regex_vc7_mdid/cregex.obj vc7/boost_regex_vc7_mdid/fileiter.obj vc7/boost_regex_vc7_mdid/instances.obj vc7/boost_regex_vc7_mdid/posix_api.obj vc7/boost_regex_vc7_mdid/regex.obj vc7/boost_regex_vc7_mdid/regex_debug.obj vc7/boost_regex_vc7_mdid/regex_synch.obj vc7/boost_regex_vc7_mdid/w32_regex_traits.obj vc7/boost_regex_vc7_mdid/wide_posix_api.obj vc7/boost_regex_vc7_mdid/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc7/boost_regex_vc7_mdid.pdb" /debug /machine:I386 /out:"vc7/boost_regex_vc7_mdid.dll" /implib:"vc7/boost_regex_vc7_mdid.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc7/boost_regex_vc7_mdid/c_regex_traits.obj vc7/boost_regex_vc7_mdid/c_regex_traits_common.obj vc7/boost_regex_vc7_mdid/cpp_regex_traits.obj vc7/boost_regex_vc7_mdid/cregex.obj vc7/boost_regex_vc7_mdid/fileiter.obj vc7/boost_regex_vc7_mdid/instances.obj vc7/boost_regex_vc7_mdid/posix_api.obj vc7/boost_regex_vc7_mdid/regex.obj vc7/boost_regex_vc7_mdid/regex_debug.obj vc7/boost_regex_vc7_mdid/regex_synch.obj vc7/boost_regex_vc7_mdid/w32_regex_traits.obj vc7/boost_regex_vc7_mdid/wide_posix_api.obj vc7/boost_regex_vc7_mdid/winstances.obj

########################################################
#
# section for boost_regex_vc7_mdi.lib
#
########################################################
vc7/boost_regex_vc7_mdi/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mdi/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mdi/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mdi/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mdi/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mdi/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mdi/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mdi/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mdi/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mdi/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mdi/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mdi/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mdi/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdi/boost_regex_vc7_mdi.pch -Fo./vc7/boost_regex_vc7_mdi/ -Fdvc7/boost_regex_vc7_mdi.pdb ../src/winstances.cpp

boost_regex_vc7_mdi_dir :
	@if not exist "vc7\boost_regex_vc7_mdi\$(NULL)" mkdir vc7\boost_regex_vc7_mdi

boost_regex_vc7_mdi_clean :
	del vc7\boost_regex_vc7_mdi\*.obj
	del vc7\boost_regex_vc7_mdi\*.idb
	del vc7\boost_regex_vc7_mdi\*.exp
	del vc7\boost_regex_vc7_mdi\*.pch

./vc7/boost_regex_vc7_mdi.lib : vc7/boost_regex_vc7_mdi/c_regex_traits.obj vc7/boost_regex_vc7_mdi/c_regex_traits_common.obj vc7/boost_regex_vc7_mdi/cpp_regex_traits.obj vc7/boost_regex_vc7_mdi/cregex.obj vc7/boost_regex_vc7_mdi/fileiter.obj vc7/boost_regex_vc7_mdi/instances.obj vc7/boost_regex_vc7_mdi/posix_api.obj vc7/boost_regex_vc7_mdi/regex.obj vc7/boost_regex_vc7_mdi/regex_debug.obj vc7/boost_regex_vc7_mdi/regex_synch.obj vc7/boost_regex_vc7_mdi/w32_regex_traits.obj vc7/boost_regex_vc7_mdi/wide_posix_api.obj vc7/boost_regex_vc7_mdi/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc7/boost_regex_vc7_mdi.pdb" /debug /machine:I386 /out:"vc7/boost_regex_vc7_mdi.dll" /implib:"vc7/boost_regex_vc7_mdi.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc7/boost_regex_vc7_mdi/c_regex_traits.obj vc7/boost_regex_vc7_mdi/c_regex_traits_common.obj vc7/boost_regex_vc7_mdi/cpp_regex_traits.obj vc7/boost_regex_vc7_mdi/cregex.obj vc7/boost_regex_vc7_mdi/fileiter.obj vc7/boost_regex_vc7_mdi/instances.obj vc7/boost_regex_vc7_mdi/posix_api.obj vc7/boost_regex_vc7_mdi/regex.obj vc7/boost_regex_vc7_mdi/regex_debug.obj vc7/boost_regex_vc7_mdi/regex_synch.obj vc7/boost_regex_vc7_mdi/w32_regex_traits.obj vc7/boost_regex_vc7_mdi/wide_posix_api.obj vc7/boost_regex_vc7_mdi/winstances.obj

########################################################
#
# section for boost_regex_vc7_mds.lib
#
########################################################
vc7/boost_regex_vc7_mds/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mds/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mds/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mds/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mds/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mds/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mds/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mds/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mds/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mds/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mds/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mds/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mds/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mds/boost_regex_vc7_mds.pch -Fo./vc7/boost_regex_vc7_mds/ -Fdvc7/boost_regex_vc7_mds.pdb ../src/winstances.cpp

boost_regex_vc7_mds_dir :
	@if not exist "vc7\boost_regex_vc7_mds\$(NULL)" mkdir vc7\boost_regex_vc7_mds

boost_regex_vc7_mds_clean :
	del vc7\boost_regex_vc7_mds\*.obj
	del vc7\boost_regex_vc7_mds\*.idb
	del vc7\boost_regex_vc7_mds\*.exp
	del vc7\boost_regex_vc7_mds\*.pch

./vc7/boost_regex_vc7_mds.lib : vc7/boost_regex_vc7_mds/c_regex_traits.obj vc7/boost_regex_vc7_mds/c_regex_traits_common.obj vc7/boost_regex_vc7_mds/cpp_regex_traits.obj vc7/boost_regex_vc7_mds/cregex.obj vc7/boost_regex_vc7_mds/fileiter.obj vc7/boost_regex_vc7_mds/instances.obj vc7/boost_regex_vc7_mds/posix_api.obj vc7/boost_regex_vc7_mds/regex.obj vc7/boost_regex_vc7_mds/regex_debug.obj vc7/boost_regex_vc7_mds/regex_synch.obj vc7/boost_regex_vc7_mds/w32_regex_traits.obj vc7/boost_regex_vc7_mds/wide_posix_api.obj vc7/boost_regex_vc7_mds/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_mds.lib $(XSFLAGS)  vc7/boost_regex_vc7_mds/c_regex_traits.obj vc7/boost_regex_vc7_mds/c_regex_traits_common.obj vc7/boost_regex_vc7_mds/cpp_regex_traits.obj vc7/boost_regex_vc7_mds/cregex.obj vc7/boost_regex_vc7_mds/fileiter.obj vc7/boost_regex_vc7_mds/instances.obj vc7/boost_regex_vc7_mds/posix_api.obj vc7/boost_regex_vc7_mds/regex.obj vc7/boost_regex_vc7_mds/regex_debug.obj vc7/boost_regex_vc7_mds/regex_synch.obj vc7/boost_regex_vc7_mds/w32_regex_traits.obj vc7/boost_regex_vc7_mds/wide_posix_api.obj vc7/boost_regex_vc7_mds/winstances.obj

########################################################
#
# section for boost_regex_vc7_mdsd.lib
#
########################################################
vc7/boost_regex_vc7_mdsd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/c_regex_traits.cpp

vc7/boost_regex_vc7_mdsd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex_vc7_mdsd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex_vc7_mdsd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/cregex.cpp

vc7/boost_regex_vc7_mdsd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/fileiter.cpp

vc7/boost_regex_vc7_mdsd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/instances.cpp

vc7/boost_regex_vc7_mdsd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/posix_api.cpp

vc7/boost_regex_vc7_mdsd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/regex.cpp

vc7/boost_regex_vc7_mdsd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/regex_debug.cpp

vc7/boost_regex_vc7_mdsd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/regex_synch.cpp

vc7/boost_regex_vc7_mdsd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex_vc7_mdsd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/wide_posix_api.cpp

vc7/boost_regex_vc7_mdsd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc7/boost_regex_vc7_mdsd/boost_regex_vc7_mdsd.pch -Fo./vc7/boost_regex_vc7_mdsd/ -Fdvc7/boost_regex_vc7_mdsd.pdb ../src/winstances.cpp

boost_regex_vc7_mdsd_dir :
	@if not exist "vc7\boost_regex_vc7_mdsd\$(NULL)" mkdir vc7\boost_regex_vc7_mdsd

boost_regex_vc7_mdsd_clean :
	del vc7\boost_regex_vc7_mdsd\*.obj
	del vc7\boost_regex_vc7_mdsd\*.idb
	del vc7\boost_regex_vc7_mdsd\*.exp
	del vc7\boost_regex_vc7_mdsd\*.pch

./vc7/boost_regex_vc7_mdsd.lib : vc7/boost_regex_vc7_mdsd/c_regex_traits.obj vc7/boost_regex_vc7_mdsd/c_regex_traits_common.obj vc7/boost_regex_vc7_mdsd/cpp_regex_traits.obj vc7/boost_regex_vc7_mdsd/cregex.obj vc7/boost_regex_vc7_mdsd/fileiter.obj vc7/boost_regex_vc7_mdsd/instances.obj vc7/boost_regex_vc7_mdsd/posix_api.obj vc7/boost_regex_vc7_mdsd/regex.obj vc7/boost_regex_vc7_mdsd/regex_debug.obj vc7/boost_regex_vc7_mdsd/regex_synch.obj vc7/boost_regex_vc7_mdsd/w32_regex_traits.obj vc7/boost_regex_vc7_mdsd/wide_posix_api.obj vc7/boost_regex_vc7_mdsd/winstances.obj
	link -lib /nologo /out:vc7/boost_regex_vc7_mdsd.lib $(XSFLAGS)  vc7/boost_regex_vc7_mdsd/c_regex_traits.obj vc7/boost_regex_vc7_mdsd/c_regex_traits_common.obj vc7/boost_regex_vc7_mdsd/cpp_regex_traits.obj vc7/boost_regex_vc7_mdsd/cregex.obj vc7/boost_regex_vc7_mdsd/fileiter.obj vc7/boost_regex_vc7_mdsd/instances.obj vc7/boost_regex_vc7_mdsd/posix_api.obj vc7/boost_regex_vc7_mdsd/regex.obj vc7/boost_regex_vc7_mdsd/regex_debug.obj vc7/boost_regex_vc7_mdsd/regex_synch.obj vc7/boost_regex_vc7_mdsd/w32_regex_traits.obj vc7/boost_regex_vc7_mdsd/wide_posix_api.obj vc7/boost_regex_vc7_mdsd/winstances.obj


