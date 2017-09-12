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

all : bcb4 bcb4\boost_regex_bcb4_sss bcb4\boost_regex_bcb4_sss.lib bcb4\boost_regex_bcb4_mss bcb4\boost_regex_bcb4_mss.lib bcb4\boost_regex_bcb4_mdi bcb4\boost_regex_bcb4_mdi.lib bcb4\boost_regex_bcb4_sdi bcb4\boost_regex_bcb4_sdi.lib bcb4\boost_regex_bcb4_mds bcb4\boost_regex_bcb4_mds.lib bcb4\boost_regex_bcb4_sds bcb4\boost_regex_bcb4_sds.lib

clean :  boost_regex_bcb4_sss_clean boost_regex_bcb4_mss_clean boost_regex_bcb4_mdi_clean boost_regex_bcb4_sdi_clean boost_regex_bcb4_mds_clean boost_regex_bcb4_sds_clean

install : all
	copy bcb4\boost_regex_bcb4_sss.lib $(BCROOT)\lib
	copy bcb4\boost_regex_bcb4_mss.lib $(BCROOT)\lib
	copy bcb4\boost_regex_bcb4_mdi.lib $(BCROOT)\lib
	copy bcb4\boost_regex_bcb4_mdi.dll $(BCROOT)\bin
	copy bcb4\boost_regex_bcb4_mdi.tds $(BCROOT)\bin
	copy bcb4\boost_regex_bcb4_sdi.lib $(BCROOT)\lib
	copy bcb4\boost_regex_bcb4_sdi.dll $(BCROOT)\bin
	copy bcb4\boost_regex_bcb4_sdi.tds $(BCROOT)\bin
	copy bcb4\boost_regex_bcb4_mds.lib $(BCROOT)\lib
	copy bcb4\boost_regex_bcb4_sds.lib $(BCROOT)\lib

bcb4 :
	-@mkdir bcb4


########################################################
#
# section for boost_regex_bcb4_sss.lib
#
########################################################
bcb4\boost_regex_bcb4_sss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_sss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_sss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_sss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_sss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_sss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_sss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_sss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_sss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_sss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_sss\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_sss : 
	-@mkdir bcb4\boost_regex_bcb4_sss

boost_regex_bcb4_sss_clean :
	del bcb4\boost_regex_bcb4_sss\*.obj
	del bcb4\boost_regex_bcb4_sss\*.il?
	del bcb4\boost_regex_bcb4_sss\*.csm
	del bcb4\boost_regex_bcb4_sss\*.tds

bcb4\boost_regex_bcb4_sss.lib : bcb4\boost_regex_bcb4_sss\c_regex_traits.obj bcb4\boost_regex_bcb4_sss\c_regex_traits_common.obj bcb4\boost_regex_bcb4_sss\cpp_regex_traits.obj bcb4\boost_regex_bcb4_sss\cregex.obj bcb4\boost_regex_bcb4_sss\fileiter.obj bcb4\boost_regex_bcb4_sss\instances.obj bcb4\boost_regex_bcb4_sss\posix_api.obj bcb4\boost_regex_bcb4_sss\regex.obj bcb4\boost_regex_bcb4_sss\regex_debug.obj bcb4\boost_regex_bcb4_sss\regex_synch.obj bcb4\boost_regex_bcb4_sss\w32_regex_traits.obj bcb4\boost_regex_bcb4_sss\wide_posix_api.obj bcb4\boost_regex_bcb4_sss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb4\boost_regex_bcb4_sss.lib  -+bcb4\boost_regex_bcb4_sss\c_regex_traits.obj -+bcb4\boost_regex_bcb4_sss\c_regex_traits_common.obj -+bcb4\boost_regex_bcb4_sss\cpp_regex_traits.obj -+bcb4\boost_regex_bcb4_sss\cregex.obj -+bcb4\boost_regex_bcb4_sss\fileiter.obj -+bcb4\boost_regex_bcb4_sss\instances.obj -+bcb4\boost_regex_bcb4_sss\posix_api.obj -+bcb4\boost_regex_bcb4_sss\regex.obj -+bcb4\boost_regex_bcb4_sss\regex_debug.obj -+bcb4\boost_regex_bcb4_sss\regex_synch.obj -+bcb4\boost_regex_bcb4_sss\w32_regex_traits.obj -+bcb4\boost_regex_bcb4_sss\wide_posix_api.obj -+bcb4\boost_regex_bcb4_sss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb4_mss.lib
#
########################################################
bcb4\boost_regex_bcb4_mss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_mss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_mss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_mss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_mss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_mss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_mss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_mss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_mss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_mss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb4\boost_regex_bcb4_mss\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_mss : 
	-@mkdir bcb4\boost_regex_bcb4_mss

