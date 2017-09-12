#!/usr/bin/python

# This tests that 
# 1) the 'make' correctly assigns types to produced targets
# 2) than if 'make' create targets of type CPP, they are
# correctly used (there was a bug with it).

from BoostBuild import Tester
t = Tester()

# In order to correctly link this app, 'b.cpp', created by 'make'
# rule, should be compiled.
t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", r'''
rule create ( dst : src * : properties * )
{
    # hack to echo a space under NT
    setup on $(dst) = "set x=int main(){ return 0; }" ;
}

import modules ;
if [ modules.peek : NT ]
{
    actions create 
    {
        $(setup)
        echo %x% > $(<)
    }
}
else
{
    actions create 
    {
        echo "int main(){ return 0; }" > $(<)
    }
}

IMPORT $(__name__) : create : : create ;

exe a : l ;

# needs to be static lib for Windows - main can't appear in DLL
static-lib l : a.cpp b.cpp ;

make b.cpp : : create ;

''')
t.write("a.cpp", "")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")

t.cleanup()
