
# very basic makefile for regress
#
# Borland C++ tools
#
# BCROOT defines the root directory of your bcb install
#
!ifndef BCROOT
BCROOT=$(MAKEDIR)\..
!endif
#
# sources to compile for each test:
#
SOURCES=tests.cpp parse.cpp regress.cpp ../../../test/src/cpp_main.cpp ../../../test/src/execution_monitor.cpp

BCC32   = $(BCROOT)\bin\Bcc32.exe 
TLINK32 = $(BCROOT)\bin\ILink32.exe

IDE_LinkFLAGS32 =  -L$(BCROOT)\LIB
LINKOPTS= -ap -Tpe -x
CFLAGS= -O2 -tWC -DSTRICT; -Vx -Ve -w-inl -w-aus -w-csu -w-eff -w-rch -I$(BCROOT)\include;..\..\..\..\; -L..\..\build\bcb5

all :: r1.exe r2.exe r3.exe r4.exe r5.exe r6.exe r1m.exe r2m.exe r3m.exe r4m.exe r5m.exe r6m.exe r1l.exe r2l.exe r3l.exe r4l.exe r5l.exe r6l.exe r1lm.exe r2lm.exe r3lm.exe r4lm.exe r5lm.exe r6lm.exe
	echo testing static single threaded version....
	r1 tests.txt test1252.txt
	r2 tests.txt
	r3 tests.txt
	r4 tests.txt test1252.txt
	r5 tests.txt
	r6 tests.txt
	echo testing static multi-threaded version....
	r1m tests.txt test1252.txt
	r2m tests.txt
	r3m tests.txt
	r4m tests.txt test1252.txt
	r5m tests.txt
	r6m tests.txt
	echo testing dll single threaded version....
	r1l tests.txt test1252.txt
	r2l tests.txt
	r3l tests.txt
	r4l tests.txt test1252.txt
	r5l tests.txt
	r6l tests.txt
	echo testing dll multi-threaded version....
	r1lm tests.txt test1252.txt
	r2lm tests.txt
	r3lm tests.txt
	r4lm tests.txt test1252.txt
	r5lm tests.txt
	r6lm tests.txt


r1.exe : tests.cpp regress.cpp parse.cpp
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er1.exe -DBOOST_RE_TEST_LOCALE_W32 tests.cpp regress.cpp parse.cpp

r2.exe : $(SOURCES)
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er2.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES)

r3.exe : $(SOURCES)
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er3.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES)

r4.exe : $(SOURCES)
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er4.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES)

r5.exe : $(SOURCES)
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er5.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES)

r6.exe : $(SOURCES)
	$(BCC32) -tWM- -D_NO_VCL $(CFLAGS) -er6.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES)
	

r1m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er1m.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES)

r2m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er2m.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES)

r3m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er3m.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES)

r4m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er4m.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES)

r5m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er5m.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES)

r6m.exe : $(SOURCES)
	$(BCC32) -tWM -D_NO_VCL $(CFLAGS) -er6m.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES)


r1l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er1l.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES)

r2l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er2l.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES)

r3l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er3l.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES)

r4l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er4l.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES)

r5l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er5l.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES)

r6l.exe : $(SOURCES)
	$(BCC32) -tWM- -tWR -D_NO_VCL $(CFLAGS) -er6l.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES)


r1lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er1lm.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES)

r2lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er2lm.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES)

r3lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er3lm.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES)

r4lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er4lm.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES)

r5lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er5lm.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES)

r6lm.exe : $(SOURCES)
	$(BCC32) -tWM -tWR -D_NO_VCL $(CFLAGS) -er6lm.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES)















