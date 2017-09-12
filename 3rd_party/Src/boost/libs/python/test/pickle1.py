r'''>>> import pickle1_ext
    >>> import pickle
    >>> pickle1_ext.world.__module__
    'pickle1_ext'
    >>> pickle1_ext.world.__safe_for_unpickling__
    1
    >>> pickle1_ext.world.__name__
    'world'
    >>> pickle1_ext.world('Hello').__reduce__()
    (<class 'pickle1_ext.world'>, ('Hello',))
    >>> wd = pickle1_ext.world('California')
    >>> pstr = pickle.dumps(wd)
    >>> wl = pickle.loads(pstr)
    >>> print wd.greet()
    Hello from California!
    >>> print wl.greet()
    Hello from California!
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
