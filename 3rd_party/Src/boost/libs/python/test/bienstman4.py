'''
>>> from bienstman4_ext import *
>>> t1 = T1()
>>> e = Expression()
>>> e.add(t1)
'''
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
