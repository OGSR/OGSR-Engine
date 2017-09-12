'''
# Try to reproduce a Numeric interaction bug if Numeric is installed.
>>> from bienstman1_ext import *
>>> try: from Numeric import *
... except: pass
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
