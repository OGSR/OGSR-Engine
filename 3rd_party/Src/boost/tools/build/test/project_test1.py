#!/usr/bin/python

from BoostBuild import Tester
import os

t = Tester("--build-system=project-test1", boost_build_path='', pass_toolset=0)

# This test does no modifications, so run in in the invocation dir

os.chdir(t.original_workdir)


expected_output1="""Project Roots:

"""

expected_output2="""'%(root-dir-prefix)sdir2':

  Module for project-root is 'project-root<%(root-dir-prefix)sdir2>'

Projects:

'/cool-library':

* Project root: %(root-dir-prefix)sdir2
* Parent project: (none)
* Requirements: <include>/home/ghost/build/boost-cvs
* Default build:
* Source location: %(root-dir-prefix)sdir2
* Projects to build:

"""

expected_output3="""'%(root-dir)s':

  Module for project-root is 'project-root<%(root-dir)s>'

Projects:

'/boost-build-test-project-1':

* Project root: %(root-dir)s
* Parent project: (none)
* Requirements: <threading>multi <include>/home/ghost/local/include
* Default build:
* Source location: %(root-dir)s
* Projects to build: dir dir2

'/boost-build-test-project-1/dir':

* Project root: %(root-dir)s
* Parent project: %(root-dir)s
* Requirements: <threading>multi <include>/home/ghost/local/include
* Default build: release
* Source location: %(root-dir-prefix)sdir/src
* Projects to build:

"""

# Test that correct project structure is created when jam is invoked
# outside of the source tree.
expected = (expected_output1 + expected_output2 + expected_output3) % \
    {"root-dir": "project-test1",
     "root-dir-prefix": "project-test1/" }

t.run_build_system(stdout=expected)

# Test that correct project structure is created when jam is invoked
# at the top of the source tree.
expected = (expected_output1 + expected_output3 + expected_output2) % \
    {"root-dir": ".",
     "root-dir-prefix": "" }

os.chdir("project-test1")
t.run_build_system(stdout=expected)

t.cleanup()
