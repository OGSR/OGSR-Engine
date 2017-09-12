#!/usr/bin/python

from BoostBuild import Tester, List
import os
import re

spaces_re = re.compile("\ \ +")
trailing_spaces_re = re.compile("\ +\n")

t = Tester(pass_toolset=0)

t.set_tree('module-actions')

expected = r'''A.act t1:   X1-t1     
B.act t1:   X1-t1   X2-B   
act t1:   X1-t1   X2-global   X3-global 
A.act t2:   X1-A   X2-t2   
B.act t2:     X2-t2   
act t2:   X1-global   X2-t2   X3-global 
A.act t3:   X1-A     X3-t3 
B.act t3:     X2-B   X3-t3 
act t3:   X1-global   X2-global   X3-t3 
'''

# On Unixes, call to 'echo 1     2      3' produces '1 2 3' (note spacing)
# Accomodate for that fact.
if os.name != 'nt':
    expected = re.sub(spaces_re, " ", expected)
    expected = re.sub(trailing_spaces_re, "\n", expected)    

# We expect t5 and t7's output to be dumped to stdout
t.run_build_system(
    stdout = expected
)

t.expect_nothing_more()
t.cleanup()

