#
# auto generated makefile for VC6+STLPort
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

!IF "$(STLPORT_PATH)" == ""
!ERROR Variable STLPORT_PATH not set.
!ENDIF


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp

all : main_dir boost_regex_vc6-stlport_mss_dir ./vc6-stlport/boost_regex_vc6-stlport_mss.lib boost_regex_vc6-stlport_mssd_dir ./vc6-stlport/boost_regex_vc6-stlport_mssd.lib boost_regex_vc6-stlport_mdid_dir ./vc6-stlport/boost_regex_vc6-stlport_mdid.lib boost_regex_vc6-stlport_mdi_dir ./vc6-stlport/boost_regex_vc6-stlport_mdi.lib boost_regex_vc6-stlport_mds_dir ./vc6-stlport/boost_regex_vc6-stlport_mds.lib boost_regex_vc6-stlport_mdsd_dir ./vc6-stlport/boost_regex_vc6-stlport_mdsd.lib boost_regex_vc6-stlport_mdidd_dir ./vc6-stlport/boost_regex_vc6-stlport_mdidd.lib boost_regex_vc6-stlport_mssdd_dir ./vc6-stlport/boost_regex_vc6-stlport_mssdd.lib boost_regex_vc6-stlport_mdsdd_dir ./vc6-stlport/boost_regex_vc6-stlport_mdsdd.lib

clean :  boost_regex_vc6-stlport_mss_clean boost_regex_vc6-stlport_mssd_clean boost_regex_vc6-stlport_mdid_clean boost_regex_vc6-stlport_mdi_clean boost_regex_vc6-stlport_mds_clean boost_regex_vc6-stlport_mdsd_clean boost_regex_vc6-stlport_mdidd_clean boost_regex_vc6-stlport_mssdd_clean boost_regex_vc6-stlport_mdsdd_clean

install : stlport_check all
	copy vc6-stlport\boost_regex_vc6-stlport_mss.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mssd.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdid.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdid.dll "$(MSVCDIR)\bin"
	copy vc6-stlport\boost_regex_vc6-stlport_mdi.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdi.dll "$(MSVCDIR)\bin"
	copy vc6-stlport\boost_regex_vc6-stlport_mds.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdsd.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdidd.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdidd.dll "$(MSVCDIR)\bin"
	copy vc6-stlport\boost_regex_vc6-stlport_mdidd.pdb "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mssdd.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mssdd.pdb "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdsdd.lib "$(MSVCDIR)\lib"
	copy vc6-stlport\boost_regex_vc6-stlport_mdsdd.pdb "$(MSVCDIR)\lib"

main_dir :
	@if not exist "vc6-stlport\$(NULL)" mkdir vc6-stlport

stlport_check : $(STLPORT_PATH)\stlport\string
	echo


########################################################
#
# section for boost_regex_vc6-stlport_mss.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mss/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mss/boost_regex_vc6-stlport_mss.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mss/ -Fdvc6-stlport/boost_regex_vc6-stlport_mss.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mss_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mss\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mss

boost_regex_vc6-stlport_mss_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mss\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mss\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mss\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mss\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mss.lib : vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mss/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mss/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mss/instances.obj vc6-stlport/boost_regex_vc6-stlport_mss/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mss/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mss/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mss.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mss/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mss/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mss/instances.obj vc6-stlport/boost_regex_vc6-stlport_mss/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mss/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mss/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mss/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mss/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mssd.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mssd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssd/boost_regex_vc6-stlport_mssd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssd.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mssd_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mssd\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mssd

boost_regex_vc6-stlport_mssd_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mssd\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mssd\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mssd\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mssd\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mssd.lib : vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mssd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mssd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mssd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mssd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mssd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssd/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mssd.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mssd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mssd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mssd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mssd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mssd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mssd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssd/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mdid.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdid/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdid/boost_regex_vc6-stlport_mdid.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdid/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdid.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mdid_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mdid\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mdid

boost_regex_vc6-stlport_mdid_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mdid\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mdid\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mdid\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mdid\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mdid.lib : vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdid/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdid/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdid/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdid/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdid/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdid/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc6-stlport/boost_regex_vc6-stlport_mdid.pdb" /debug /machine:I386 /out:"vc6-stlport/boost_regex_vc6-stlport_mdid.dll" /implib:"vc6-stlport/boost_regex_vc6-stlport_mdid.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdid/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdid/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdid/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdid/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdid/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdid/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdid/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdid/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mdi.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdi/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /I$(STLPORT_PATH)\stlport /Gy /I../../../ /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdi/boost_regex_vc6-stlport_mdi.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdi/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdi.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mdi_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mdi\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mdi

