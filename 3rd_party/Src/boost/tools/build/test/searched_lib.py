#!/usr/bin/python

# Test usage of searched-libs: one which are found via -l
# switch to the linker/compiler. 

from BoostBuild import Tester
import string
t = Tester()

# To start with, we have to prepate a library to link with
t.write("lib/project-root.jam", "import gcc ; ")
t.write("lib/Jamfile", "lib test_lib : test_lib.cpp ;")
t.write("lib/test_lib.cpp", "void foo() {}\n");

t.run_build_system(subdir="lib")
t.expect_addition("lib/bin/$toolset/debug/test_lib.dll")

t.copy("lib/bin/$toolset/debug/test_lib.dll", "lib/libtest_lib.dll")


# A regression test: <library>property referring to
# searched-lib was mishandled. As the result, we were
# putting target name to the command line!
# Note that 
#    g++ ...... <.>z
# works nicely in some cases, sending output from compiler
# to file 'z'.
# This problem shows up when searched libs are in usage
# requirements.

t.write('project-root.jam', 'import gcc ;')
t.write('Jamfile', 'exe main : main.cpp d/d2/a ;')
t.write("main.cpp", """
int main() { return 0; }
""")

t.write('d/d2/Jamfile', """
lib test_lib : : <name>test_lib <search>../../lib ;
lib a : a.cpp : : : <library>test_lib ;
""")
t.write('d/d2/a.cpp', """
""")

t.run_build_system()

t.cleanup()
