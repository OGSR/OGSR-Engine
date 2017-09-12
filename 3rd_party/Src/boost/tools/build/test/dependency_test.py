#!/usr/bin/python

from BoostBuild import Tester, List

t = Tester()

t.set_tree("dependency-test")
t.run_build_system()
# Check that main target 'c' was able to find 'x.h' from
# 'a's dependency graph
t.expect_addition("bin/$toolset/debug/main-target-c/c.exe")
# Check that main target 'e' was able to find 'y.h'
t.expect_addition("bin/$toolset/debug/main-target-e/e.exe")

# Check handling of first level includes.

# Both 'a' and 'b' include "a.h" and should be updated
t.touch("a.h")
t.run_build_system()

t.expect_touch("bin/$toolset/debug/a.exe")
t.expect_touch("bin/$toolset/debug/a.obj")
t.expect_touch("bin/$toolset/debug/b.exe")
t.expect_touch("bin/$toolset/debug/b.obj")
t.expect_touch("bin/$toolset/debug/main-target-c/c.exe")
t.expect_nothing_more()

# Only 'a' include <a.h> and should be updated
t.touch("src1/a.h")
t.run_build_system()

t.expect_touch("bin/$toolset/debug/a.exe")
t.expect_touch("bin/$toolset/debug/a.obj")
t.expect_touch("bin/$toolset/debug/main-target-c/c.exe")
t.expect_nothing_more()

# "src/a.h" includes "b.h" (in the same dir)
t.touch("src1/b.h")
t.run_build_system()
t.expect_touch("bin/$toolset/debug/a.exe")
t.expect_touch("bin/$toolset/debug/a.obj")
t.expect_touch("bin/$toolset/debug/main-target-c/c.exe")
t.expect_nothing_more()

# included by "src/b.h". We had a bug: file included via "",
# like "b.h" is in this case was not scanned at all.
t.touch("src1/c.h")
t.run_build_system()
t.expect_touch("bin/$toolset/debug/a.exe")

t.touch("b.h")
t.run_build_system()
t.expect_nothing_more()

# Test dependency on generated header.
# TODO: we have also to check that generated header is found correctly
# if it is different for different subvariants. Lacking any toolset
# support, this check will be implemented later.
t.touch("x.foo")
t.run_build_system()
t.expect_touch("bin/$toolset/debug/a.obj")

t.cleanup()