boost_regex_vc6-stlport_mdi_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mdi\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mdi\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mdi\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mdi\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mdi.lib : vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdi/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdi/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdi/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdi/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdi/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdi/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc6-stlport/boost_regex_vc6-stlport_mdi.pdb" /debug /machine:I386 /out:"vc6-stlport/boost_regex_vc6-stlport_mdi.dll" /implib:"vc6-stlport/boost_regex_vc6-stlport_mdi.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdi/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdi/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdi/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdi/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdi/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdi/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdi/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdi/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mds.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mds/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /YX /FD /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mds/boost_regex_vc6-stlport_mds.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mds/ -Fdvc6-stlport/boost_regex_vc6-stlport_mds.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mds_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mds\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mds

boost_regex_vc6-stlport_mds_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mds\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mds\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mds\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mds\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mds.lib : vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mds/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mds/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mds/instances.obj vc6-stlport/boost_regex_vc6-stlport_mds/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mds/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mds/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mds.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mds/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mds/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mds/instances.obj vc6-stlport/boost_regex_vc6-stlport_mds/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mds/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mds/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mds/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mds/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mdsd.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsd/boost_regex_vc6-stlport_mdsd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsd.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mdsd_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mdsd\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mdsd

boost_regex_vc6-stlport_mdsd_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mdsd\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mdsd\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mdsd\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mdsd\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mdsd.lib : vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mdsd.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsd/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mdidd.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdidd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdidd/boost_regex_vc6-stlport_mdidd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdidd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdidd.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mdidd_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mdidd\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mdidd

boost_regex_vc6-stlport_mdidd_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mdidd\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mdidd\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mdidd\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mdidd\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mdidd.lib : vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc6-stlport/boost_regex_vc6-stlport_mdidd.pdb" /debug /machine:I386 /out:"vc6-stlport/boost_regex_vc6-stlport_mdidd.dll" /implib:"vc6-stlport/boost_regex_vc6-stlport_mdidd.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdidd/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mssdd.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mssdd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I..\..\..\ /D__STL_DEBUG /D_STLP_DEBUG /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /YX /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mssdd/boost_regex_vc6-stlport_mssdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mssdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mssdd.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mssdd_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mssdd\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mssdd

boost_regex_vc6-stlport_mssdd_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mssdd\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mssdd\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mssdd\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mssdd\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mssdd.lib : vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mssdd.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mssdd/winstances.obj

########################################################
#
# section for boost_regex_vc6-stlport_mdsdd.lib
#
########################################################
vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/c_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/c_regex_traits_common.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/cpp_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/cregex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/fileiter.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/instances.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/regex.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/regex_debug.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/regex_synch.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/w32_regex_traits.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/wide_posix_api.cpp

vc6-stlport/boost_regex_vc6-stlport_mdsdd/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I$(STLPORT_PATH)\stlport /I../../../ /DBOOST_REGEX_STATIC_LINK /D__STL_DEBUG /D_STLP_DEBUG /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GZ  /c $(CXXFLAGS) -Fpvc6-stlport/boost_regex_vc6-stlport_mdsdd/boost_regex_vc6-stlport_mdsdd.pch -Fo./vc6-stlport/boost_regex_vc6-stlport_mdsdd/ -Fdvc6-stlport/boost_regex_vc6-stlport_mdsdd.pdb ../src/winstances.cpp

boost_regex_vc6-stlport_mdsdd_dir :
	@if not exist "vc6-stlport\boost_regex_vc6-stlport_mdsdd\$(NULL)" mkdir vc6-stlport\boost_regex_vc6-stlport_mdsdd

boost_regex_vc6-stlport_mdsdd_clean :
	del vc6-stlport\boost_regex_vc6-stlport_mdsdd\*.obj
	del vc6-stlport\boost_regex_vc6-stlport_mdsdd\*.idb
	del vc6-stlport\boost_regex_vc6-stlport_mdsdd\*.exp
	del vc6-stlport\boost_regex_vc6-stlport_mdsdd\*.pch

./vc6-stlport/boost_regex_vc6-stlport_mdsdd.lib : vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/winstances.obj
	link -lib /nologo /out:vc6-stlport/boost_regex_vc6-stlport_mdsdd.lib $(XSFLAGS)  vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/c_regex_traits_common.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/cpp_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/cregex.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/fileiter.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/instances.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_debug.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/regex_synch.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/w32_regex_traits.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/wide_posix_api.obj vc6-stlport/boost_regex_vc6-stlport_mdsdd/winstances.obj


