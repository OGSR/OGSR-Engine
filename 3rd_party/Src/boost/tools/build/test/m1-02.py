#!/usr/bin/python

# Tests that 'make' accepts target from other directories and that
# build request for those targets can be overriden.

from BoostBuild import Tester, List

t = Tester()
t.set_tree("test1")

t.run_build_system("-sTOOLSET=yfc")

t.expect_addition("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj")
t.expect_addition("auxillary/bin/b.obj/yfc/debug/runtime-link-dynamic/optimization-space/b.obj")
t.expect_addition("bin/a/yfc/debug/runtime-link-dynamic/a")
t.expect_nothing_more()

t.fail(t.read("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj") !=\
"""
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
a.cpp
""")

t.fail(t.read("auxillary/bin/b.obj/yfc/debug/runtime-link-dynamic/b.obj") !=\
"""
<optimization>space <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
b.cpp
""")


t.fail(t.read("bin/a/yfc/debug/runtime-link-dynamic/a") !=\
"""
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
a.cpp
<optimization>space <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
b.cpp
""")

# Check that we have vanilla target names available in subdirs

t.touch("auxillary/b.cpp")
t.run_build_system("-sTOOLSET b.obj", subdir="auxillary")
t.expect_touch("auxillary/bin/b.obj/yfc/debug/runtime-link-dynamic/optimization-space/b.obj")
t.expect_no_modification("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj")
t.expect_no_modification("bin/a/yfc/debug/runtime-link-dynamic/a")


# Check that we cannot request link-incompatible property for source target

t.write('Jamfile', t.read('Jamfile2'))
stdout="""Error: subvariant of target ./a with properties
    <optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
requests link-incompatible property
    <rtti>off
for source @auxillary/b.obj
"""
t.run_build_system("-sTOOLSET=yfc", stdout=stdout)

# Check that if we request link-compatible property then requirement for
# the source target will override it, with warning. This is similar to
# the way build requests are satisfied (see the first test)
# CONSIDER: should be print the main target which requests this one
# (and modifies requiremenets)?

t.write('Jamfile3', t.read('Jamfile3'))
t.write('auxillary/Jamfile3', t.read('auxillary/Jamfile3'))
stdout="""Warning: cannot exactly satisfy request for auxillary/b.obj with properties
    <optimization>space <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
Using
    <optimization>speed <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
instead.
"""
t.run_build_system("-sTOOLSET=yfc", stdout=stdout)


# Check for link-incompatible properties

t.write('Jamfile4', t.read('Jamfile4'))
t.write('auxillary/Jamfile4', t.read('auxillary/Jamfile4'))
stdout="""Warning: cannot satisfy request for auxillary/b.obj with properties
    <optimization>space <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
Nothing will be built.
""")


t.pass_test()

