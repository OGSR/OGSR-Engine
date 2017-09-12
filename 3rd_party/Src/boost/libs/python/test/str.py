"""
>>> from str_ext import *
>>> def printer(*args):
...     for x in args: print x,
...     print
...
>>> work_with_string(printer)
['this', 'is', 'a', 'demo', 'string']
['this', 'is', 'a', 'demo string']
this<->is<->a<->demo<->string
This is a demo string
[    this is a demo string     ]
2
this is a demo string
this is a demo string
0
0
['this is a demo string']
this is a demo string
THIS IS A DEMO STRING
This Is A Demo String
find
10
10 3 5
10
10
expandtabs
                tab     demo    !
        tab demo    !
              tab    demo   !
operators
part1part2
this is a blabla string
18
18
0
0
aaaaaaaaaaaaaaaaaaaaa
"""

def run(args = None):
    import sys
    import doctest

    if args is not None:
        sys.argv = args
    return doctest.testmod(sys.modules.get(__name__))
    
if __name__ == '__main__':
    print "running..."
    import sys
    sys.exit(run()[0])
