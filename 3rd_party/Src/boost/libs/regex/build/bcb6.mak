#
# auto generated makefile for C++ Builder
#
# usage:
# make
#   brings libraries up to date
# make install
#   brings libraries up to date and copies binaries to your C++ Builder /lib and /bin directories (recomended)
# make clean
#   removes all temporary files.

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
# add additional libraries to link to here:
#
LIBS=
#
# add additional static-library creation flags here:
#
XSFLAGS=

!ifndef BCROOT
BCROOT=$(MAKEDIR)\..
!endif


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp

all : bcb6 bcb6\boost_regex_bcb6_sss bcb6\boost_regex_bcb6_sss.lib bcb6\boost_regex_bcb6_mss bcb6\boost_regex_bcb6_mss.lib bcb6\boost_regex_bcb6_mdi bcb6\boost_regex_bcb6_mdi.lib bcb6\boost_regex_bcb6_sdi bcb6\boost_regex_bcb6_sdi.lib bcb6\boost_regex_bcb6_mds bcb6\boost_regex_bcb6_mds.lib bcb6\boost_regex_bcb6_sds bcb6\boost_regex_bcb6_sds.lib bcb6\boost_regex_bcb6_sssdd bcb6\boost_regex_bcb6_sssdd.lib bcb6\boost_regex_bcb6_mssdd bcb6\boost_regex_bcb6_mssdd.lib bcb6\boost_regex_bcb6_mdidd bcb6\boost_regex_bcb6_mdidd.lib bcb6\boost_regex_bcb6_sdidd bcb6\boost_regex_bcb6_sdidd.lib bcb6\boost_regex_bcb6_mdsdd bcb6\boost_regex_bcb6_mdsdd.lib bcb6\boost_regex_bcb6_sdsdd bcb6\boost_regex_bcb6_sdsdd.lib

clean :  boost_regex_bcb6_sss_clean boost_regex_bcb6_mss_clean boost_regex_bcb6_mdi_clean boost_regex_bcb6_sdi_clean boost_regex_bcb6_mds_clean boost_regex_bcb6_sds_clean boost_regex_bcb6_sssdd_clean boost_regex_bcb6_mssdd_clean boost_regex_bcb6_mdidd_clean boost_regex_bcb6_sdidd_clean boost_regex_bcb6_mdsdd_clean boost_regex_bcb6_sdsdd_clean

install : all
	copy bcb6\boost_regex_bcb6_sss.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mss.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mdi.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mdi.dll $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_mdi.tds $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_sdi.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_sdi.dll $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_sdi.tds $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_mds.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_sds.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_sssdd.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mssdd.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mdidd.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_mdidd.dll $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_mdidd.tds $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_sdidd.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_sdidd.dll $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_sdidd.tds $(BCROOT)\bin
	copy bcb6\boost_regex_bcb6_mdsdd.lib $(BCROOT)\lib
	copy bcb6\boost_regex_bcb6_sdsdd.lib $(BCROOT)\lib

bcb6 :
	-@mkdir bcb6


########################################################
#
# section for boost_regex_bcb6_sss.lib
#
########################################################
bcb6\boost_regex_bcb6_sss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sss\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sss : 
	-@mkdir bcb6\boost_regex_bcb6_sss

boost_regex_bcb6_sss_clean :
	del bcb6\boost_regex_bcb6_sss\*.obj
	del bcb6\boost_regex_bcb6_sss\*.il?
	del bcb6\boost_regex_bcb6_sss\*.csm
	del bcb6\boost_regex_bcb6_sss\*.tds

