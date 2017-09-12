#!/usr/bin/python

from BoostBuild import Tester
t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", "lib a : a.cpp : <include>. ;")
t.write("a.cpp", """
#include <a.h>
void foo() {}
""")
t.write("a.h", "")

t.write("d/Jamfile", "exe b : b.cpp ../a ; ")
t.write("d/b.cpp", """
    void foo();
    int main() { foo(); }
""")

t.run_build_system(subdir="d")

t.cleanup()
