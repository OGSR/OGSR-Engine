#!/usr/bin/python

# Test that we can change build directory using 
# the 'build-dir' project attribute.

from BoostBuild import Tester
t = Tester()


# Test that top-level project can affect build dir
t.write("project-root.jam", "import gcc ; ")
t.write("Jamfile", """
project
    : build-dir build
    ;
    
exe a : a.cpp ;
build-project src ;    
""")
t.write("a.cpp", "int main() { return 0; }\n")

t.write("src/Jamfile", "exe b : b.cpp ; ")
t.write("src/b.cpp", "int main() { return 0; }\n")

t.run_build_system()

t.expect_addition(["build/bin/$toolset/debug/a.exe",
                   "build/src/bin/$toolset/debug/b.exe"])
		   
# Test that building from child projects work
t.run_build_system(subdir='src')
t.expect_nothing_more()		   
		   
# Test that project can override build dir
t.write("Jamfile", """
exe a : a.cpp ;
build-project src ;
""")		   

t.write("src/Jamfile", """
project
    : build-dir build 
    ;
exe b : b.cpp ;    
""")

t.run_build_system()
t.expect_addition(["bin/$toolset/debug/a.exe",
                   "src/build/bin/$toolset/debug/b.exe"])
		   
t.cleanup()		   
