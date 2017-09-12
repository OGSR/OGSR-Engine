'''
    >>> from nested_ext import *
    
    >>> X
    <class 'nested_ext.X'>
    
    >>> X.__module__
    'nested_ext'
    
    >>> X.__name__
    'X'
    
    >>> X.Y
    <class 'nested_ext.Y'>
    
    >>> X.Y.__module__
    'nested_ext'
    
    >>> X.Y.__name__
    'Y'
    
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
