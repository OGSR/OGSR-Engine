#!/usr/bin/python

# Test that unused sources are at least reported.

from BoostBuild import Tester
from string import find
t = Tester()

t.write("a.h", """ 
""")

t.write("a.cpp", """ 
int main()
{
    return 0;
} 
""")

t.write("Jamfile", """ 
exe a : a.cpp a.h ; 
""")

t.write("project-root.jam", """ 
""")

t.run_build_system()
t.fail_test(find(t.stdout(), "warning: Unused source target { a.H }") == -1)

t.cleanup()