bcb6\boost_regex_bcb6_sss.lib : bcb6\boost_regex_bcb6_sss\c_regex_traits.obj bcb6\boost_regex_bcb6_sss\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sss\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sss\cregex.obj bcb6\boost_regex_bcb6_sss\fileiter.obj bcb6\boost_regex_bcb6_sss\instances.obj bcb6\boost_regex_bcb6_sss\posix_api.obj bcb6\boost_regex_bcb6_sss\regex.obj bcb6\boost_regex_bcb6_sss\regex_debug.obj bcb6\boost_regex_bcb6_sss\regex_synch.obj bcb6\boost_regex_bcb6_sss\w32_regex_traits.obj bcb6\boost_regex_bcb6_sss\wide_posix_api.obj bcb6\boost_regex_bcb6_sss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_sss.lib  -+bcb6\boost_regex_bcb6_sss\c_regex_traits.obj -+bcb6\boost_regex_bcb6_sss\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_sss\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_sss\cregex.obj -+bcb6\boost_regex_bcb6_sss\fileiter.obj -+bcb6\boost_regex_bcb6_sss\instances.obj -+bcb6\boost_regex_bcb6_sss\posix_api.obj -+bcb6\boost_regex_bcb6_sss\regex.obj -+bcb6\boost_regex_bcb6_sss\regex_debug.obj -+bcb6\boost_regex_bcb6_sss\regex_synch.obj -+bcb6\boost_regex_bcb6_sss\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_sss\wide_posix_api.obj -+bcb6\boost_regex_bcb6_sss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_mss.lib
#
########################################################
bcb6\boost_regex_bcb6_mss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mss\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mss : 
	-@mkdir bcb6\boost_regex_bcb6_mss

boost_regex_bcb6_mss_clean :
	del bcb6\boost_regex_bcb6_mss\*.obj
	del bcb6\boost_regex_bcb6_mss\*.il?
	del bcb6\boost_regex_bcb6_mss\*.csm
	del bcb6\boost_regex_bcb6_mss\*.tds

bcb6\boost_regex_bcb6_mss.lib : bcb6\boost_regex_bcb6_mss\c_regex_traits.obj bcb6\boost_regex_bcb6_mss\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mss\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mss\cregex.obj bcb6\boost_regex_bcb6_mss\fileiter.obj bcb6\boost_regex_bcb6_mss\instances.obj bcb6\boost_regex_bcb6_mss\posix_api.obj bcb6\boost_regex_bcb6_mss\regex.obj bcb6\boost_regex_bcb6_mss\regex_debug.obj bcb6\boost_regex_bcb6_mss\regex_synch.obj bcb6\boost_regex_bcb6_mss\w32_regex_traits.obj bcb6\boost_regex_bcb6_mss\wide_posix_api.obj bcb6\boost_regex_bcb6_mss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_mss.lib  -+bcb6\boost_regex_bcb6_mss\c_regex_traits.obj -+bcb6\boost_regex_bcb6_mss\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_mss\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_mss\cregex.obj -+bcb6\boost_regex_bcb6_mss\fileiter.obj -+bcb6\boost_regex_bcb6_mss\instances.obj -+bcb6\boost_regex_bcb6_mss\posix_api.obj -+bcb6\boost_regex_bcb6_mss\regex.obj -+bcb6\boost_regex_bcb6_mss\regex_debug.obj -+bcb6\boost_regex_bcb6_mss\regex_synch.obj -+bcb6\boost_regex_bcb6_mss\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_mss\wide_posix_api.obj -+bcb6\boost_regex_bcb6_mss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_mdi.lib
#
########################################################
bcb6\boost_regex_bcb6_mdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdi\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mdi :
	-@mkdir bcb6\boost_regex_bcb6_mdi

boost_regex_bcb6_mdi_clean :
	del bcb6\boost_regex_bcb6_mdi\*.obj
	del bcb6\boost_regex_bcb6_mdi\*.il?
	del bcb6\boost_regex_bcb6_mdi\*.csm
	del bcb6\boost_regex_bcb6_mdi\*.tds
	del bcb6\*.tds

