#!/usr/bin/python

# This tests a bug where files were places to a directory corresponding to a main
# target that was using its main target.

from BoostBuild import Tester
t = Tester()

# In order to correctly link this app, 'b.cpp', created by 'make'
# rule, should be compiled.
t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", """
lib b : b.cpp : <define>BAR ;
exe a : a.cpp b ;
""")
t.write("a.cpp", """
void foo();
int main() { foo(); }
""")
t.write("b.cpp", "void foo() {}\n")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/main-target-b/b.o")

# This tests another bug: when source file was used by two main targets,
# one without any requirements and another with free requirements, it 
# was compiled twice with to the same locaiton. 

t.write("Jamfile", """
exe a : a.cpp ;
exe b : a.cpp : <define>FOO ;
""")
t.write("a.cpp", """
int main() { return 0; }
""")

t.rm("bin")
t.run_build_system()
t.expect_addition(["bin/$toolset/debug/a.o", "bin/$toolset/debug/main-target-b/a.o"])


t.cleanup()
