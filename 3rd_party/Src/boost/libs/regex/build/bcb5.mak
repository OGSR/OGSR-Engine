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

all : bcb5 bcb5\boost_regex_bcb5_sss bcb5\boost_regex_bcb5_sss.lib bcb5\boost_regex_bcb5_mss bcb5\boost_regex_bcb5_mss.lib bcb5\boost_regex_bcb5_mdi bcb5\boost_regex_bcb5_mdi.lib bcb5\boost_regex_bcb5_sdi bcb5\boost_regex_bcb5_sdi.lib bcb5\boost_regex_bcb5_mds bcb5\boost_regex_bcb5_mds.lib bcb5\boost_regex_bcb5_sds bcb5\boost_regex_bcb5_sds.lib

clean :  boost_regex_bcb5_sss_clean boost_regex_bcb5_mss_clean boost_regex_bcb5_mdi_clean boost_regex_bcb5_sdi_clean boost_regex_bcb5_mds_clean boost_regex_bcb5_sds_clean

install : all
	copy bcb5\boost_regex_bcb5_sss.lib $(BCROOT)\lib
	copy bcb5\boost_regex_bcb5_mss.lib $(BCROOT)\lib
	copy bcb5\boost_regex_bcb5_mdi.lib $(BCROOT)\lib
	copy bcb5\boost_regex_bcb5_mdi.dll $(BCROOT)\bin
	copy bcb5\boost_regex_bcb5_mdi.tds $(BCROOT)\bin
	copy bcb5\boost_regex_bcb5_sdi.lib $(BCROOT)\lib
	copy bcb5\boost_regex_bcb5_sdi.dll $(BCROOT)\bin
	copy bcb5\boost_regex_bcb5_sdi.tds $(BCROOT)\bin
	copy bcb5\boost_regex_bcb5_mds.lib $(BCROOT)\lib
	copy bcb5\boost_regex_bcb5_sds.lib $(BCROOT)\lib

bcb5 :
	-@mkdir bcb5


########################################################
#
# section for boost_regex_bcb5_sss.lib
#
########################################################
bcb5\boost_regex_bcb5_sss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_sss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_sss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_sss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_sss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_sss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_sss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_sss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_sss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_sss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM- -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8037 -w-8057 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_sss\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_sss : 
	-@mkdir bcb5\boost_regex_bcb5_sss

boost_regex_bcb5_sss_clean :
	del bcb5\boost_regex_bcb5_sss\*.obj
	del bcb5\boost_regex_bcb5_sss\*.il?
	del bcb5\boost_regex_bcb5_sss\*.csm
	del bcb5\boost_regex_bcb5_sss\*.tds

bcb5\boost_regex_bcb5_sss.lib : bcb5\boost_regex_bcb5_sss\c_regex_traits.obj bcb5\boost_regex_bcb5_sss\c_regex_traits_common.obj bcb5\boost_regex_bcb5_sss\cpp_regex_traits.obj bcb5\boost_regex_bcb5_sss\cregex.obj bcb5\boost_regex_bcb5_sss\fileiter.obj bcb5\boost_regex_bcb5_sss\instances.obj bcb5\boost_regex_bcb5_sss\posix_api.obj bcb5\boost_regex_bcb5_sss\regex.obj bcb5\boost_regex_bcb5_sss\regex_debug.obj bcb5\boost_regex_bcb5_sss\regex_synch.obj bcb5\boost_regex_bcb5_sss\w32_regex_traits.obj bcb5\boost_regex_bcb5_sss\wide_posix_api.obj bcb5\boost_regex_bcb5_sss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb5\boost_regex_bcb5_sss.lib  -+bcb5\boost_regex_bcb5_sss\c_regex_traits.obj -+bcb5\boost_regex_bcb5_sss\c_regex_traits_common.obj -+bcb5\boost_regex_bcb5_sss\cpp_regex_traits.obj -+bcb5\boost_regex_bcb5_sss\cregex.obj -+bcb5\boost_regex_bcb5_sss\fileiter.obj -+bcb5\boost_regex_bcb5_sss\instances.obj -+bcb5\boost_regex_bcb5_sss\posix_api.obj -+bcb5\boost_regex_bcb5_sss\regex.obj -+bcb5\boost_regex_bcb5_sss\regex_debug.obj -+bcb5\boost_regex_bcb5_sss\regex_synch.obj -+bcb5\boost_regex_bcb5_sss\w32_regex_traits.obj -+bcb5\boost_regex_bcb5_sss\wide_posix_api.obj -+bcb5\boost_regex_bcb5_sss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb5_mss.lib
#
########################################################
bcb5\boost_regex_bcb5_mss\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mss\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_mss\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mss\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_mss\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_mss\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_mss\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_mss\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_mss\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_mss\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_mss\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mss\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_mss\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWM -D_NO_VCL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ $(CXXFLAGS) -obcb5\boost_regex_bcb5_mss\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_mss : 
	-@mkdir bcb5\boost_regex_bcb5_mss

