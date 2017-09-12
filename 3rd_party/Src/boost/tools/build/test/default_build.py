#!/usr/bin/python

# Test that default build clause actually has any effect.

from BoostBuild import Tester
t = Tester(pass_toolset=0)

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", "exe a : a.cpp : : debug release ;")
t.write("a.cpp", "int main() { return 0; }\n")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that we can declare default build only in the first
# alternative
t.write("Jamfile", """
exe a : a.cpp : : debug release ;
exe a : b.cpp : : debug release ;
""")
expected="""error: default build can be specified only in first alternative
main target is  ./a

"""
t.run_build_system("--no-error-backtrace", status=1, stdout=expected)


# Now try a harder example: default build which contains <define>
# should cause <define> to be present when "b" is compiled.
# This happens only of "build-project b" is placed first.
t.write("Jamfile", """
    project 
	: default-build <define>FOO 
	;

    build-project a ;
    build-project b ;	
""")

t.write("a/Jamfile", """
    exe a : a.cpp ../b/b ;
""")
t.write("a/a.cpp", """
void foo();
int main() { foo(); }
""")

t.write("b/Jamfile", """
    lib b : b.cpp ;
""")
t.write("b/b.cpp", """
#ifdef FOO
void foo() {}
#endif
""")

# Uncomment when BB10 is resolved.
#t.run_build_system()

t.cleanup()
