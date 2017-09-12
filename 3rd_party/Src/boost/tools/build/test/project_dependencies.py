#!/usr/bin/python

# Test that we can specify a dependency property
# in project requirements, and that it won't
# cause every main target in the project to
# be generated in it's own subdirectory.

from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", "build-project src ;")
t.write("lib/Jamfile", "lib lib1 : lib1.cpp ;")
t.write("lib/lib1.cpp", "void foo() {}\n")
t.write("src/Jamfile", """
project
    : requirements <library>../lib/lib1
    ;
    
exe a : a.cpp ;
exe b : b.cpp ;    
""")
t.write("src/a.cpp", """
void foo();
int main() { foo(); return 0; }
""")
t.copy("src/a.cpp", "src/b.cpp")

t.run_build_system()

# Test that there's no "main-target-a" part.
t.expect_addition("src/bin/$toolset/debug/" * List("a.exe b.exe"))


t.cleanup()