bcb6\boost_regex_bcb6_mdi.lib : bcb6\boost_regex_bcb6_mdi\c_regex_traits.obj bcb6\boost_regex_bcb6_mdi\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mdi\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mdi\cregex.obj bcb6\boost_regex_bcb6_mdi\fileiter.obj bcb6\boost_regex_bcb6_mdi\instances.obj bcb6\boost_regex_bcb6_mdi\posix_api.obj bcb6\boost_regex_bcb6_mdi\regex.obj bcb6\boost_regex_bcb6_mdi\regex_debug.obj bcb6\boost_regex_bcb6_mdi\regex_synch.obj bcb6\boost_regex_bcb6_mdi\w32_regex_traits.obj bcb6\boost_regex_bcb6_mdi\wide_posix_api.obj bcb6\boost_regex_bcb6_mdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb6\boost_regex_bcb6_mdi.dll $(XLFLAGS)  bcb6\boost_regex_bcb6_mdi\c_regex_traits.obj bcb6\boost_regex_bcb6_mdi\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mdi\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mdi\cregex.obj bcb6\boost_regex_bcb6_mdi\fileiter.obj bcb6\boost_regex_bcb6_mdi\instances.obj bcb6\boost_regex_bcb6_mdi\posix_api.obj bcb6\boost_regex_bcb6_mdi\regex.obj bcb6\boost_regex_bcb6_mdi\regex_debug.obj bcb6\boost_regex_bcb6_mdi\regex_synch.obj bcb6\boost_regex_bcb6_mdi\w32_regex_traits.obj bcb6\boost_regex_bcb6_mdi\wide_posix_api.obj bcb6\boost_regex_bcb6_mdi\winstances.obj $(LIBS)
|
	implib -w bcb6\boost_regex_bcb6_mdi.lib bcb6\boost_regex_bcb6_mdi.dll

########################################################
#
# section for boost_regex_bcb6_sdi.lib
#
########################################################
bcb6\boost_regex_bcb6_sdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdi\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sdi :
	-@mkdir bcb6\boost_regex_bcb6_sdi

boost_regex_bcb6_sdi_clean :
	del bcb6\boost_regex_bcb6_sdi\*.obj
	del bcb6\boost_regex_bcb6_sdi\*.il?
	del bcb6\boost_regex_bcb6_sdi\*.csm
	del bcb6\boost_regex_bcb6_sdi\*.tds
	del bcb6\*.tds

bcb6\boost_regex_bcb6_sdi.lib : bcb6\boost_regex_bcb6_sdi\c_regex_traits.obj bcb6\boost_regex_bcb6_sdi\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sdi\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sdi\cregex.obj bcb6\boost_regex_bcb6_sdi\fileiter.obj bcb6\boost_regex_bcb6_sdi\instances.obj bcb6\boost_regex_bcb6_sdi\posix_api.obj bcb6\boost_regex_bcb6_sdi\regex.obj bcb6\boost_regex_bcb6_sdi\regex_debug.obj bcb6\boost_regex_bcb6_sdi\regex_synch.obj bcb6\boost_regex_bcb6_sdi\w32_regex_traits.obj bcb6\boost_regex_bcb6_sdi\wide_posix_api.obj bcb6\boost_regex_bcb6_sdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb6\boost_regex_bcb6_sdi.dll $(XLFLAGS)  bcb6\boost_regex_bcb6_sdi\c_regex_traits.obj bcb6\boost_regex_bcb6_sdi\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sdi\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sdi\cregex.obj bcb6\boost_regex_bcb6_sdi\fileiter.obj bcb6\boost_regex_bcb6_sdi\instances.obj bcb6\boost_regex_bcb6_sdi\posix_api.obj bcb6\boost_regex_bcb6_sdi\regex.obj bcb6\boost_regex_bcb6_sdi\regex_debug.obj bcb6\boost_regex_bcb6_sdi\regex_synch.obj bcb6\boost_regex_bcb6_sdi\w32_regex_traits.obj bcb6\boost_regex_bcb6_sdi\wide_posix_api.obj bcb6\boost_regex_bcb6_sdi\winstances.obj $(LIBS)
|
	implib -w bcb6\boost_regex_bcb6_sdi.lib bcb6\boost_regex_bcb6_sdi.dll

########################################################
#
# section for boost_regex_bcb6_mds.lib
#
########################################################
bcb6\boost_regex_bcb6_mds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mds\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mds : 
	-@mkdir bcb6\boost_regex_bcb6_mds

