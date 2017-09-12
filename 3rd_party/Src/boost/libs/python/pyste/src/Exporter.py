import os.path

#==============================================================================
# Exporter
#==============================================================================
class Exporter:
    'Base class for objects capable to generate boost.python code.'

    INDENT = ' ' * 4
    
    def __init__(self, info, parser_tail=None):
        self.info = info
        self.parser_tail = parser_tail
    
        
    def Parse(self, parser):
        self.parser = parser
        header = self.info.include
        tail = self.parser_tail
        declarations, parser_header = parser.parse(header, tail=tail)
        self.parser_header = parser_header
        self.SetDeclarations(declarations)


    def SetDeclarations(self, declarations):
        self.declarations = declarations

        
    def GenerateCode(self, codeunit, exported_names):
        self.WriteInclude(codeunit)
        self.Export(codeunit, exported_names)        


    def WriteInclude(self, codeunit):
        codeunit.Write('include', '#include <%s>\n' % self.info.include)
        
        
    def Export(self, codeunit, exported_names):
        'subclasses must override this to do the real work'
        pass
    
                    
    def Name(self):
        '''Returns the name of this Exporter. The name will be added to the
        list of names exported, which may have a use for other exporters.
        '''
        return None

    
    def GetDeclarations(self, fullname):
        decls = [x for x in self.declarations if x.FullName() == fullname]
        if not decls:
            raise RuntimeError, 'no %s declaration found!' % fullname
        return decls


    def GetDeclaration(self, fullname):
        decls = self.GetDeclarations(fullname)
        assert len(decls) == 1
        return decls[0]


    def Order(self):
        '''Returns a number that indicates to which order this exporter
        belongs.  The exporters will be called from the lowest order to the
        highest order.
        This function will only be called after Parse has been called. 
        '''
        return None # don't care