boost_regex_bcb4_mss_clean :
	del bcb4\boost_regex_bcb4_mss\*.obj
	del bcb4\boost_regex_bcb4_mss\*.il?
	del bcb4\boost_regex_bcb4_mss\*.csm
	del bcb4\boost_regex_bcb4_mss\*.tds

bcb4\boost_regex_bcb4_mss.lib : bcb4\boost_regex_bcb4_mss\c_regex_traits.obj bcb4\boost_regex_bcb4_mss\c_regex_traits_common.obj bcb4\boost_regex_bcb4_mss\cpp_regex_traits.obj bcb4\boost_regex_bcb4_mss\cregex.obj bcb4\boost_regex_bcb4_mss\fileiter.obj bcb4\boost_regex_bcb4_mss\instances.obj bcb4\boost_regex_bcb4_mss\posix_api.obj bcb4\boost_regex_bcb4_mss\regex.obj bcb4\boost_regex_bcb4_mss\regex_debug.obj bcb4\boost_regex_bcb4_mss\regex_synch.obj bcb4\boost_regex_bcb4_mss\w32_regex_traits.obj bcb4\boost_regex_bcb4_mss\wide_posix_api.obj bcb4\boost_regex_bcb4_mss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb4\boost_regex_bcb4_mss.lib  -+bcb4\boost_regex_bcb4_mss\c_regex_traits.obj -+bcb4\boost_regex_bcb4_mss\c_regex_traits_common.obj -+bcb4\boost_regex_bcb4_mss\cpp_regex_traits.obj -+bcb4\boost_regex_bcb4_mss\cregex.obj -+bcb4\boost_regex_bcb4_mss\fileiter.obj -+bcb4\boost_regex_bcb4_mss\instances.obj -+bcb4\boost_regex_bcb4_mss\posix_api.obj -+bcb4\boost_regex_bcb4_mss\regex.obj -+bcb4\boost_regex_bcb4_mss\regex_debug.obj -+bcb4\boost_regex_bcb4_mss\regex_synch.obj -+bcb4\boost_regex_bcb4_mss\w32_regex_traits.obj -+bcb4\boost_regex_bcb4_mss\wide_posix_api.obj -+bcb4\boost_regex_bcb4_mss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb4_mdi.lib
#
########################################################
bcb4\boost_regex_bcb4_mdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_mdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_mdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_mdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_mdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_mdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_mdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_mdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_mdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_mdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mdi\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_mdi :
	-@mkdir bcb4\boost_regex_bcb4_mdi

boost_regex_bcb4_mdi_clean :
	del bcb4\boost_regex_bcb4_mdi\*.obj
	del bcb4\boost_regex_bcb4_mdi\*.il?
	del bcb4\boost_regex_bcb4_mdi\*.csm
	del bcb4\boost_regex_bcb4_mdi\*.tds
	del bcb4\*.tds

bcb4\boost_regex_bcb4_mdi.lib : bcb4\boost_regex_bcb4_mdi\c_regex_traits.obj bcb4\boost_regex_bcb4_mdi\c_regex_traits_common.obj bcb4\boost_regex_bcb4_mdi\cpp_regex_traits.obj bcb4\boost_regex_bcb4_mdi\cregex.obj bcb4\boost_regex_bcb4_mdi\fileiter.obj bcb4\boost_regex_bcb4_mdi\instances.obj bcb4\boost_regex_bcb4_mdi\posix_api.obj bcb4\boost_regex_bcb4_mdi\regex.obj bcb4\boost_regex_bcb4_mdi\regex_debug.obj bcb4\boost_regex_bcb4_mdi\regex_synch.obj bcb4\boost_regex_bcb4_mdi\w32_regex_traits.obj bcb4\boost_regex_bcb4_mdi\wide_posix_api.obj bcb4\boost_regex_bcb4_mdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb4\boost_regex_bcb4_mdi.dll $(XLFLAGS)  bcb4\boost_regex_bcb4_mdi\c_regex_traits.obj bcb4\boost_regex_bcb4_mdi\c_regex_traits_common.obj bcb4\boost_regex_bcb4_mdi\cpp_regex_traits.obj bcb4\boost_regex_bcb4_mdi\cregex.obj bcb4\boost_regex_bcb4_mdi\fileiter.obj bcb4\boost_regex_bcb4_mdi\instances.obj bcb4\boost_regex_bcb4_mdi\posix_api.obj bcb4\boost_regex_bcb4_mdi\regex.obj bcb4\boost_regex_bcb4_mdi\regex_debug.obj bcb4\boost_regex_bcb4_mdi\regex_synch.obj bcb4\boost_regex_bcb4_mdi\w32_regex_traits.obj bcb4\boost_regex_bcb4_mdi\wide_posix_api.obj bcb4\boost_regex_bcb4_mdi\winstances.obj $(LIBS)
|
	implib -w bcb4\boost_regex_bcb4_mdi.lib bcb4\boost_regex_bcb4_mdi.dll

