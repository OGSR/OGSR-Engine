#
# auto generated makefile for gcc compiler
#
# usage:
# make
#   brings libraries up to date
# make clean
#   deletes temporary object files (but not archives).
#

#
# the following environment variables are recognised:
# CXXFLAGS= extra compiler options - note applies to all build variants
# INCLUDES= additional include directories
# LDFLAGS=  additional linker options
# LIBS=     additional library files

# compiler:
CXX=g++
LINKER=g++ -shared

#
# compiler options for release build:
#
C1=-c -O2 -I../../../ -fPIC
#
# compiler options for debug build:
#
C2=-c -g -I../../../ -fPIC


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp

all : gcc  gcc gcc/boost_regex_shared ./gcc/libboost_regex.so gcc gcc/boost_regex_debug_shared ./gcc/libboost_regex_debug.so

gcc :
	mkdir -p gcc

clean :  boost_regex_clean boost_regex_debug_clean

install : all



########################################################
#
# section for libboost_regex.a
#
########################################################
gcc/boost_regex_shared/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/c_regex_traits.o $(C1) $(CXXFLAGS) ../src/c_regex_traits.cpp

gcc/boost_regex_shared/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/c_regex_traits_common.o $(C1) $(CXXFLAGS) ../src/c_regex_traits_common.cpp

gcc/boost_regex_shared/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/cpp_regex_traits.o $(C1) $(CXXFLAGS) ../src/cpp_regex_traits.cpp

gcc/boost_regex_shared/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/cregex.o $(C1) $(CXXFLAGS) ../src/cregex.cpp

gcc/boost_regex_shared/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/fileiter.o $(C1) $(CXXFLAGS) ../src/fileiter.cpp

gcc/boost_regex_shared/instances.o: ../src/instances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/instances.o $(C1) $(CXXFLAGS) ../src/instances.cpp

gcc/boost_regex_shared/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/posix_api.o $(C1) $(CXXFLAGS) ../src/posix_api.cpp

gcc/boost_regex_shared/regex.o: ../src/regex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/regex.o $(C1) $(CXXFLAGS) ../src/regex.cpp

gcc/boost_regex_shared/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/regex_debug.o $(C1) $(CXXFLAGS) ../src/regex_debug.cpp

gcc/boost_regex_shared/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/regex_synch.o $(C1) $(CXXFLAGS) ../src/regex_synch.cpp

gcc/boost_regex_shared/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/w32_regex_traits.o $(C1) $(CXXFLAGS) ../src/w32_regex_traits.cpp

gcc/boost_regex_shared/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/wide_posix_api.o $(C1) $(CXXFLAGS) ../src/wide_posix_api.cpp

gcc/boost_regex_shared/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_shared/winstances.o $(C1) $(CXXFLAGS) ../src/winstances.cpp

gcc/boost_regex_shared : 
	mkdir -p gcc/boost_regex_shared

