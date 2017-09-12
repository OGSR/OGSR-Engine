#!/usr/bin/python

# Test conditional properties

from BoostBuild import Tester, List
import os
from string import strip

t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("a.cpp", """
#ifdef STATIC
int main() {  return 0; }
#endif
""")
t.write("Jamfile", "exe a : a.cpp : <link>static:<define>STATIC ;")
t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/main-target-a/a.exe")

t.write("Jamfile", """
project : requirements <link>static:<define>STATIC ;
exe a : a.cpp ;
""")
t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/a.exe")

t.cleanup()