boost_regex_bcb6_mds_clean :
	del bcb6\boost_regex_bcb6_mds\*.obj
	del bcb6\boost_regex_bcb6_mds\*.il?
	del bcb6\boost_regex_bcb6_mds\*.csm
	del bcb6\boost_regex_bcb6_mds\*.tds

bcb6\boost_regex_bcb6_mds.lib : bcb6\boost_regex_bcb6_mds\c_regex_traits.obj bcb6\boost_regex_bcb6_mds\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mds\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mds\cregex.obj bcb6\boost_regex_bcb6_mds\fileiter.obj bcb6\boost_regex_bcb6_mds\instances.obj bcb6\boost_regex_bcb6_mds\posix_api.obj bcb6\boost_regex_bcb6_mds\regex.obj bcb6\boost_regex_bcb6_mds\regex_debug.obj bcb6\boost_regex_bcb6_mds\regex_synch.obj bcb6\boost_regex_bcb6_mds\w32_regex_traits.obj bcb6\boost_regex_bcb6_mds\wide_posix_api.obj bcb6\boost_regex_bcb6_mds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_mds.lib  -+bcb6\boost_regex_bcb6_mds\c_regex_traits.obj -+bcb6\boost_regex_bcb6_mds\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_mds\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_mds\cregex.obj -+bcb6\boost_regex_bcb6_mds\fileiter.obj -+bcb6\boost_regex_bcb6_mds\instances.obj -+bcb6\boost_regex_bcb6_mds\posix_api.obj -+bcb6\boost_regex_bcb6_mds\regex.obj -+bcb6\boost_regex_bcb6_mds\regex_debug.obj -+bcb6\boost_regex_bcb6_mds\regex_synch.obj -+bcb6\boost_regex_bcb6_mds\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_mds\wide_posix_api.obj -+bcb6\boost_regex_bcb6_mds\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_sds.lib
#
########################################################
bcb6\boost_regex_bcb6_sds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sds\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sds : 
	-@mkdir bcb6\boost_regex_bcb6_sds

boost_regex_bcb6_sds_clean :
	del bcb6\boost_regex_bcb6_sds\*.obj
	del bcb6\boost_regex_bcb6_sds\*.il?
	del bcb6\boost_regex_bcb6_sds\*.csm
	del bcb6\boost_regex_bcb6_sds\*.tds

bcb6\boost_regex_bcb6_sds.lib : bcb6\boost_regex_bcb6_sds\c_regex_traits.obj bcb6\boost_regex_bcb6_sds\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sds\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sds\cregex.obj bcb6\boost_regex_bcb6_sds\fileiter.obj bcb6\boost_regex_bcb6_sds\instances.obj bcb6\boost_regex_bcb6_sds\posix_api.obj bcb6\boost_regex_bcb6_sds\regex.obj bcb6\boost_regex_bcb6_sds\regex_debug.obj bcb6\boost_regex_bcb6_sds\regex_synch.obj bcb6\boost_regex_bcb6_sds\w32_regex_traits.obj bcb6\boost_regex_bcb6_sds\wide_posix_api.obj bcb6\boost_regex_bcb6_sds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_sds.lib  -+bcb6\boost_regex_bcb6_sds\c_regex_traits.obj -+bcb6\boost_regex_bcb6_sds\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_sds\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_sds\cregex.obj -+bcb6\boost_regex_bcb6_sds\fileiter.obj -+bcb6\boost_regex_bcb6_sds\instances.obj -+bcb6\boost_regex_bcb6_sds\posix_api.obj -+bcb6\boost_regex_bcb6_sds\regex.obj -+bcb6\boost_regex_bcb6_sds\regex_debug.obj -+bcb6\boost_regex_bcb6_sds\regex_synch.obj -+bcb6\boost_regex_bcb6_sds\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_sds\wide_posix_api.obj -+bcb6\boost_regex_bcb6_sds\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_sssdd.lib
#
########################################################
bcb6\boost_regex_bcb6_sssdd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sssdd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sssdd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sssdd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sssdd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sssdd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sssdd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sssdd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sssdd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sssdd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sssdd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sssdd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sssdd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_sssdd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sssdd : 
	-@mkdir bcb6\boost_regex_bcb6_sssdd

