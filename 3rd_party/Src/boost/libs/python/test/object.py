'''
>>> from object_ext import *
>>> def print1(x):
...     print x
>>> call_object_3(print1)
3
>>> message()
'hello, world!'
>>> number()
42

>>> test('hi')
1
>>> test(None)
0
>>> test_not('hi')
0
>>> test_not(0)
1

        Attributes

>>> class X: pass
...
>>> x = X()

>>> try: obj_getattr(x, 'foo')
... except AttributeError: pass
... else: print 'expected an exception'

>>> obj_setattr(x, 'foo', 1)
>>> x.foo
1
>>> obj_getattr(x, 'foo')
1
>>> obj_const_getattr(x, 'foo')
1
>>> obj_setattr42(x, 'foo')
>>> x.foo
42
>>> obj_moveattr(x, 'foo', 'bar')
>>> x.bar
42
>>> test_attr(x, 'foo')
1
>>> test_not_attr(x, 'foo')
0
>>> x.foo = None
>>> test_attr(x, 'foo')
0
>>> test_not_attr(x, 'foo')
1

        Items

>>> d = {}
>>> obj_setitem(d, 'foo', 1)
>>> d['foo']
1
>>> obj_getitem(d, 'foo')
1
>>> obj_const_getitem(d, 'foo')
1
>>> obj_setitem42(d, 'foo')
>>> obj_getitem(d, 'foo')
42
>>> d['foo']
42
>>> obj_moveitem(d, 'foo', 'bar')
>>> d['bar']
42
>>> obj_moveitem2(d, 'bar', d, 'baz')
>>> d['baz']
42
>>> test_item(d, 'foo')
1
>>> test_not_item(d, 'foo')
0
>>> d['foo'] = None
>>> test_item(d, 'foo')
0
>>> test_not_item(d, 'foo')
1

        Slices
        
>>> assert check_string_slice()

        Operators

        
>>> assert check_binary_operators()

>>> class X: pass
...
>>> assert check_inplace(range(3), X())


       Now make sure that object is actually managing reference counts
       
>>> import weakref
>>> class Z: pass
...
>>> z = Z()
>>> def death(r): print 'death'
...
>>> r = weakref.ref(z, death)
>>> z.foo = 1
>>> obj_getattr(z, 'foo')
1
>>> del z
death
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
