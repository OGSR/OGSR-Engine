#!/usr/bin/python
# Test the very basic 'make' functionality.

from BoostBuild import Tester, List

t = Tester()
t.set_tree("test1")


# Check that we can build something

t.run_build_system("-sTOOLSET=yfc")

t.expect_addition("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj")
t.expect_addition("bin/a/yfc/debug/runtime-link-dynamic/a")
t.expect_nothing_more()

t.fail(t.read("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj") !=\
"""
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
a.cpp
""")

t.fail(t.read("bin/a/yfc/debug/runtime-link-dynamic/a") !=\
"""
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
<optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
a.cpp
""")

# Check that we have vanilla target names available

t.touch("a.cpp")
t.run_build_system("-sTOOLSET a.obj")
t.expect_touch("bin/a.obj/yfc/debug/runtime-link-dynamic/a.obj")
t.expect_no_modification("bin/a/yfc/debug/runtime-link-dynamic/a")


# Check that if build request cannot be completely matches, a warning is
# issued and subvariant with link-compatible properties is used

t.write("Jamfile", t.read("Jamfile2"))
stdout="""Warning: cannot exactly satisfy request for ./a with properties
    <optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
Using
    <optimization>space <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
instead.
""")
t.run_build_system("-sTOOLSET=yfc", stdout=stdout)

# Check that conflicting link-incompatible requirements prevent building.
t.write("Jamfile", t.read("Jamfile3"))
stdout="""Warning: cannot satisfy request for ./a with properties
    <optimization>off <rtti>on <runtime-link>dynamic <toolset>yfc <variant>debug
Nothing will be built.
""")
t.run_build_system("-sTOOLSET=yfc", stdout=stdout, status=1)

t.pass_test()

