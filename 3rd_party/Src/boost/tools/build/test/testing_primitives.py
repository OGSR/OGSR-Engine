#!/usr/bin/python

from BoostBuild import Tester, List
import os
from string import strip
import re

def match_re(actual,expected):
    return re.match(expected,actual,re.DOTALL) != None

t = Tester(match = match_re)

t.set_tree('testing-primitives')

# We expect t5 and t7's output to be dumped to stdout
t.run_build_system(stdout=r'''.*failing t5.*failing t7''')

t.expect_addition('t2.txt')
t.expect_addition('t3.txt')

t.expect_addition('t5.out')

t.expect_addition('t6.out')
t.expect_addition('t6.txt')

t.expect_addition('t7.out')
t.expect_addition('t7.txt')

t.expect_addition('t8.out')

t.expect_nothing_more()
t.cleanup()
print 'tesing complete'
