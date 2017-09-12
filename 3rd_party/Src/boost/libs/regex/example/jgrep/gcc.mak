
# very basic makefile for jgrep
#
# GNU compiler g++
#
CXX= $(INCLUDES) -O2 -I../../../../ -I./ $(CXXFLAGS) $(LDFLAGS)

jgrep : jgrep.cpp main.cpp
	g++ -ojgrep $(CXX) jgrep.cpp main.cpp -L../../build/gcc -lboost_regex $(LIBS)

debug : jgrep.cpp main.cpp
	g++ -ojgrep -I../../../../ -I./ -g jgrep.cpp main.cpp -L../../build/gcc -lboost_regex_debug $(LIBS)




