boost_regex_bcb5_mss_clean :
	del bcb5\boost_regex_bcb5_mss\*.obj
	del bcb5\boost_regex_bcb5_mss\*.il?
	del bcb5\boost_regex_bcb5_mss\*.csm
	del bcb5\boost_regex_bcb5_mss\*.tds

bcb5\boost_regex_bcb5_mss.lib : bcb5\boost_regex_bcb5_mss\c_regex_traits.obj bcb5\boost_regex_bcb5_mss\c_regex_traits_common.obj bcb5\boost_regex_bcb5_mss\cpp_regex_traits.obj bcb5\boost_regex_bcb5_mss\cregex.obj bcb5\boost_regex_bcb5_mss\fileiter.obj bcb5\boost_regex_bcb5_mss\instances.obj bcb5\boost_regex_bcb5_mss\posix_api.obj bcb5\boost_regex_bcb5_mss\regex.obj bcb5\boost_regex_bcb5_mss\regex_debug.obj bcb5\boost_regex_bcb5_mss\regex_synch.obj bcb5\boost_regex_bcb5_mss\w32_regex_traits.obj bcb5\boost_regex_bcb5_mss\wide_posix_api.obj bcb5\boost_regex_bcb5_mss\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb5\boost_regex_bcb5_mss.lib  -+bcb5\boost_regex_bcb5_mss\c_regex_traits.obj -+bcb5\boost_regex_bcb5_mss\c_regex_traits_common.obj -+bcb5\boost_regex_bcb5_mss\cpp_regex_traits.obj -+bcb5\boost_regex_bcb5_mss\cregex.obj -+bcb5\boost_regex_bcb5_mss\fileiter.obj -+bcb5\boost_regex_bcb5_mss\instances.obj -+bcb5\boost_regex_bcb5_mss\posix_api.obj -+bcb5\boost_regex_bcb5_mss\regex.obj -+bcb5\boost_regex_bcb5_mss\regex_debug.obj -+bcb5\boost_regex_bcb5_mss\regex_synch.obj -+bcb5\boost_regex_bcb5_mss\w32_regex_traits.obj -+bcb5\boost_regex_bcb5_mss\wide_posix_api.obj -+bcb5\boost_regex_bcb5_mss\winstances.obj
|

########################################################
#
# section for boost_regex_bcb5_mdi.lib
#
########################################################
bcb5\boost_regex_bcb5_mdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_mdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_mdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_mdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_mdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_mdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_mdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_mdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_mdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_mdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mdi\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_mdi :
	-@mkdir bcb5\boost_regex_bcb5_mdi

boost_regex_bcb5_mdi_clean :
	del bcb5\boost_regex_bcb5_mdi\*.obj
	del bcb5\boost_regex_bcb5_mdi\*.il?
	del bcb5\boost_regex_bcb5_mdi\*.csm
	del bcb5\boost_regex_bcb5_mdi\*.tds
	del bcb5\*.tds

