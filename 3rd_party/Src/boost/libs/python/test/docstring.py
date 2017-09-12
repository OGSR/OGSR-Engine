'''
>>> from docstring_ext import *

>>> def printdoc(x):
...     print x.__doc__

>>> printdoc(X)
A simple class wrapper around a C++ int
includes some error-checking

>>> printdoc(X.__init__)
this is the __init__ function
its documentation has two lines.

>>> printdoc(X.value)
gets the value of the object

>>> printdoc(create)
creates a new X object

>>> printdoc(fact)
compute the factorial
'''

def run(args = None):
    import sys
    import doctest

    if args is not None:
        sys.argv = args
        
    import docstring_ext
    
    result = doctest.testmod(sys.modules.get(__name__))
    
    import pydoc
    import re
    docmodule = lambda m: re.sub(".\10", "", pydoc.text.docmodule(m))
    try:
        print 'printing module help:'
        print docmodule(docstring_ext)
    except object, x:
        print '********* failed **********'
        print x
        result = list(result)
        result[0] += 1
        return tuple(result)
        
    return result
    
if __name__ == '__main__':
    print "running..."
    import sys
    sys.exit(run()[0])
