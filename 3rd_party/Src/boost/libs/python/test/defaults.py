"""
>>> False = 0 # Python 2.2 needs these
>>> True = 1

>>> from defaults_ext import *
>>> bar(1)
'int(1); char(D); string(default); double(0.0); '

>>> bar(2, 'X')
'int(2); char(X); string(default); double(0.0); '

>>> bar(3, 'Y', "Hello World")
'int(3); char(Y); string(Hello World); double(0.0); '

>>> bar(4, 'Z', "Hi There", 3.3)
'int(4); char(Z); string(Hi There); double(3.3); '

>>> foo(1)
'int(1); char(D); string(default); double(0.0); '

>>> foo(2, 'X')
'int(2); char(X); string(default); double(0.0); '

>>> foo(3, 'Y', "Hello World")
'int(3); char(Y); string(Hello World); double(0.0); '

>>> foo(4, 'Z', "Hi There", 3.3)
'int(4); char(Z); string(Hi There); double(3.3); '

>>> x = X()
>>> x.bar(1)
'int(1); char(D); string(default); double(0.0); '

>>> x.bar(2, 'X')
'int(2); char(X); string(default); double(0.0); '

>>> x.bar(3, 'Y', "Hello World")
'int(3); char(Y); string(Hello World); double(0.0); '

>>> x.bar(4, 'Z', "Hi There", 3.3)
'int(4); char(Z); string(Hi There); double(3.3); '

>>> x.foo(5)
'int(5); bool(0); '

>>> x.foo(6, 0)
'int(6); bool(0); '

>>> x.foo(7, 1)
'int(7); bool(1); '

>>> x.foo("A")
'string(A); bool(0); '

>>> x.foo("B", False)
'string(B); bool(0); '

>>> x.foo("C", True)
'string(C); bool(1); '

>>> x.foo([0,1,2], [2,3,4])
'list([0, 1, 2]); list([2, 3, 4]); bool(0); '

>>> x.foo([0,1,2], [2,3,4], False)
'list([0, 1, 2]); list([2, 3, 4]); bool(0); '

>>> x.foo([0,1,2], [2,3,4], True)
'list([0, 1, 2]); list([2, 3, 4]); bool(1); '

>>> x = X(1)
>>> x.get_state()
'int(1); char(D); string(constructor); double(0.0); '

>>> x = X(1, 'X')
>>> x.get_state()
'int(1); char(X); string(constructor); double(0.0); '

>>> x = X(1, 'X', "Yabadabadoo")
>>> x.get_state()
'int(1); char(X); string(Yabadabadoo); double(0.0); '

>>> x = X(1, 'X', "Phoenix", 3.65)
>>> x.get_state()
'int(1); char(X); string(Phoenix); double(3.65); '

>>> x.bar2().get_state()
'int(0); char(D); string(default); double(0.0); '

>>> x.bar2(1).get_state()
'int(1); char(D); string(default); double(0.0); '

>>> x.bar2(1, 'K').get_state()
'int(1); char(K); string(default); double(0.0); '

>>> x.bar2(1, 'K', "Kim").get_state()
'int(1); char(K); string(Kim); double(0.0); '

>>> x.bar2(1, 'K', "Kim", 9.9).get_state()
'int(1); char(K); string(Kim); double(9.9); '

>>> x = X("Phoenix", 1)
>>> x.get_state()
'Got exactly two arguments from constructor: string(Phoenix); bool(1); '

>>> def printdoc(x):
...     print x.__doc__

>>> printdoc(X.__init__)
doc of init

>>> printdoc(Y.__init__)
doc of Y init

>>> printdoc(X.bar2)
doc of X::bar2

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













