########################################################
#
# section for boost_regex_bcb4_sdi.lib
#
########################################################
bcb4\boost_regex_bcb4_sdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_sdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_sdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_sdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_sdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_sdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_sdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_sdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_sdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_sdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sdi\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_sdi :
	-@mkdir bcb4\boost_regex_bcb4_sdi

boost_regex_bcb4_sdi_clean :
	del bcb4\boost_regex_bcb4_sdi\*.obj
	del bcb4\boost_regex_bcb4_sdi\*.il?
	del bcb4\boost_regex_bcb4_sdi\*.csm
	del bcb4\boost_regex_bcb4_sdi\*.tds
	del bcb4\*.tds

bcb4\boost_regex_bcb4_sdi.lib : bcb4\boost_regex_bcb4_sdi\c_regex_traits.obj bcb4\boost_regex_bcb4_sdi\c_regex_traits_common.obj bcb4\boost_regex_bcb4_sdi\cpp_regex_traits.obj bcb4\boost_regex_bcb4_sdi\cregex.obj bcb4\boost_regex_bcb4_sdi\fileiter.obj bcb4\boost_regex_bcb4_sdi\instances.obj bcb4\boost_regex_bcb4_sdi\posix_api.obj bcb4\boost_regex_bcb4_sdi\regex.obj bcb4\boost_regex_bcb4_sdi\regex_debug.obj bcb4\boost_regex_bcb4_sdi\regex_synch.obj bcb4\boost_regex_bcb4_sdi\w32_regex_traits.obj bcb4\boost_regex_bcb4_sdi\wide_posix_api.obj bcb4\boost_regex_bcb4_sdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb4\boost_regex_bcb4_sdi.dll $(XLFLAGS)  bcb4\boost_regex_bcb4_sdi\c_regex_traits.obj bcb4\boost_regex_bcb4_sdi\c_regex_traits_common.obj bcb4\boost_regex_bcb4_sdi\cpp_regex_traits.obj bcb4\boost_regex_bcb4_sdi\cregex.obj bcb4\boost_regex_bcb4_sdi\fileiter.obj bcb4\boost_regex_bcb4_sdi\instances.obj bcb4\boost_regex_bcb4_sdi\posix_api.obj bcb4\boost_regex_bcb4_sdi\regex.obj bcb4\boost_regex_bcb4_sdi\regex_debug.obj bcb4\boost_regex_bcb4_sdi\regex_synch.obj bcb4\boost_regex_bcb4_sdi\w32_regex_traits.obj bcb4\boost_regex_bcb4_sdi\wide_posix_api.obj bcb4\boost_regex_bcb4_sdi\winstances.obj $(LIBS)
|
	implib -w bcb4\boost_regex_bcb4_sdi.lib bcb4\boost_regex_bcb4_sdi.dll

########################################################
#
# section for boost_regex_bcb4_mds.lib
#
########################################################
bcb4\boost_regex_bcb4_mds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_mds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_mds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_mds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_mds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_mds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_mds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_mds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_mds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_mds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_mds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_mds\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_mds : 
	-@mkdir bcb4\boost_regex_bcb4_mds

boost_regex_bcb4_mds_clean :
	del bcb4\boost_regex_bcb4_mds\*.obj
	del bcb4\boost_regex_bcb4_mds\*.il?
	del bcb4\boost_regex_bcb4_mds\*.csm
	del bcb4\boost_regex_bcb4_mds\*.tds

