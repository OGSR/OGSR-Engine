'''
>>> from data_members_ext import *

>>> x = X(42)
>>> x.x
42
>>> try: x.x = 77
... except AttributeError: pass
... else: print 'no error'

>>> x.fair_value
42.0
>>> y = Y(69)
>>> y.x
69
>>> y.x = 77
>>> y.x
77

>>> v = Var("pi")
>>> v.value = 3.14
>>> v.name
'pi'
>>> v.name2
'pi'

>>> v.get_name1()
'pi'

>>> v.get_name2()
'pi'

>>> v.y.x
6
>>> v.y.x = -7
>>> v.y.x
-7

>>> v.name3
'pi'

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
