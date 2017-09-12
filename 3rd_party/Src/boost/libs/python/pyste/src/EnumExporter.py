from Exporter import Exporter
from settings import *

#==============================================================================
# EnumExporter 
#==============================================================================
class EnumExporter(Exporter):
    'Exports enumerators'

    def __init__(self, info):
        Exporter.__init__(self, info)


    def SetDeclarations(self, declarations):
        Exporter.SetDeclarations(self, declarations)
        self.enum = self.GetDeclaration(self.info.name)


    def Export(self, codeunit, expoted_names):
        indent = self.INDENT
        in_indent = self.INDENT*2
        rename = self.info.rename or self.enum.name
        full_name = self.enum.FullName()
        code = indent + namespaces.python + 'enum_< %s >("%s")\n' % (full_name, rename)
        for name in self.enum.values:         
            rename = self.info[name].rename or name
            value_fullname = self.enum.ValueFullName(name)
            code += in_indent + '.value("%s", %s)\n' % (rename, value_fullname)
        code += indent + ';\n\n'
        codeunit.Write('module', code)
