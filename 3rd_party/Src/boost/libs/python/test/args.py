"""
>>> from args_ext import *

>>> raw(3, 4, foo = 'bar', baz = 42)
((3, 4), {'foo': 'bar', 'baz': 42})

>>> f(x= 1, y = 3, z = 'hello')
(1, 3.0, 'hello')

>>> f(z = 'hello', x = 3, y = 2.5)
(3, 2.5, 'hello')

>>> f(1, z = 'hi', y = 3)
(1, 3.0, 'hi')

>>> try: f(1, 2, 'hello', bar = 'baz')
... except TypeError: pass
... else: print 'expected an exception: unknown keyword'


   Exercise the functions using default stubs

>>> f1(z = 'nix', y = .125, x = 2)
(2, 0.125, 'nix')
>>> f1(y = .125, x = 2)
(2, 0.125, 'wow')
>>> f1(x = 2)
(2, 4.25, 'wow')
>>> f1()
(1, 4.25, 'wow')

>>> f2(z = 'nix', y = .125, x = 2)
(2, 0.125, 'nix')
>>> f2(y = .125, x = 2)
(2, 0.125, 'wow')
>>> f2(x = 2)
(2, 4.25, 'wow')
>>> f2()
(1, 4.25, 'wow')

>>> f3(z = 'nix', y = .125, x = 2)
(2, 0.125, 'nix')
>>> f3(y = .125, x = 2)
(2, 0.125, 'wow')
>>> f3(x = 2)
(2, 4.25, 'wow')
>>> f3()
(1, 4.25, 'wow')

   Member function tests

>>> q = X()
>>> q.f(x= 1, y = 3, z = 'hello')
(1, 3.0, 'hello')

>>> q.f(z = 'hello', x = 3, y = 2.5)
(3, 2.5, 'hello')

>>> q.f(1, z = 'hi', y = 3)
(1, 3.0, 'hi')

>>> try: q.f(1, 2, 'hello', bar = 'baz')
... except TypeError: pass
... else: print 'expected an exception: unknown keyword'

   Exercise member functions using default stubs
   
>>> q.f1(z = 'nix', y = .125, x = 2)
(2, 0.125, 'nix')
>>> q.f1(y = .125, x = 2)
(2, 0.125, 'wow')
>>> q.f1(x = 2)
(2, 4.25, 'wow')
>>> q.f1()
(1, 4.25, 'wow')

>>> X.f.__doc__
"This is X.f's docstring"

>>> xfuncs = (X.inner0, X.inner1, X.inner2, X.inner3, X.inner4, X.inner5)
>>> for f in xfuncs:
...    print f(q,1).value(),
...    print f(q, n = 1).value(),
...    print f(q, n = 0).value(),
...    print f.__doc__
1 1 0 docstring
1 1 0 docstring
1 1 0 docstring
1 1 0 docstring
1 1 0 docstring
1 1 0 docstring

>>> x = X(a1 = 44, a0 = 22)
>>> x.inner0(0).value()
22
>>> x.inner0(1).value()
44

>>> x = X(a0 = 7)
>>> x.inner0(0).value()
7
>>> x.inner0(1).value()
1

>>> inner(n = 1, self = q).value()
1

>>> y = Y(value = 33)
>>> y.raw(this = 1, that = 'the other')[1]
{'this': 1, 'that': 'the other'}

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













































