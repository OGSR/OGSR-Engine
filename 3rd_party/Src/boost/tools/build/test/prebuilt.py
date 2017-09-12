#!/usr/bin/python

# Test that we can use already built sources

from BoostBuild import Tester
t = Tester()

t.set_tree('prebuilt')

# First, build the external project
t.run_build_system("debug release", subdir="ext")

# Then pretend that we don't have the sources for the external project,
# and can only use compiled binaries
t.copy("ext/Jamfile2", "ext/Jamfile")

# Now check that we can build the main project, and that
# correct prebuilt file is picked, depending of variant.
# This also checks that correct includes for prebuilt
# libraries are used.

t.run_build_system("debug release")
t.expect_addition("bin/$toolset/debug/main-target-hello/hello.exe")
t.expect_addition("bin/$toolset/release/main-target-hello/hello.exe")

t.cleanup()