boost_regex_bcb6_sssdd_clean :
	del bcb6\boost_regex_bcb6_sssdd\*.obj
	del bcb6\boost_regex_bcb6_sssdd\*.il?
	del bcb6\boost_regex_bcb6_sssdd\*.csm
	del bcb6\boost_regex_bcb6_sssdd\*.tds

bcb6\boost_regex_bcb6_sssdd.lib : bcb6\boost_regex_bcb6_sssdd\c_regex_traits.obj bcb6\boost_regex_bcb6_sssdd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sssdd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sssdd\cregex.obj bcb6\boost_regex_bcb6_sssdd\fileiter.obj bcb6\boost_regex_bcb6_sssdd\instances.obj bcb6\boost_regex_bcb6_sssdd\posix_api.obj bcb6\boost_regex_bcb6_sssdd\regex.obj bcb6\boost_regex_bcb6_sssdd\regex_debug.obj bcb6\boost_regex_bcb6_sssdd\regex_synch.obj bcb6\boost_regex_bcb6_sssdd\w32_regex_traits.obj bcb6\boost_regex_bcb6_sssdd\wide_posix_api.obj bcb6\boost_regex_bcb6_sssdd\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_sssdd.lib  -+bcb6\boost_regex_bcb6_sssdd\c_regex_traits.obj -+bcb6\boost_regex_bcb6_sssdd\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_sssdd\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_sssdd\cregex.obj -+bcb6\boost_regex_bcb6_sssdd\fileiter.obj -+bcb6\boost_regex_bcb6_sssdd\instances.obj -+bcb6\boost_regex_bcb6_sssdd\posix_api.obj -+bcb6\boost_regex_bcb6_sssdd\regex.obj -+bcb6\boost_regex_bcb6_sssdd\regex_debug.obj -+bcb6\boost_regex_bcb6_sssdd\regex_synch.obj -+bcb6\boost_regex_bcb6_sssdd\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_sssdd\wide_posix_api.obj -+bcb6\boost_regex_bcb6_sssdd\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_mssdd.lib
#
########################################################
bcb6\boost_regex_bcb6_mssdd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mssdd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mssdd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mssdd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mssdd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mssdd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mssdd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mssdd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mssdd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mssdd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mssdd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mssdd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mssdd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_STLP_DEBUG -D_NO_VCL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb6\boost_regex_bcb6_mssdd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mssdd : 
	-@mkdir bcb6\boost_regex_bcb6_mssdd

boost_regex_bcb6_mssdd_clean :
	del bcb6\boost_regex_bcb6_mssdd\*.obj
	del bcb6\boost_regex_bcb6_mssdd\*.il?
	del bcb6\boost_regex_bcb6_mssdd\*.csm
	del bcb6\boost_regex_bcb6_mssdd\*.tds

