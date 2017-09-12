#
# auto generated makefile for Sun Forte 6.1
#
# usage:
# make
#   brings libraries up to date
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
LDFLAGS=
#
# add additional libraries to link to here:
#
LIBS=
#
# lib suffix string:
#
LIBSUFFIX=
#
# template cache path:
#
SUNWS_CACHE_NAME=SunWS_cache



ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp

all : sunpro sunpro/libboost_regex$(LIBSUFFIX) sunpro/libboost_regex$(LIBSUFFIX).a sunpro/libboost_regex_mt$(LIBSUFFIX) sunpro/libboost_regex_mt$(LIBSUFFIX).a sunpro/shared_libboost_regex$(LIBSUFFIX) sunpro/libboost_regex$(LIBSUFFIX).so sunpro/shared_libboost_regex_mt$(LIBSUFFIX) sunpro/libboost_regex_mt$(LIBSUFFIX).so

clean :  libboost_regex$(LIBSUFFIX)_clean libboost_regex_mt$(LIBSUFFIX)_clean libboost_regex$(LIBSUFFIX)_clean_shared libboost_regex_mt$(LIBSUFFIX)_clean_shared

install : all

sunpro :
	mkdir -p sunpro


########################################################
#
# section for libboost_regex$(LIBSUFFIX).a
#
########################################################
sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits.o ../src/c_regex_traits.cpp

sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o ../src/c_regex_traits_common.cpp

sunpro/libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o ../src/cpp_regex_traits.cpp

sunpro/libboost_regex$(LIBSUFFIX)/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/cregex.o ../src/cregex.cpp

sunpro/libboost_regex$(LIBSUFFIX)/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/fileiter.o ../src/fileiter.cpp

sunpro/libboost_regex$(LIBSUFFIX)/instances.o: ../src/instances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/instances.o ../src/instances.cpp

sunpro/libboost_regex$(LIBSUFFIX)/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/posix_api.o ../src/posix_api.cpp

sunpro/libboost_regex$(LIBSUFFIX)/regex.o: ../src/regex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/regex.o ../src/regex.cpp

sunpro/libboost_regex$(LIBSUFFIX)/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/regex_debug.o ../src/regex_debug.cpp

sunpro/libboost_regex$(LIBSUFFIX)/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/regex_synch.o ../src/regex_synch.cpp

sunpro/libboost_regex$(LIBSUFFIX)/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/w32_regex_traits.o ../src/w32_regex_traits.cpp

sunpro/libboost_regex$(LIBSUFFIX)/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/wide_posix_api.o ../src/wide_posix_api.cpp

sunpro/libboost_regex$(LIBSUFFIX)/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX)/winstances.o ../src/winstances.cpp

sunpro/libboost_regex$(LIBSUFFIX) : 
	mkdir -p sunpro/libboost_regex$(LIBSUFFIX)

