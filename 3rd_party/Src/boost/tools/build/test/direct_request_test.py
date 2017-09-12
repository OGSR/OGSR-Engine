#!/usr/bin/python

from BoostBuild import Tester, List
import os
from string import strip

t = Tester()

# First check some startup
t.set_tree("direct-request-test")
t.run_build_system(extra_args="define=MACROS")

t.expect_addition("bin/$toolset/debug/" 
                  * (List("a.o b.o b.dll a.exe")))
		  
# Regression test: direct build request was not working
# when there's more than one level of 'build-project'

t.rm(".")
t.write('project-root.jam', '')
t.write('Jamfile', 'build-project a ;')
t.write('a/Jamfile', 'build-project b ;')
t.write('a/b/Jamfile', '')

t.run_build_system("release")		  


t.cleanup()
