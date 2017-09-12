'''
Various helpers for interface files.
'''

from settings import *

#==============================================================================
# FunctionWrapper
#==============================================================================
class FunctionWrapper(object): 
    '''Holds information about a wrapper for a function or a method. It is in 2
    parts: the name of the Wrapper, and its code.  The code is placed in the
    declaration section of the module, while the name is used to def' the
    function or method (with the pyste namespace prepend to it). If code is None,
    the name is left unchanged.
    '''

    def __init__(self, name, code=None):
        self.name = name
        self.code = code

    def FullName(self):
        if self.code:
            return namespaces.pyste + self.name
        else:
            return self.name
