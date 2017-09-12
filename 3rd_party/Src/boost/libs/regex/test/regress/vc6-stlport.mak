
# very basic makefile for regression tests
#
# Visual C++ 6 + full stlport 4.x
#
# we don't test single threaded builds as stlport doesn't support these...
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
# sources to compile for each test:
#
SOURCES=tests.cpp parse.cpp regress.cpp ../../../test/src/cpp_main.cpp ../../../test/src/execution_monitor.cpp

!IF "$(MSVCDIR)" == ""
!ERROR Variable MSVCDIR not set.
!ENDIF

!IF "$(STLPORT_PATH)" == ""
!ERROR Variable STLPORT_PATH not set.
!ENDIF


CFLAGS= $(INCLUDES) /I$(STLPORT_PATH)\stlport /O2 /GF /Gy -GX -GR -I..\..\..\..\ $(CXXFLAGS)

LFLAGS= -link /LIBPATH:..\..\build\vc6-stlport /LIBPATH:$(STLPORT_PATH)\lib user32.lib $(XLFLAGS)

all : $(STLPORT_PATH)\stlport\string r1m.exe r2m.exe r4m.exe r5m.exe r1l.exe r2l.exe r4l.exe r5l.exe r1ls.exe r2ls.exe r4ls.exe r5ls.exe r1md.exe r2md.exe r4md.exe r5md.exe r1lmd.exe r2lmd.exe r4lmd.exe r5lmd.exe r1mdd.exe r2mdd.exe r4mdd.exe r5mdd.exe r1lmdd.exe r2lmdd.exe
	echo testing static multi-threaded version....
	r1m tests.txt test1252.txt
	r2m tests.txt
	r4m tests.txt test1252.txt
	r5m tests.txt
	echo testing DLL version....
	r1l tests.txt test1252.txt
	r2l tests.txt
	r4l tests.txt test1252.txt
	r5l tests.txt
	echo testing static version with dll runtime....
	r1ls tests.txt test1252.txt
	r2ls tests.txt
	r4ls tests.txt test1252.txt
	r5ls tests.txt
	echo testing static multi-threaded debug version....
	r1md tests.txt test1252.txt
	r2md tests.txt
	r4md tests.txt test1252.txt
	r5md tests.txt
	echo testing dll debug version....
	r1lmd tests.txt test1252.txt
	r2lmd tests.txt
	r4lmd tests.txt test1252.txt
	r5lmd tests.txt
	echo testing static multi-threaded stl-debug version....
	r1mdd tests.txt test1252.txt
	r2mdd tests.txt
	r4mdd tests.txt test1252.txt
	r5mdd tests.txt
	echo testing dll stl-debug version....
	r1lmdd tests.txt test1252.txt
	r2lmdd tests.txt


r1m.exe : $(SOURCES)
	cl /MT /D_MT -o r1m.exe $(CFLAGS) -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2m.exe : $(SOURCES)
	cl /MT /D_MT $(CFLAGS) -o r2m.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3m.exe : $(SOURCES)
	cl /MT /D_MT $(CFLAGS) -o r3m.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4m.exe : $(SOURCES)
	cl /MT /D_MT $(CFLAGS) -o r4m.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5m.exe : $(SOURCES)
	cl /MT /D_MT $(CFLAGS) -o r5m.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6m.exe : $(SOURCES)
	cl /MT /D_MT $(CFLAGS) -o r6m.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)


r1l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r1l.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r2l.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r3l.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r4l.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r5l.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6l.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r6l.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r1ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r1ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r2ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r3ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r4ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r5ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6ls.exe : $(SOURCES)
	cl /MD /D_MT /D_DLL $(CFLAGS) -o r6ls.exe -DBOOST_RE_STATIC_LIB -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)


r1md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r1md.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r2md.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r3md.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r4md.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r5md.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6md.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r6md.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)


r1lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r1lmd.exe -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r2lmd.exe -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r3lmd.exe -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r4lmd.exe -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r5lmd.exe -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6lmd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r6lmd.exe -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r1lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r1lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r2lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r3lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r4lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r5lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6lmdd.exe : $(SOURCES)
	cl /MDd /D_MT /D_DLL $(CFLAGS) -o r6lmdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r1mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r1mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_W32 $(SOURCES) $(LFLAGS)

r2mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r2mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_C $(SOURCES) $(LFLAGS)

r3mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r3mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_CPP $(SOURCES) $(LFLAGS)

r4mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r4mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_W32 -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r5mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r5mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_C -DTEST_UNICODE $(SOURCES) $(LFLAGS)

r6mdd.exe : $(SOURCES)
	cl /MTd /D_MT $(CFLAGS) -o r6mdd.exe -D__STL_DEBUG /D_STLP_DEBUG -DBOOST_RE_TEST_LOCALE_CPP -DTEST_UNICODE $(SOURCES) $(LFLAGS)