bcb6\boost_regex_bcb6_mssdd.lib : bcb6\boost_regex_bcb6_mssdd\c_regex_traits.obj bcb6\boost_regex_bcb6_mssdd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mssdd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mssdd\cregex.obj bcb6\boost_regex_bcb6_mssdd\fileiter.obj bcb6\boost_regex_bcb6_mssdd\instances.obj bcb6\boost_regex_bcb6_mssdd\posix_api.obj bcb6\boost_regex_bcb6_mssdd\regex.obj bcb6\boost_regex_bcb6_mssdd\regex_debug.obj bcb6\boost_regex_bcb6_mssdd\regex_synch.obj bcb6\boost_regex_bcb6_mssdd\w32_regex_traits.obj bcb6\boost_regex_bcb6_mssdd\wide_posix_api.obj bcb6\boost_regex_bcb6_mssdd\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_mssdd.lib  -+bcb6\boost_regex_bcb6_mssdd\c_regex_traits.obj -+bcb6\boost_regex_bcb6_mssdd\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_mssdd\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_mssdd\cregex.obj -+bcb6\boost_regex_bcb6_mssdd\fileiter.obj -+bcb6\boost_regex_bcb6_mssdd\instances.obj -+bcb6\boost_regex_bcb6_mssdd\posix_api.obj -+bcb6\boost_regex_bcb6_mssdd\regex.obj -+bcb6\boost_regex_bcb6_mssdd\regex_debug.obj -+bcb6\boost_regex_bcb6_mssdd\regex_synch.obj -+bcb6\boost_regex_bcb6_mssdd\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_mssdd\wide_posix_api.obj -+bcb6\boost_regex_bcb6_mssdd\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_mdidd.lib
#
########################################################
bcb6\boost_regex_bcb6_mdidd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdidd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mdidd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdidd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mdidd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mdidd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mdidd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mdidd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mdidd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mdidd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mdidd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdidd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mdidd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdidd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mdidd :
	-@mkdir bcb6\boost_regex_bcb6_mdidd

boost_regex_bcb6_mdidd_clean :
	del bcb6\boost_regex_bcb6_mdidd\*.obj
	del bcb6\boost_regex_bcb6_mdidd\*.il?
	del bcb6\boost_regex_bcb6_mdidd\*.csm
	del bcb6\boost_regex_bcb6_mdidd\*.tds
	del bcb6\*.tds

bcb6\boost_regex_bcb6_mdidd.lib : bcb6\boost_regex_bcb6_mdidd\c_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mdidd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\cregex.obj bcb6\boost_regex_bcb6_mdidd\fileiter.obj bcb6\boost_regex_bcb6_mdidd\instances.obj bcb6\boost_regex_bcb6_mdidd\posix_api.obj bcb6\boost_regex_bcb6_mdidd\regex.obj bcb6\boost_regex_bcb6_mdidd\regex_debug.obj bcb6\boost_regex_bcb6_mdidd\regex_synch.obj bcb6\boost_regex_bcb6_mdidd\w32_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\wide_posix_api.obj bcb6\boost_regex_bcb6_mdidd\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWM -tWR -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb6\boost_regex_bcb6_mdidd.dll $(XLFLAGS)  bcb6\boost_regex_bcb6_mdidd\c_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mdidd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\cregex.obj bcb6\boost_regex_bcb6_mdidd\fileiter.obj bcb6\boost_regex_bcb6_mdidd\instances.obj bcb6\boost_regex_bcb6_mdidd\posix_api.obj bcb6\boost_regex_bcb6_mdidd\regex.obj bcb6\boost_regex_bcb6_mdidd\regex_debug.obj bcb6\boost_regex_bcb6_mdidd\regex_synch.obj bcb6\boost_regex_bcb6_mdidd\w32_regex_traits.obj bcb6\boost_regex_bcb6_mdidd\wide_posix_api.obj bcb6\boost_regex_bcb6_mdidd\winstances.obj $(LIBS)
|
	implib -w bcb6\boost_regex_bcb6_mdidd.lib bcb6\boost_regex_bcb6_mdidd.dll

########################################################
#
# section for boost_regex_bcb6_sdidd.lib
#
########################################################
bcb6\boost_regex_bcb6_sdidd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdidd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sdidd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdidd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sdidd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sdidd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sdidd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sdidd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sdidd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sdidd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sdidd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdidd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sdidd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdidd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sdidd :
	-@mkdir bcb6\boost_regex_bcb6_sdidd

boost_regex_bcb6_sdidd_clean :
	del bcb6\boost_regex_bcb6_sdidd\*.obj
	del bcb6\boost_regex_bcb6_sdidd\*.il?
	del bcb6\boost_regex_bcb6_sdidd\*.csm
	del bcb6\boost_regex_bcb6_sdidd\*.tds
	del bcb6\*.tds

