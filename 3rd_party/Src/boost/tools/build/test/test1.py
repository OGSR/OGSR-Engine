#!/usr/bin/python

import BoostBuild

t = BoostBuild.Tester()

t.write("test.jam","""
actions unbuilt { } unbuilt all ;
ECHO "Hi" ;
""")

t.run_build_system("-ftest.jam", stdout="Hi\n")
t.pass_test()
