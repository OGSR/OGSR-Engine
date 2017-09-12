'''
>>> from shared_ptr_ext import *

>>> type(factory(3))
<class 'shared_ptr_ext.Y'>
>>> type(factory(42))
<class 'shared_ptr_ext.YY'>

>>> class P(Z):
...     def v(self):
...         return -Z.v(self);
...     def __del__(self):
...         print 'bye'
...
>>> p = P(12)
>>> p.value()
12
>>> p.v()
-12
>>> look(p)
12
>>> try: modify(p)
... except TypeError: pass
... else: 'print expected a TypeError'
>>> look(None)
-1
>>> store(p)
>>> del p
>>> stored_v()
-12
>>> z_count()
1
>>> z_look_store()
12
>>> z_release()
bye
>>> z_count()
0

>>> z = Z(13)
>>> z.value()
13
>>> z.v()
13
>>> try: modify(z)
... except TypeError: pass
... else: 'print expected a TypeError'
>>> store(z)
>>> assert stored_z() is z  # show that deleter introspection works
>>> del z
>>> stored_v()
13
>>> z_count()
1
>>> z_look_store()
13
>>> z_release()
>>> z_count()
0

>>> x = X(17)
>>> x.value()
17
>>> look(x)
17
>>> try: modify(x)
... except TypeError: pass
... else: 'print expected a TypeError'
>>> look(None)
-1
>>> store(x)
>>> del x
>>> x_count()
1
>>> x_look_store()
17
>>> x_release()
>>> x_count()
0


>>> y = Y(19)
>>> y.value()
19
>>> modify(y)
>>> look(y)
-1
>>> store(Y(23))
>>> y_count()
1
>>> y_look_store()
23
>>> y_release()
>>> y_count()
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
