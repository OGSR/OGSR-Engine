'''
Usage:
    pyste [options] --module=<name> interface-files

where options are:
    -I <path>           add an include path
    -D <symbol>         define symbol
    --no-using          do not declare "using namespace boost";
                        use explicit declarations instead
    --pyste-ns=<name>   set the namespace where new types will be declared;
                        default is "pyste"
'''

import sys
import os
import getopt
import exporters
import CodeUnit
import infos
import exporterutils
import settings
from policies import *
from CppParser import CppParser, CppParserError
from Exporter import Exporter
from FunctionExporter import FunctionExporter
from ClassExporter import ClassExporter
from IncludeExporter import IncludeExporter
from HeaderExporter import HeaderExporter


def GetDefaultIncludes():
    if 'INCLUDE' in os.environ:
        include = os.environ['INCLUDE']
        return include.split(os.pathsep)
    else:
        return []


def ParseArguments():

    def Usage():
        print __doc__
        sys.exit(1)
        
    options, files = getopt.getopt(sys.argv[1:], 'I:D:', ['module=', 'out=', 'no-using', 'pyste-ns=', 'debug'])
    includes = GetDefaultIncludes()
    defines = []
    module = None
    out = None
    for opt, value in options:
        if opt == '-I':
            includes.append(value)
        elif opt == '-D':
            defines.append(value)
        elif opt == '--module':
            module = value
        elif opt == '--out':
            out = value 
        elif opt == '--no-using':
            settings.namespaces.python = 'boost::python::'
            CodeUnit.CodeUnit.USING_BOOST_NS = False
        elif opt == '--pyste-ns':
            settings.namespaces.pyste = value + '::'
        elif opt == '--debug':
            settings.DEBUG = True
        else:
            print 'Unknown option:', opt
            Usage()

    if not files or not module:
        Usage()
    if not out:
        out = module + '.cpp'
    return includes, defines, module, out, files

    
def CreateContext():
    'create the context where a interface file can be executed'
    context = {}
    # infos
    context['Function'] = infos.FunctionInfo
    context['Class'] = infos.ClassInfo
    context['Include'] = infos.IncludeInfo
    context['Template'] = infos.ClassTemplateInfo
    context['Enum'] = infos.EnumInfo
    context['AllFromHeader'] = infos.HeaderInfo
    # functions
    context['rename'] = infos.rename
    context['set_policy'] = infos.set_policy
    context['exclude'] = infos.exclude
    context['set_wrapper'] = infos.set_wrapper
    # policies
    context['return_internal_reference'] = return_internal_reference
    context['with_custodian_and_ward'] = with_custodian_and_ward
    context['return_value_policy'] = return_value_policy
    context['reference_existing_object'] = reference_existing_object
    context['copy_const_reference'] = copy_const_reference
    context['copy_non_const_reference'] = copy_non_const_reference
    context['manage_new_object'] = manage_new_object
    # utils
    context['Wrapper'] = exporterutils.FunctionWrapper
    return context                                        

    
def Main():
    includes, defines, module, out, interfaces = ParseArguments()
    # execute the interface files
    for interface in interfaces:
        context = CreateContext()
        execfile(interface, context)        
    # parse all the C++ code        
    parser = CppParser(includes, defines) 
    exports = exporters.exporters[:]
    for export in exports:
        try:
            export.Parse(parser)
        except CppParserError, e:            
            print '\n'
            print '***', e, ': exitting'
            return 2
    print 
    # sort the exporters by its order
    exports = [(x.Order(), x) for x in exporters.exporters]
    exports.sort()
    exports = [x for _, x in exports]
    # now generate the wrapper code
    codeunit = CodeUnit.CodeUnit(module)
    exported_names = []
    for export in exports:
        export.GenerateCode(codeunit, exported_names)    
        exported_names.append(export.Name())
    codeunit.Save(out)                
    print 'Module %s generated' % module
    return 0


def UsePsyco():
    'Tries to use psyco if it is installed'
    try:
        import psyco
        import elementtree.XMLTreeBuilder as XMLTreeBuilder
        import GCCXMLParser

        psyco.bind(XMLTreeBuilder.fixtext)
        psyco.bind(XMLTreeBuilder.fixname)
        psyco.bind(XMLTreeBuilder.TreeBuilder)
        psyco.bind(GCCXMLParser.GCCXMLParser) 
    except ImportError: pass         

    
if __name__ == '__main__':
    UsePsyco()
    status = Main()
    sys.exit(status)