boost_regex_clean :
	rm -f gcc/boost_regex_shared/*.o

./gcc/libboost_regex.so : gcc/boost_regex_shared/c_regex_traits.o gcc/boost_regex_shared/c_regex_traits_common.o gcc/boost_regex_shared/cpp_regex_traits.o gcc/boost_regex_shared/cregex.o gcc/boost_regex_shared/fileiter.o gcc/boost_regex_shared/instances.o gcc/boost_regex_shared/posix_api.o gcc/boost_regex_shared/regex.o gcc/boost_regex_shared/regex_debug.o gcc/boost_regex_shared/regex_synch.o gcc/boost_regex_shared/w32_regex_traits.o gcc/boost_regex_shared/wide_posix_api.o gcc/boost_regex_shared/winstances.o
	$(LINKER) -o gcc/libboost_regex.so $(LDFLAGS)  gcc/boost_regex_shared/c_regex_traits.o gcc/boost_regex_shared/c_regex_traits_common.o gcc/boost_regex_shared/cpp_regex_traits.o gcc/boost_regex_shared/cregex.o gcc/boost_regex_shared/fileiter.o gcc/boost_regex_shared/instances.o gcc/boost_regex_shared/posix_api.o gcc/boost_regex_shared/regex.o gcc/boost_regex_shared/regex_debug.o gcc/boost_regex_shared/regex_synch.o gcc/boost_regex_shared/w32_regex_traits.o gcc/boost_regex_shared/wide_posix_api.o gcc/boost_regex_shared/winstances.o $(LIBS)

########################################################
#
# section for libboost_regex_debug.a
#
########################################################
gcc/boost_regex_debug_shared/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/c_regex_traits.o $(C2) $(CXXFLAGS) ../src/c_regex_traits.cpp

gcc/boost_regex_debug_shared/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/c_regex_traits_common.o $(C2) $(CXXFLAGS) ../src/c_regex_traits_common.cpp

gcc/boost_regex_debug_shared/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/cpp_regex_traits.o $(C2) $(CXXFLAGS) ../src/cpp_regex_traits.cpp

gcc/boost_regex_debug_shared/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/cregex.o $(C2) $(CXXFLAGS) ../src/cregex.cpp

gcc/boost_regex_debug_shared/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/fileiter.o $(C2) $(CXXFLAGS) ../src/fileiter.cpp

gcc/boost_regex_debug_shared/instances.o: ../src/instances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/instances.o $(C2) $(CXXFLAGS) ../src/instances.cpp

gcc/boost_regex_debug_shared/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/posix_api.o $(C2) $(CXXFLAGS) ../src/posix_api.cpp

gcc/boost_regex_debug_shared/regex.o: ../src/regex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/regex.o $(C2) $(CXXFLAGS) ../src/regex.cpp

gcc/boost_regex_debug_shared/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/regex_debug.o $(C2) $(CXXFLAGS) ../src/regex_debug.cpp

gcc/boost_regex_debug_shared/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/regex_synch.o $(C2) $(CXXFLAGS) ../src/regex_synch.cpp

gcc/boost_regex_debug_shared/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/w32_regex_traits.o $(C2) $(CXXFLAGS) ../src/w32_regex_traits.cpp

gcc/boost_regex_debug_shared/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/wide_posix_api.o $(C2) $(CXXFLAGS) ../src/wide_posix_api.cpp

gcc/boost_regex_debug_shared/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex_debug_shared/winstances.o $(C2) $(CXXFLAGS) ../src/winstances.cpp

gcc/boost_regex_debug_shared : 
	mkdir -p gcc/boost_regex_debug_shared

boost_regex_debug_clean :
	rm -f gcc/boost_regex_debug_shared/*.o

./gcc/libboost_regex_debug.so : gcc/boost_regex_debug_shared/c_regex_traits.o gcc/boost_regex_debug_shared/c_regex_traits_common.o gcc/boost_regex_debug_shared/cpp_regex_traits.o gcc/boost_regex_debug_shared/cregex.o gcc/boost_regex_debug_shared/fileiter.o gcc/boost_regex_debug_shared/instances.o gcc/boost_regex_debug_shared/posix_api.o gcc/boost_regex_debug_shared/regex.o gcc/boost_regex_debug_shared/regex_debug.o gcc/boost_regex_debug_shared/regex_synch.o gcc/boost_regex_debug_shared/w32_regex_traits.o gcc/boost_regex_debug_shared/wide_posix_api.o gcc/boost_regex_debug_shared/winstances.o
	$(LINKER) -o gcc/libboost_regex_debug.so $(LDFLAGS)  gcc/boost_regex_debug_shared/c_regex_traits.o gcc/boost_regex_debug_shared/c_regex_traits_common.o gcc/boost_regex_debug_shared/cpp_regex_traits.o gcc/boost_regex_debug_shared/cregex.o gcc/boost_regex_debug_shared/fileiter.o gcc/boost_regex_debug_shared/instances.o gcc/boost_regex_debug_shared/posix_api.o gcc/boost_regex_debug_shared/regex.o gcc/boost_regex_debug_shared/regex_debug.o gcc/boost_regex_debug_shared/regex_synch.o gcc/boost_regex_debug_shared/w32_regex_traits.o gcc/boost_regex_debug_shared/wide_posix_api.o gcc/boost_regex_debug_shared/winstances.o $(LIBS)


