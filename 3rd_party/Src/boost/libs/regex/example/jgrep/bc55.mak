
# very basic makefile for jgrep.exe
#
# Borland C++ tools
#
# BCROOT defines the root directory of your bc builder install
#
!ifndef BCROOT
BCROOT=$(MAKEDIR)\..
!endif

BCC32   = $(BCROOT)\bin\Bcc32.exe
LIBPATH=..\..\build\bcb5

IDE_LinkFLAGS32 =  -L$(BCROOT)\LIB
LINKOPTS= -ap -Tpe -x
COMPOPTS= -O2 -tWM- -Vx -Ve -DSTRICT; -I$(BCROOT)\include;../../../../; -D_NO_VCL



jgrep.exe : jgrep.obj main.obj
  $(BCC32) $(COMPOPTS) -e$@  -L$(LIBPATH) main.obj jgrep.obj

jgrep.obj :  jgrep.cpp
  $(BCC32) -c @&&|
 $(COMPOPTS) -o$@ jgrep.cpp
|

main.obj :  main.cpp
  $(BCC32) -c @&&|
 $(COMPOPTS) -o$@ main.cpp
|



















