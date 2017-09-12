'''
>>> from long_ext import *
>>> new_long()
0L
>>> longify(42)
42L
>>> longify_string('300')
300L
>>> is_long(20L)
'yes'
>>> is_long('20')
0
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