bcb5\boost_regex_bcb5_mdi.lib : bcb5\boost_regex_bcb5_mdi\c_regex_traits.obj bcb5\boost_regex_bcb5_mdi\c_regex_traits_common.obj bcb5\boost_regex_bcb5_mdi\cpp_regex_traits.obj bcb5\boost_regex_bcb5_mdi\cregex.obj bcb5\boost_regex_bcb5_mdi\fileiter.obj bcb5\boost_regex_bcb5_mdi\instances.obj bcb5\boost_regex_bcb5_mdi\posix_api.obj bcb5\boost_regex_bcb5_mdi\regex.obj bcb5\boost_regex_bcb5_mdi\regex_debug.obj bcb5\boost_regex_bcb5_mdi\regex_synch.obj bcb5\boost_regex_bcb5_mdi\w32_regex_traits.obj bcb5\boost_regex_bcb5_mdi\wide_posix_api.obj bcb5\boost_regex_bcb5_mdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWM -tWR -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb5\boost_regex_bcb5_mdi.dll $(XLFLAGS)  bcb5\boost_regex_bcb5_mdi\c_regex_traits.obj bcb5\boost_regex_bcb5_mdi\c_regex_traits_common.obj bcb5\boost_regex_bcb5_mdi\cpp_regex_traits.obj bcb5\boost_regex_bcb5_mdi\cregex.obj bcb5\boost_regex_bcb5_mdi\fileiter.obj bcb5\boost_regex_bcb5_mdi\instances.obj bcb5\boost_regex_bcb5_mdi\posix_api.obj bcb5\boost_regex_bcb5_mdi\regex.obj bcb5\boost_regex_bcb5_mdi\regex_debug.obj bcb5\boost_regex_bcb5_mdi\regex_synch.obj bcb5\boost_regex_bcb5_mdi\w32_regex_traits.obj bcb5\boost_regex_bcb5_mdi\wide_posix_api.obj bcb5\boost_regex_bcb5_mdi\winstances.obj $(LIBS)
|
	implib -w bcb5\boost_regex_bcb5_mdi.lib bcb5\boost_regex_bcb5_mdi.dll

########################################################
#
# section for boost_regex_bcb5_sdi.lib
#
########################################################
bcb5\boost_regex_bcb5_sdi\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sdi\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_sdi\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sdi\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_sdi\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_sdi\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_sdi\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_sdi\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_sdi\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_sdi\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_sdi\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sdi\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_sdi\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sdi\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_sdi :
	-@mkdir bcb5\boost_regex_bcb5_sdi

boost_regex_bcb5_sdi_clean :
	del bcb5\boost_regex_bcb5_sdi\*.obj
	del bcb5\boost_regex_bcb5_sdi\*.il?
	del bcb5\boost_regex_bcb5_sdi\*.csm
	del bcb5\boost_regex_bcb5_sdi\*.tds
	del bcb5\*.tds

bcb5\boost_regex_bcb5_sdi.lib : bcb5\boost_regex_bcb5_sdi\c_regex_traits.obj bcb5\boost_regex_bcb5_sdi\c_regex_traits_common.obj bcb5\boost_regex_bcb5_sdi\cpp_regex_traits.obj bcb5\boost_regex_bcb5_sdi\cregex.obj bcb5\boost_regex_bcb5_sdi\fileiter.obj bcb5\boost_regex_bcb5_sdi\instances.obj bcb5\boost_regex_bcb5_sdi\posix_api.obj bcb5\boost_regex_bcb5_sdi\regex.obj bcb5\boost_regex_bcb5_sdi\regex_debug.obj bcb5\boost_regex_bcb5_sdi\regex_synch.obj bcb5\boost_regex_bcb5_sdi\w32_regex_traits.obj bcb5\boost_regex_bcb5_sdi\wide_posix_api.obj bcb5\boost_regex_bcb5_sdi\winstances.obj
	bcc32 @&&|
