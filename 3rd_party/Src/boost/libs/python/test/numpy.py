def numeric_tests():
    '''
    >>> from numpy_ext import *
    >>> x = new_array()
    >>> x[1,1] = 0.0

    >>> try: take_array(3)
    ... except TypeError: pass
    ... else: print 'expected a TypeError'

    >>> take_array(x)

    >>> print x
    [[1 2 3]
     [4 0 6]
     [7 8 9]]

    >>> y = x.copy()


    >>> p = _printer()
    >>> check = p.check
    >>> exercise(x, p)
    >>> y[2,1] = 3
    >>> check(y);

    >>> check(y.astype('D'));

    >>> check(y.copy());

    >>> check(y.typecode());

    >>> p.results
    []
    >>> del p
    '''
    pass

def _numarray_tests():
    '''
    >>> from numpy_ext import *
    >>> x = new_array()
    >>> y = x.copy()
    >>> p = _printer()
    >>> check = p.check
    >>> exercise_numarray(x, p)

    >>> check(y.astype());

    >>> check(y.argmax());
    >>> check(y.argmax(0));

    >>> check(y.argmin());
    >>> check(y.argmin(0));

    >>> check(y.argsort());
    >>> check(y.argsort(1));
    
    >>> y.byteswap();
    >>> check(y);

    >>> check(y.diagonal());
    >>> check(y.diagonal(1));
    >>> check(y.diagonal(0, 1));
    >>> check(y.diagonal(0, 1, 0));

    >>> check(y.is_c_array());
    >>> check(y.isbyteswapped());

    >>> check(y.trace());
    >>> check(y.trace(1));
    >>> check(y.trace(0, 1));
    >>> check(y.trace(0, 1, 0));

    >>> check(y.new('D'));
    >>> y.sort();
    >>> check(y);
    >>> check(y.type());

    >>> check(y.array((1.2, 3.4)));
    >>> check(y.array((1.2, 3.4), "Double"));
    >>> check(y.array((1.2, 3.4), "Double", (1,2,1)));
    >>> check(y.array((1.2, 3.4), "Double", (2,1,1), false));
    >>> check(y.array((1.2, 3.4), "Double", (2,), true, true));
    
    >>> p.results
    []
    >>> del p
    '''
    pass

false = 0;
true = 1;
class _printer(object):
    def __init__(self):
        self.results = [];
    def __call__(self, *stuff):
        self.results += [ str(x) for x in stuff ]
    def check(self, x):
        if self.results[0] == str(x):
            del self.results[0]
        else:
            print '  Expected:\n %s\n  but got:\n %s' % (x, self.results[0])

def _run(args = None):
    import sys
    import doctest

    if args is not None:
        sys.argv = args

    # See which of the numeric modules are installed
    has_numeric = 0
    try:
        import Numeric
        m = Numeric
        has_numeric = 1
    except ImportError: pass

    has_numarray = 0
    try:
        import numarray
        m = numarray
        has_numarray = 1
    except ImportError: pass
    
    # Bail if neither one is installed
    if not (has_numeric or has_numarray):
        return 0

    # test the info routine outside the doctest. See numpy.cpp for an
    # explanation
    import numpy_ext
    if (has_numarray):
        numpy_ext.info(m.array((1,2,3)))

    failures = 0

    #
    # Run tests 4 different ways if both modules are installed, just
    # to show that set_module_and_type() is working properly
    #
    
    # run all the tests with default module search
    print 'testing default extension module'
    failures += doctest.testmod(sys.modules.get(__name__))[0]

    # test against Numeric if installed
    if has_numeric:
        print 'testing Numeric module explicitly'
        numpy_ext.set_module_and_type('Numeric', 'ArrayType')
        failures += doctest.testmod(sys.modules.get(__name__))[0]
    
    global __test__
    if has_numarray:
        # Add the _numarray_tests to the list of things to test in
        # this case.
        __test__ = { 'numarray_tests':_numarray_tests,
                     'numeric_tests': numeric_tests }
        print 'testing numarray module explicitly'
        numpy_ext.set_module_and_type('numarray', 'NDArray')
        failures += doctest.testmod(sys.modules.get(__name__))[0]
        del __test__

    # see that we can go back to the default
    print 'testing default module again'
    numpy_ext.set_module_and_type('', '')
    failures += doctest.testmod(sys.modules.get(__name__))[0]
    
    return failures
    
if __name__ == '__main__':
    print "running..."
    import sys
    sys.exit(_run())
