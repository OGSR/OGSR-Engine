from Exporter import Exporter
from ClassExporter import ClassExporter
from FunctionExporter import FunctionExporter
from EnumExporter import EnumExporter
from infos import *
from declarations import *
import os.path
import exporters

#==============================================================================
# HeaderExporter
#==============================================================================
class HeaderExporter(Exporter):
    'Exports all declarations found in the given header'

    def __init__(self, info, parser_tail=None):
        Exporter.__init__(self, info, parser_tail)


    def WriteInclude(self, codeunit):
        pass


    def SetDeclarations(self, declarations):
        def IsInternalName(name):
            '''Returns true if the given name looks like a internal compiler
            structure'''
            return name.startswith('__')
        
        Exporter.SetDeclarations(self, declarations)
        header = os.path.normpath(self.parser_header)
        for decl in declarations:
            # check if this declaration is in the header
            location = os.path.normpath(decl.location[0])
            if location != header or IsInternalName(decl.name):
                continue
            # ok, check the type of the declaration and export it accordingly
            self.HandleDeclaration(decl)
            

    def HandleDeclaration(self, decl):
        '''Dispatch the declaration to the appropriate method, that must create
        a suitable info object for a Exporter, create a Exporter, set its
        declarations and append it to the list of exporters.
        ''' 
        dispatch_table = {
            Class : ClassExporter,
            Enumeration : EnumExporter,
            Function : FunctionExporter,
        }
        
        for decl_type, exporter_type in dispatch_table.items():
            if type(decl) == decl_type:
                self.HandleExporter(decl, exporter_type)
                break

            
    def HandleExporter(self, decl, exporter_type):
        info = self.info[decl.name]
        info.name = decl.FullName()
        info.include = self.info.include
        exporter = exporter_type(info)
        exporter.SetDeclarations(self.declarations)
        exporters.exporters.append(exporter)


        
