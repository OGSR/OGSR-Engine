from Exporter import Exporter
from policies import *
from declarations import *
from settings import *


class FunctionExporter(Exporter):
    'Generates boost.python code to export the given function.'
    
    def __init__(self, info, tail=None):
        Exporter.__init__(self, info, tail)
        
        
    def Export(self, codeunit, exported_names):
        decls = self.GetDeclarations(self.info.name)
        for decl in decls:
            self.CheckPolicy(decl)
            self.ExportDeclaration(decl, len(decls) == 1, codeunit)
        self.GenerateOverloads(decls, codeunit)            


    def Name(self):
        return self.info.name


    def CheckPolicy(self, func):
        'Warns the user if this function needs a policy'            
        def IsString(type):
            return type.const and type.name == 'char' and isinstance(type, PointerType)    
        needs_policy = isinstance(func.result, (ReferenceType, PointerType))
        if IsString(func.result):
            needs_policy = False
        if needs_policy and self.info.policy is None:
            print '---> Error: Function "%s" needs a policy.' % func.FullName() 
            print 

    def ExportDeclaration(self, decl, unique, codeunit):
        name = self.info.rename or decl.name
        defs = namespaces.python + 'def("%s", ' % name
        wrapper = self.info.wrapper
        if wrapper:
            pointer = '&' + wrapper.FullName()
        elif not unique:                
            pointer = decl.PointerDeclaration()
        else:
            pointer = '&' + decl.FullName()
        defs += pointer            
        defs += self.PolicyCode()                            
        overload = self.OverloadName(decl)
        if overload:
            defs += ', %s()' % (namespaces.pyste + overload)
        defs += ');'
        codeunit.Write('module', self.INDENT + defs + '\n')  
        # add the code of the wrapper
        if wrapper and wrapper.code:
            codeunit.Write('declaration', code + '\n')
            

    def OverloadName(self, decl):
        if decl.minArgs != decl.maxArgs:
            return '%s_overloads_%i_%i' % \
                (decl.name, decl.minArgs, decl.maxArgs)
        else:
            return ''

        
    def GenerateOverloads(self, declarations, codeunit):
        codes = {}
        for decl in declarations:
            overload = self.OverloadName(decl)
            if overload and overload not in codes:
                code = 'BOOST_PYTHON_FUNCTION_OVERLOADS(%s, %s, %i, %i)' %\
                    (overload, decl.FullName(), decl.minArgs, decl.maxArgs)
                codeunit.Write('declaration', code + '\n')
                codes[overload] = None
        

    def PolicyCode(self):
        policy = self.info.policy
        if policy is not None:
            assert isinstance(policy, Policy)
            return ', %s()' % policy.Code() 
        else:
            return ''

