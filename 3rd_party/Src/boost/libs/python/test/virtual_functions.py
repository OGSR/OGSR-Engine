'''
>>> from virtual_functions_ext import *

>>> class C1(concrete):
...     def f(self, y):
...         return concrete.f(self, Y(-y.value()))

>>> class C2(concrete):
...     pass

>>> class A1(abstract):
...     def f(self, y):
...         return y.value() * 2

>>> class A2(abstract):
...     pass


>>> y1 = Y(16)
>>> y2 = Y(17)



#
# Test abstract with f overridden
#
>>> a1 = A1(42)
>>> a1.value()
42

# Call f indirectly from C++
>>> a1.call_f(y1)
32

# Call f directly from Python
>>> a1.f(y2)
34

#
# Test abstract with f not overridden
#
>>> a2 = A2(42)
>>> a2.value()
42

# Call f indirectly from C++
>>> try: a2.call_f(y1)
... except AttributeError: pass
... else: print 'no exception'

# Call f directly from Python
>>> try: a2.call_f(y2)
... except AttributeError: pass
... else: print 'no exception'

############# Concrete Tests ############

#
# Test concrete with f overridden
#
>>> c1 = C1(42)
>>> c1.value()
42

# Call f indirectly from C++
>>> c1.call_f(y1)
-16

# Call f directly from Python
>>> c1.f(y2)
-17

#
# Test concrete with f not overridden
#
>>> c2 = C2(42)
>>> c2.value()
42

# Call f indirectly from C++
>>> c2.call_f(y1)
16

# Call f directly from Python
>>> c2.f(y2)
17


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