bcb6\boost_regex_bcb6_sdidd.lib : bcb6\boost_regex_bcb6_sdidd\c_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sdidd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\cregex.obj bcb6\boost_regex_bcb6_sdidd\fileiter.obj bcb6\boost_regex_bcb6_sdidd\instances.obj bcb6\boost_regex_bcb6_sdidd\posix_api.obj bcb6\boost_regex_bcb6_sdidd\regex.obj bcb6\boost_regex_bcb6_sdidd\regex_debug.obj bcb6\boost_regex_bcb6_sdidd\regex_synch.obj bcb6\boost_regex_bcb6_sdidd\w32_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\wide_posix_api.obj bcb6\boost_regex_bcb6_sdidd\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWR -tWM- -D_STLP_DEBUG -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb6\boost_regex_bcb6_sdidd.dll $(XLFLAGS)  bcb6\boost_regex_bcb6_sdidd\c_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sdidd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\cregex.obj bcb6\boost_regex_bcb6_sdidd\fileiter.obj bcb6\boost_regex_bcb6_sdidd\instances.obj bcb6\boost_regex_bcb6_sdidd\posix_api.obj bcb6\boost_regex_bcb6_sdidd\regex.obj bcb6\boost_regex_bcb6_sdidd\regex_debug.obj bcb6\boost_regex_bcb6_sdidd\regex_synch.obj bcb6\boost_regex_bcb6_sdidd\w32_regex_traits.obj bcb6\boost_regex_bcb6_sdidd\wide_posix_api.obj bcb6\boost_regex_bcb6_sdidd\winstances.obj $(LIBS)
|
	implib -w bcb6\boost_regex_bcb6_sdidd.lib bcb6\boost_regex_bcb6_sdidd.dll

########################################################
#
# section for boost_regex_bcb6_mdsdd.lib
#
########################################################
bcb6\boost_regex_bcb6_mdsdd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdsdd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_mdsdd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdsdd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_mdsdd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_mdsdd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_mdsdd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_mdsdd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_mdsdd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_mdsdd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_mdsdd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_mdsdd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_mdsdd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_mdsdd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_mdsdd : 
	-@mkdir bcb6\boost_regex_bcb6_mdsdd

boost_regex_bcb6_mdsdd_clean :
	del bcb6\boost_regex_bcb6_mdsdd\*.obj
	del bcb6\boost_regex_bcb6_mdsdd\*.il?
	del bcb6\boost_regex_bcb6_mdsdd\*.csm
	del bcb6\boost_regex_bcb6_mdsdd\*.tds

bcb6\boost_regex_bcb6_mdsdd.lib : bcb6\boost_regex_bcb6_mdsdd\c_regex_traits.obj bcb6\boost_regex_bcb6_mdsdd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_mdsdd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_mdsdd\cregex.obj bcb6\boost_regex_bcb6_mdsdd\fileiter.obj bcb6\boost_regex_bcb6_mdsdd\instances.obj bcb6\boost_regex_bcb6_mdsdd\posix_api.obj bcb6\boost_regex_bcb6_mdsdd\regex.obj bcb6\boost_regex_bcb6_mdsdd\regex_debug.obj bcb6\boost_regex_bcb6_mdsdd\regex_synch.obj bcb6\boost_regex_bcb6_mdsdd\w32_regex_traits.obj bcb6\boost_regex_bcb6_mdsdd\wide_posix_api.obj bcb6\boost_regex_bcb6_mdsdd\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_mdsdd.lib  -+bcb6\boost_regex_bcb6_mdsdd\c_regex_traits.obj -+bcb6\boost_regex_bcb6_mdsdd\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_mdsdd\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_mdsdd\cregex.obj -+bcb6\boost_regex_bcb6_mdsdd\fileiter.obj -+bcb6\boost_regex_bcb6_mdsdd\instances.obj -+bcb6\boost_regex_bcb6_mdsdd\posix_api.obj -+bcb6\boost_regex_bcb6_mdsdd\regex.obj -+bcb6\boost_regex_bcb6_mdsdd\regex_debug.obj -+bcb6\boost_regex_bcb6_mdsdd\regex_synch.obj -+bcb6\boost_regex_bcb6_mdsdd\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_mdsdd\wide_posix_api.obj -+bcb6\boost_regex_bcb6_mdsdd\winstances.obj
|

