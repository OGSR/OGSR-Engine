
#
# very simple makefile for Visual C++ 6
#

jgrep.exe: main.cpp jgrep.cpp jgrep.h
	cl -GX -GR /Oityb1 /GF /Gy -I..\..\..\..\ jgrep.cpp main.cpp /link /LIBPATH:..\..\build\vc6