-lw-dup -lw-dpl -tWD -tWR -tWM- -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; -ebcb5\boost_regex_bcb5_sdi.dll $(XLFLAGS)  bcb5\boost_regex_bcb5_sdi\c_regex_traits.obj bcb5\boost_regex_bcb5_sdi\c_regex_traits_common.obj bcb5\boost_regex_bcb5_sdi\cpp_regex_traits.obj bcb5\boost_regex_bcb5_sdi\cregex.obj bcb5\boost_regex_bcb5_sdi\fileiter.obj bcb5\boost_regex_bcb5_sdi\instances.obj bcb5\boost_regex_bcb5_sdi\posix_api.obj bcb5\boost_regex_bcb5_sdi\regex.obj bcb5\boost_regex_bcb5_sdi\regex_debug.obj bcb5\boost_regex_bcb5_sdi\regex_synch.obj bcb5\boost_regex_bcb5_sdi\w32_regex_traits.obj bcb5\boost_regex_bcb5_sdi\wide_posix_api.obj bcb5\boost_regex_bcb5_sdi\winstances.obj $(LIBS)
|
	implib -w bcb5\boost_regex_bcb5_sdi.lib bcb5\boost_regex_bcb5_sdi.dll

########################################################
#
# section for boost_regex_bcb5_mds.lib
#
########################################################
bcb5\boost_regex_bcb5_mds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_mds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_mds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_mds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_mds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_mds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_mds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_mds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_mds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_mds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_mds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWM -tWR -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_mds\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_mds : 
	-@mkdir bcb5\boost_regex_bcb5_mds

boost_regex_bcb5_mds_clean :
	del bcb5\boost_regex_bcb5_mds\*.obj
	del bcb5\boost_regex_bcb5_mds\*.il?
	del bcb5\boost_regex_bcb5_mds\*.csm
	del bcb5\boost_regex_bcb5_mds\*.tds

bcb5\boost_regex_bcb5_mds.lib : bcb5\boost_regex_bcb5_mds\c_regex_traits.obj bcb5\boost_regex_bcb5_mds\c_regex_traits_common.obj bcb5\boost_regex_bcb5_mds\cpp_regex_traits.obj bcb5\boost_regex_bcb5_mds\cregex.obj bcb5\boost_regex_bcb5_mds\fileiter.obj bcb5\boost_regex_bcb5_mds\instances.obj bcb5\boost_regex_bcb5_mds\posix_api.obj bcb5\boost_regex_bcb5_mds\regex.obj bcb5\boost_regex_bcb5_mds\regex_debug.obj bcb5\boost_regex_bcb5_mds\regex_synch.obj bcb5\boost_regex_bcb5_mds\w32_regex_traits.obj bcb5\boost_regex_bcb5_mds\wide_posix_api.obj bcb5\boost_regex_bcb5_mds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb5\boost_regex_bcb5_mds.lib  -+bcb5\boost_regex_bcb5_mds\c_regex_traits.obj -+bcb5\boost_regex_bcb5_mds\c_regex_traits_common.obj -+bcb5\boost_regex_bcb5_mds\cpp_regex_traits.obj -+bcb5\boost_regex_bcb5_mds\cregex.obj -+bcb5\boost_regex_bcb5_mds\fileiter.obj -+bcb5\boost_regex_bcb5_mds\instances.obj -+bcb5\boost_regex_bcb5_mds\posix_api.obj -+bcb5\boost_regex_bcb5_mds\regex.obj -+bcb5\boost_regex_bcb5_mds\regex_debug.obj -+bcb5\boost_regex_bcb5_mds\regex_synch.obj -+bcb5\boost_regex_bcb5_mds\w32_regex_traits.obj -+bcb5\boost_regex_bcb5_mds\wide_posix_api.obj -+bcb5\boost_regex_bcb5_mds\winstances.obj
|

########################################################
#
# section for boost_regex_bcb5_sds.lib
#
########################################################
bcb5\boost_regex_bcb5_sds\c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\c_regex_traits.obj ../src/c_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sds\c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\c_regex_traits_common.obj ../src/c_regex_traits_common.cpp
|

bcb5\boost_regex_bcb5_sds\cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\cpp_regex_traits.obj ../src/cpp_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sds\cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\cregex.obj ../src/cregex.cpp
|

