from __future__ import generators

def enumerate(seq):
    i = 0
    for x in seq:
        yield i, x
        i += 1
