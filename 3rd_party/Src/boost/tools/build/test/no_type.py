#!/usr/bin/python

# Test that we cannot specify targets of unknown type as sources.
# This is based on the fact that Unix 'ar' will happily consume
# just about anything.

from BoostBuild import Tester
t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", """
static-lib a : a.foo ;
""")
t.write("a.foo", "")

t.run_build_system(status=1)

t.cleanup()
