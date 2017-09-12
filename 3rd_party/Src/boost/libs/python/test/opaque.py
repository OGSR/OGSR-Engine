# Copyright Gottfried Ganßauge 2003. Permission to copy, use,
# modify, sell and distribute this software is granted provided this
# copyright notice appears in all copies. This software is provided
# "as is" without express or implied warranty, and with no claim as
# to its suitability for any purpose.
"""
>>> from opaque_ext import *
>>> #
>>> # Check for correct conversion
>>> use(get())

# Check that None is converted to a NULL opaque pointer
>>> useany(get())
1
>>> useany(None)
0

# check that we don't lose type information by converting NULL opaque
# pointers to None
>>> assert getnull() is None
>>> useany(getnull())
0

>>> failuse(get())
Traceback (most recent call last):
        ...
RuntimeError: success
>>> #
>>> # Check that there is no conversion from integers ...
>>> use(0)
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
>>> #
>>> # ... and from strings to opaque objects
>>> use("")
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
>>> #
>>> # Now check the same for another opaque pointer type
>>> use2(get2())
>>> failuse2(get2())
Traceback (most recent call last):
        ...
RuntimeError: success
>>> use2(0)
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
>>> use2("")
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
>>> #
>>> # Check that opaque types are distinct
>>> use(get2())
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
>>> use2(get())
Traceback (most recent call last):
        ...
TypeError: bad argument type for built-in operation
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
