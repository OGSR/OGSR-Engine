#!/usr/bin/python

# Test staging

from BoostBuild import Tester
t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", """
lib a : a.cpp ;
stage dist : a a.h auxilliary/1 ;
""")
t.write("a.cpp", "")
t.write("a.h", "")
t.write("auxilliary/1", "")

t.run_build_system()
t.expect_addition(["dist/a.dll", "dist/a.h", "dist/1"])

# Test the <location> property
t.write("Jamfile", """
lib a : a.cpp ;
stage dist : a 
    : <variant>debug:<location>ds <variant>release:<location>rs
    ;
""")

t.run_build_system()
t.expect_addition("ds/a.dll")

t.run_build_system("release")
t.expect_addition("rs/a.dll")

t.cleanup()
