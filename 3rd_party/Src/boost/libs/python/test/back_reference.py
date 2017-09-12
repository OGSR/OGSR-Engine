'''
>>> from back_reference_ext import *
>>> y = Y(3)
>>> z = Z(4)
>>> x_instances()
2
>>> y2 = copy_Y(y)
>>> x_instances()
3
>>> z2 = copy_Z(z)
>>> x_instances()
4
>>> assert y_identity(y) is y
>>> y_equality(y, y)
1
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