bcb5\boost_regex_bcb5_sds\fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\fileiter.obj ../src/fileiter.cpp
|

bcb5\boost_regex_bcb5_sds\instances.obj: ../src/instances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\instances.obj ../src/instances.cpp
|

bcb5\boost_regex_bcb5_sds\posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\posix_api.obj ../src/posix_api.cpp
|

bcb5\boost_regex_bcb5_sds\regex.obj: ../src/regex.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\regex.obj ../src/regex.cpp
|

bcb5\boost_regex_bcb5_sds\regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\regex_debug.obj ../src/regex_debug.cpp
|

bcb5\boost_regex_bcb5_sds\regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\regex_synch.obj ../src/regex_synch.cpp
|

bcb5\boost_regex_bcb5_sds\w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\w32_regex_traits.obj ../src/w32_regex_traits.cpp
|

bcb5\boost_regex_bcb5_sds\wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\wide_posix_api.obj ../src/wide_posix_api.cpp
|

bcb5\boost_regex_bcb5_sds\winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	bcc32 @&&|
-c $(INCLUDES) -tWD -tWR -tWM- -DBOOST_REGEX_STATIC_LINK -D_NO_VCL -D_RTLDLL -O2 -Ve -Vx -w-inl -w-aus -w-rch -w-8012 -w-8057 -w-8037 -DSTRICT; -I$(BCROOT)\include;../../../ -L$(BCROOT)\lib;$(BCROOT)\lib\release; $(CXXFLAGS) -obcb5\boost_regex_bcb5_sds\winstances.obj ../src/winstances.cpp
|

bcb5\boost_regex_bcb5_sds : 
	-@mkdir bcb5\boost_regex_bcb5_sds

boost_regex_bcb5_sds_clean :
	del bcb5\boost_regex_bcb5_sds\*.obj
	del bcb5\boost_regex_bcb5_sds\*.il?
	del bcb5\boost_regex_bcb5_sds\*.csm
	del bcb5\boost_regex_bcb5_sds\*.tds

bcb5\boost_regex_bcb5_sds.lib : bcb5\boost_regex_bcb5_sds\c_regex_traits.obj bcb5\boost_regex_bcb5_sds\c_regex_traits_common.obj bcb5\boost_regex_bcb5_sds\cpp_regex_traits.obj bcb5\boost_regex_bcb5_sds\cregex.obj bcb5\boost_regex_bcb5_sds\fileiter.obj bcb5\boost_regex_bcb5_sds\instances.obj bcb5\boost_regex_bcb5_sds\posix_api.obj bcb5\boost_regex_bcb5_sds\regex.obj bcb5\boost_regex_bcb5_sds\regex_debug.obj bcb5\boost_regex_bcb5_sds\regex_synch.obj bcb5\boost_regex_bcb5_sds\w32_regex_traits.obj bcb5\boost_regex_bcb5_sds\wide_posix_api.obj bcb5\boost_regex_bcb5_sds\winstances.obj
	tlib @&&|
/P128 /C /u /a $(XSFLAGS) bcb5\boost_regex_bcb5_sds.lib  -+bcb5\boost_regex_bcb5_sds\c_regex_traits.obj -+bcb5\boost_regex_bcb5_sds\c_regex_traits_common.obj -+bcb5\boost_regex_bcb5_sds\cpp_regex_traits.obj -+bcb5\boost_regex_bcb5_sds\cregex.obj -+bcb5\boost_regex_bcb5_sds\fileiter.obj -+bcb5\boost_regex_bcb5_sds\instances.obj -+bcb5\boost_regex_bcb5_sds\posix_api.obj -+bcb5\boost_regex_bcb5_sds\regex.obj -+bcb5\boost_regex_bcb5_sds\regex_debug.obj -+bcb5\boost_regex_bcb5_sds\regex_synch.obj -+bcb5\boost_regex_bcb5_sds\w32_regex_traits.obj -+bcb5\boost_regex_bcb5_sds\wide_posix_api.obj -+bcb5\boost_regex_bcb5_sds\winstances.obj
|