bcb4\boost_regex_bcb4_mds.lib : bcb4\boost_regex_bcb4_mds\c_regex_traits.obj bcb4\boost_regex_bcb4_mds\c_regex_traits_common.obj bcb4\boost_regex_bcb4_mds\cpp_regex_traits.obj bcb4\boost_regex_bcb4_mds\cregex.obj bcb4\boost_regex_bcb4_mds\fileiter.obj bcb4\boost_regex_bcb4_mds\instances.obj bcb4\boost_regex_bcb4_mds\posix_api.obj bcb4\boost_regex_bcb4_mds\regex.obj bcb4\boost_regex_bcb4_mds\regex_debug.obj bcb4\boost_regex_bcb4_mds\regex_synch.obj bcb4\boost_regex_bcb4_mds\w32_regex_traits.obj bcb4\boost_regex_bcb4_mds\wide_posix_api.obj bcb4\boost_regex_bcb4_mds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb4\boost_regex_bcb4_mds.lib  -+bcb4\boost_regex_bcb4_mds\c_regex_traits.obj -+bcb4\boost_regex_bcb4_mds\c_regex_traits_common.obj -+bcb4\boost_regex_bcb4_mds\cpp_regex_traits.obj -+bcb4\boost_regex_bcb4_mds\cregex.obj -+bcb4\boost_regex_bcb4_mds\fileiter.obj -+bcb4\boost_regex_bcb4_mds\instances.obj -+bcb4\boost_regex_bcb4_mds\posix_api.obj -+bcb4\boost_regex_bcb4_mds\regex.obj -+bcb4\boost_regex_bcb4_mds\regex_debug.obj -+bcb4\boost_regex_bcb4_mds\regex_synch.obj -+bcb4\boost_regex_bcb4_mds\w32_regex_traits.obj -+bcb4\boost_regex_bcb4_mds\wide_posix_api.obj -+bcb4\boost_regex_bcb4_mds\winstances.obj
|

########################################################
#
# section for boost_regex_bcb4_sds.lib
#
########################################################
bcb4\boost_regex_bcb4_sds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb4\boost_regex_bcb4_sds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\cregex.obj ../src/cregex.cpp
|

bcb4\boost_regex_bcb4_sds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\fileiter.obj ../src/fileiter.cpp
|

bcb4\boost_regex_bcb4_sds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\instances.obj ../src/instances.cpp
|

bcb4\boost_regex_bcb4_sds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\posix_api.obj ../src/posix_api.cpp
|

bcb4\boost_regex_bcb4_sds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\regex.obj ../src/regex.cpp
|

bcb4\boost_regex_bcb4_sds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\regex_debug.obj ../src/regex_debug.cpp
|

bcb4\boost_regex_bcb4_sds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\regex_synch.obj ../src/regex_synch.cpp
|

bcb4\boost_regex_bcb4_sds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb4\boost_regex_bcb4_sds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb4\boost_regex_bcb4_sds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb4\boost_regex_bcb4_sds\winstances.obj ../src/winstances.cpp
|

bcb4\boost_regex_bcb4_sds : 
	-@mkdir bcb4\boost_regex_bcb4_sds

boost_regex_bcb4_sds_clean :
	del bcb4\boost_regex_bcb4_sds\*.obj
	del bcb4\boost_regex_bcb4_sds\*.il?
	del bcb4\boost_regex_bcb4_sds\*.csm
	del bcb4\boost_regex_bcb4_sds\*.tds

bcb4\boost_regex_bcb4_sds.lib : bcb4\boost_regex_bcb4_sds\c_regex_traits.obj bcb4\boost_regex_bcb4_sds\c_regex_traits_common.obj bcb4\boost_regex_bcb4_sds\cpp_regex_traits.obj bcb4\boost_regex_bcb4_sds\cregex.obj bcb4\boost_regex_bcb4_sds\fileiter.obj bcb4\boost_regex_bcb4_sds\instances.obj bcb4\boost_regex_bcb4_sds\posix_api.obj bcb4\boost_regex_bcb4_sds\regex.obj bcb4\boost_regex_bcb4_sds\regex_debug.obj bcb4\boost_regex_bcb4_sds\regex_synch.obj bcb4\boost_regex_bcb4_sds\w32_regex_traits.obj bcb4\boost_regex_bcb4_sds\wide_posix_api.obj bcb4\boost_regex_bcb4_sds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb4\boost_regex_bcb4_sds.lib  -+bcb4\boost_regex_bcb4_sds\c_regex_traits.obj -+bcb4\boost_regex_bcb4_sds\c_regex_traits_common.obj -+bcb4\boost_regex_bcb4_sds\cpp_regex_traits.obj -+bcb4\boost_regex_bcb4_sds\cregex.obj -+bcb4\boost_regex_bcb4_sds\fileiter.obj -+bcb4\boost_regex_bcb4_sds\instances.obj -+bcb4\boost_regex_bcb4_sds\posix_api.obj -+bcb4\boost_regex_bcb4_sds\regex.obj -+bcb4\boost_regex_bcb4_sds\regex_debug.obj -+bcb4\boost_regex_bcb4_sds\regex_synch.obj -+bcb4\boost_regex_bcb4_sds\w32_regex_traits.obj -+bcb4\boost_regex_bcb4_sds\wide_posix_api.obj -+bcb4\boost_regex_bcb4_sds\winstances.obj
|


