"""
>>> from dict_ext import *
>>> def printer(*args):
...     for x in args: print x,
...     print
...
>>> print new_dict()
{}
>>> print data_dict()
{1: {'key2': 'value2'}, 'key1': 'value1'}
>>> tmp = data_dict()
>>> print dict_keys(tmp)
[1, 'key1']
>>> print dict_values(tmp)
[{'key2': 'value2'}, 'value1']
>>> print dict_items(tmp)
[(1, {'key2': 'value2'}), ('key1', 'value1')]
>>> print dict_from_sequence([(1,1),(2,2),(3,3)])
{1: 1, 2: 2, 3: 3}
>>> test_templates(printer)
a test string
13
None
{1.5: 13, 1: 'a test string'}
default
0
default
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