libboost_regex$(LIBSUFFIX)_clean :
	rm -f sunpro/libboost_regex$(LIBSUFFIX)/*.o
	rm -fr sunpro/libboost_regex$(LIBSUFFIX)/$(SUNWS_CACHE_NAME)

sunpro/libboost_regex$(LIBSUFFIX).a : sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o sunpro/libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/cregex.o sunpro/libboost_regex$(LIBSUFFIX)/fileiter.o sunpro/libboost_regex$(LIBSUFFIX)/instances.o sunpro/libboost_regex$(LIBSUFFIX)/posix_api.o sunpro/libboost_regex$(LIBSUFFIX)/regex.o sunpro/libboost_regex$(LIBSUFFIX)/regex_debug.o sunpro/libboost_regex$(LIBSUFFIX)/regex_synch.o sunpro/libboost_regex$(LIBSUFFIX)/w32_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/wide_posix_api.o sunpro/libboost_regex$(LIBSUFFIX)/winstances.o
	CC -xar $(CXXFLAGS) $(LDFLAGS) -o sunpro/libboost_regex$(LIBSUFFIX).a  sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o sunpro/libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/cregex.o sunpro/libboost_regex$(LIBSUFFIX)/fileiter.o sunpro/libboost_regex$(LIBSUFFIX)/instances.o sunpro/libboost_regex$(LIBSUFFIX)/posix_api.o sunpro/libboost_regex$(LIBSUFFIX)/regex.o sunpro/libboost_regex$(LIBSUFFIX)/regex_debug.o sunpro/libboost_regex$(LIBSUFFIX)/regex_synch.o sunpro/libboost_regex$(LIBSUFFIX)/w32_regex_traits.o sunpro/libboost_regex$(LIBSUFFIX)/wide_posix_api.o sunpro/libboost_regex$(LIBSUFFIX)/winstances.o

########################################################
#
# section for libboost_regex_mt$(LIBSUFFIX).a
#
########################################################
sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o ../src/c_regex_traits.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o ../src/c_regex_traits_common.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o ../src/cpp_regex_traits.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/cregex.o ../src/cregex.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/fileiter.o ../src/fileiter.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/instances.o: ../src/instances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/instances.o ../src/instances.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/posix_api.o ../src/posix_api.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/regex.o: ../src/regex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex.o ../src/regex.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_debug.o ../src/regex_debug.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_synch.o ../src/regex_synch.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o ../src/w32_regex_traits.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o ../src/wide_posix_api.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX)/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX)/winstances.o ../src/winstances.cpp

sunpro/libboost_regex_mt$(LIBSUFFIX) : 
	mkdir -p sunpro/libboost_regex_mt$(LIBSUFFIX)

libboost_regex_mt$(LIBSUFFIX)_clean :
	rm -f sunpro/libboost_regex_mt$(LIBSUFFIX)/*.o
	rm -fr sunpro/libboost_regex_mt$(LIBSUFFIX)/$(SUNWS_CACHE_NAME)

sunpro/libboost_regex_mt$(LIBSUFFIX).a : sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o sunpro/libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/cregex.o sunpro/libboost_regex_mt$(LIBSUFFIX)/fileiter.o sunpro/libboost_regex_mt$(LIBSUFFIX)/instances.o sunpro/libboost_regex_mt$(LIBSUFFIX)/posix_api.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_debug.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_synch.o sunpro/libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o sunpro/libboost_regex_mt$(LIBSUFFIX)/winstances.o
	CC -xar $(CXXFLAGS) $(LDFLAGS) -o sunpro/libboost_regex_mt$(LIBSUFFIX).a  sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o sunpro/libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/cregex.o sunpro/libboost_regex_mt$(LIBSUFFIX)/fileiter.o sunpro/libboost_regex_mt$(LIBSUFFIX)/instances.o sunpro/libboost_regex_mt$(LIBSUFFIX)/posix_api.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_debug.o sunpro/libboost_regex_mt$(LIBSUFFIX)/regex_synch.o sunpro/libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o sunpro/libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o sunpro/libboost_regex_mt$(LIBSUFFIX)/winstances.o

########################################################
#
# section for libboost_regex$(LIBSUFFIX).so
#
########################################################
sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits.o ../src/c_regex_traits.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o ../src/c_regex_traits_common.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o ../src/cpp_regex_traits.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/cregex.o ../src/cregex.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/fileiter.o ../src/fileiter.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/instances.o: ../src/instances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/instances.o ../src/instances.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/posix_api.o ../src/posix_api.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/regex.o: ../src/regex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex.o ../src/regex.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_debug.o ../src/regex_debug.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_synch.o ../src/regex_synch.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/w32_regex_traits.o ../src/w32_regex_traits.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/wide_posix_api.o ../src/wide_posix_api.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX)/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex$(LIBSUFFIX)/winstances.o ../src/winstances.cpp

sunpro/shared_libboost_regex$(LIBSUFFIX) :
	mkdir -p sunpro/shared_libboost_regex$(LIBSUFFIX)

libboost_regex$(LIBSUFFIX)_clean_shared :
	rm -f sunpro/shared_libboost_regex$(LIBSUFFIX)/*.o
	rm -fr sunpro/shared_libboost_regex$(LIBSUFFIX)/$(SUNWS_CACHE_NAME)

sunpro/libboost_regex$(LIBSUFFIX).so : sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o sunpro/shared_libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/cregex.o sunpro/shared_libboost_regex$(LIBSUFFIX)/fileiter.o sunpro/shared_libboost_regex$(LIBSUFFIX)/instances.o sunpro/shared_libboost_regex$(LIBSUFFIX)/posix_api.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_debug.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_synch.o sunpro/shared_libboost_regex$(LIBSUFFIX)/w32_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/wide_posix_api.o sunpro/shared_libboost_regex$(LIBSUFFIX)/winstances.o
	CC -KPIC -O2 -I../../../ -G -o sunpro/libboost_regex$(LIBSUFFIX).so $(LDFLAGS)  sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/c_regex_traits_common.o sunpro/shared_libboost_regex$(LIBSUFFIX)/cpp_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/cregex.o sunpro/shared_libboost_regex$(LIBSUFFIX)/fileiter.o sunpro/shared_libboost_regex$(LIBSUFFIX)/instances.o sunpro/shared_libboost_regex$(LIBSUFFIX)/posix_api.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_debug.o sunpro/shared_libboost_regex$(LIBSUFFIX)/regex_synch.o sunpro/shared_libboost_regex$(LIBSUFFIX)/w32_regex_traits.o sunpro/shared_libboost_regex$(LIBSUFFIX)/wide_posix_api.o sunpro/shared_libboost_regex$(LIBSUFFIX)/winstances.o $(LIBS)

########################################################
#
# section for libboost_regex_mt$(LIBSUFFIX).so
#
########################################################
sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o ../src/c_regex_traits.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o ../src/c_regex_traits_common.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o ../src/cpp_regex_traits.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cregex.o ../src/cregex.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/fileiter.o ../src/fileiter.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/instances.o: ../src/instances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/instances.o ../src/instances.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/posix_api.o ../src/posix_api.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex.o: ../src/regex.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex.o ../src/regex.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_debug.o ../src/regex_debug.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_synch.o ../src/regex_synch.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o ../src/w32_regex_traits.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o ../src/wide_posix_api.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	CC -c $(INCLUDES) -KPIC -O2 -mt -I../../../ $(CXXFLAGS) -o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/winstances.o ../src/winstances.cpp

sunpro/shared_libboost_regex_mt$(LIBSUFFIX) :
	mkdir -p sunpro/shared_libboost_regex_mt$(LIBSUFFIX)

libboost_regex_mt$(LIBSUFFIX)_clean_shared :
	rm -f sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/*.o
	rm -fr sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/$(SUNWS_CACHE_NAME)

sunpro/libboost_regex_mt$(LIBSUFFIX).so : sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cregex.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/fileiter.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/instances.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/posix_api.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_debug.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_synch.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/winstances.o
	CC -KPIC -O2 -mt -I../../../ -G -o sunpro/libboost_regex_mt$(LIBSUFFIX).so $(LDFLAGS)  sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/c_regex_traits_common.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cpp_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/cregex.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/fileiter.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/instances.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/posix_api.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_debug.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/regex_synch.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/w32_regex_traits.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/wide_posix_api.o sunpro/shared_libboost_regex_mt$(LIBSUFFIX)/winstances.o $(LIBS)