########################################################
#
# section for boost_regex_bcb6_sdsdd.lib
#
########################################################
bcb6\boost_regex_bcb6_sdsdd\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdsdd\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb6\boost_regex_bcb6_sdsdd\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdsdd\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\cregex.obj ../src/cregex.cpp
|

bcb6\boost_regex_bcb6_sdsdd\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\fileiter.obj ../src/fileiter.cpp
|

bcb6\boost_regex_bcb6_sdsdd\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\instances.obj ../src/instances.cpp
|

bcb6\boost_regex_bcb6_sdsdd\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\posix_api.obj ../src/posix_api.cpp
|

bcb6\boost_regex_bcb6_sdsdd\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\regex.obj ../src/regex.cpp
|

bcb6\boost_regex_bcb6_sdsdd\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\regex_debug.obj ../src/regex_debug.cpp
|

bcb6\boost_regex_bcb6_sdsdd\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\regex_synch.obj ../src/regex_synch.cpp
|

bcb6\boost_regex_bcb6_sdsdd\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb6\boost_regex_bcb6_sdsdd\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb6\boost_regex_bcb6_sdsdd\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_STLP_DEBUG -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -v -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb6\boost_regex_bcb6_sdsdd\winstances.obj ../src/winstances.cpp
|

bcb6\boost_regex_bcb6_sdsdd : 
	-@mkdir bcb6\boost_regex_bcb6_sdsdd

boost_regex_bcb6_sdsdd_clean :
	del bcb6\boost_regex_bcb6_sdsdd\*.obj
	del bcb6\boost_regex_bcb6_sdsdd\*.il?
	del bcb6\boost_regex_bcb6_sdsdd\*.csm
	del bcb6\boost_regex_bcb6_sdsdd\*.tds

bcb6\boost_regex_bcb6_sdsdd.lib : bcb6\boost_regex_bcb6_sdsdd\c_regex_traits.obj bcb6\boost_regex_bcb6_sdsdd\c_regex_traits_common.obj bcb6\boost_regex_bcb6_sdsdd\cpp_regex_traits.obj bcb6\boost_regex_bcb6_sdsdd\cregex.obj bcb6\boost_regex_bcb6_sdsdd\fileiter.obj bcb6\boost_regex_bcb6_sdsdd\instances.obj bcb6\boost_regex_bcb6_sdsdd\posix_api.obj bcb6\boost_regex_bcb6_sdsdd\regex.obj bcb6\boost_regex_bcb6_sdsdd\regex_debug.obj bcb6\boost_regex_bcb6_sdsdd\regex_synch.obj bcb6\boost_regex_bcb6_sdsdd\w32_regex_traits.obj bcb6\boost_regex_bcb6_sdsdd\wide_posix_api.obj bcb6\boost_regex_bcb6_sdsdd\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb6\boost_regex_bcb6_sdsdd.lib  -+bcb6\boost_regex_bcb6_sdsdd\c_regex_traits.obj -+bcb6\boost_regex_bcb6_sdsdd\c_regex_traits_common.obj -+bcb6\boost_regex_bcb6_sdsdd\cpp_regex_traits.obj -+bcb6\boost_regex_bcb6_sdsdd\cregex.obj -+bcb6\boost_regex_bcb6_sdsdd\fileiter.obj -+bcb6\boost_regex_bcb6_sdsdd\instances.obj -+bcb6\boost_regex_bcb6_sdsdd\posix_api.obj -+bcb6\boost_regex_bcb6_sdsdd\regex.obj -+bcb6\boost_regex_bcb6_sdsdd\regex_debug.obj -+bcb6\boost_regex_bcb6_sdsdd\regex_synch.obj -+bcb6\boost_regex_bcb6_sdsdd\w32_regex_traits.obj -+bcb6\boost_regex_bcb6_sdsdd\wide_posix_api.obj -+bcb6\boost_regex_bcb6_sdsdd\winstances.obj
|


