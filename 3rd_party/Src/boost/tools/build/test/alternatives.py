#!/usr/bin/python

# Test main target alternatives.

from BoostBuild import Tester
t = Tester()


# Test that basic alternatives selection works.
t.write("project-root.jam", " ")
t.write("Jamfile", """

exe a : a_empty.cpp ;
exe a : a.cpp : <variant>release ;
""")
t.write("a_empty.cpp", "")
t.write("a.cpp", "int main() { return 0; }\n")

t.run_build_system("release")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that everything works ok even with default
# build.

t.write("Jamfile", """

exe a : a_empty.cpp : <variant>release ;
exe a : a.cpp : <variant>debug ;
""")

t.run_build_system(pass_toolset=0)
t.expect_addition("bin/$toolset/debug/a.exe")


# Test that only properties which are in build request
# matters when selection alternative. IOW, alternative
# with <variant>release is better than one with
# <variant>debug when building release version.
t.write("Jamfile", """

exe a : a_empty.cpp : <variant>debug ;
exe a : a.cpp : <variant>release ;
""")

t.rm("bin/$toolset/release/a.exe")
t.run_build_system("release")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that free properties do not matter. We really don't
# want <cxxflags> property in build request to affect
# alternative selection.
t.write("Jamfile", """
exe a : a_empty.cpp : <variant>debug <define>FOO <include>BAR ;
exe a : a.cpp : <variant>release ;
""")

t.rm("bin/$toolset/release/a.exe")
t.run_build_system("release define=FOO")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that abibuity is reported correctly
t.write("Jamfile", """
exe a : a_empty.cpp ;
exe a : a.cpp ;
""")
expected="""error: Ambiguous alternatives for main target ./a

"""
t.run_build_system("--no-error-backtrace", status=1, stdout=expected)

		   
t.cleanup()		   
