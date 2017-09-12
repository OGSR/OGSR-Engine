import os.path
import copy
import exporters 
from ClassExporter import ClassExporter
from FunctionExporter import FunctionExporter
from IncludeExporter import IncludeExporter
from EnumExporter import EnumExporter
from HeaderExporter import HeaderExporter
from exporterutils import FunctionWrapper


#==============================================================================
# DeclarationInfo
#==============================================================================
class DeclarationInfo:
    
    def __init__(self, otherInfo=None):
        self.__infos = {}
        self.__attributes = {}
        if otherInfo is not None:
            self.__infos = copy.deepcopy(otherInfo.__infos)
            self.__attributes = copy.deepcopy(otherInfo.__attributes)


    def __getitem__(self, name):
        'Used to access sub-infos'        
        if name.startswith('__'):
            raise AttributeError
        default = DeclarationInfo()
        default._Attribute('name', name)
        return self.__infos.setdefault(name, default)


    def __getattr__(self, name):
        return self[name]


    def _Attribute(self, name, value=None):
        if value is None:
            # get value
            return self.__attributes.get(name)
        else:
            # set value
            self.__attributes[name] = value


#==============================================================================
# FunctionInfo
#==============================================================================
class FunctionInfo(DeclarationInfo):

    def __init__(self, name, include, tail=None, otherOption=None):        
        DeclarationInfo.__init__(self, otherOption)
        self._Attribute('name', name)
        self._Attribute('include', include)
        # create a FunctionExporter
        exporter = FunctionExporter(InfoWrapper(self), tail)
        exporters.exporters.append(exporter)


#==============================================================================
# ClassInfo
#==============================================================================
class ClassInfo(DeclarationInfo):

    def __init__(self, name, include, tail=None, otherInfo=None):
        DeclarationInfo.__init__(self, otherInfo)
        self._Attribute('name', name)
        self._Attribute('include', include)
        # create a ClassExporter
        exporter = ClassExporter(InfoWrapper(self), tail)
        exporters.exporters.append(exporter) 
        

#==============================================================================
# IncludeInfo
#==============================================================================
class IncludeInfo(DeclarationInfo):

    def __init__(self, include):
        DeclarationInfo.__init__(self)
        self._Attribute('include', include)
        exporter = IncludeExporter(InfoWrapper(self))
        exporters.exporters.append(exporter)        

    
#==============================================================================
# templates
#==============================================================================
def GenerateName(name, type_list):
    name = name.replace('::', '_')
    names = [name] + type_list
    return '_'.join(names) 

    
class ClassTemplateInfo(DeclarationInfo):

    def __init__(self, name, include):
        DeclarationInfo.__init__(self)
        self._Attribute('name', name)
        self._Attribute('include', include)


    def Instantiate(self, type_list, rename=None):
        if not rename:
            rename = GenerateName(self._Attribute('name'), type_list)
        # generate code to instantiate the template
        types = ', '.join(type_list)
        tail = 'typedef %s< %s > %s;\n' % (self._Attribute('name'), types, rename)
        tail += 'void __instantiate_%s()\n' % rename
        tail += '{ sizeof(%s); }\n\n' % rename
        # create a ClassInfo
        class_ = ClassInfo(rename, self._Attribute('include'), tail, self)
        return class_


    def __call__(self, types, rename=None):
        if isinstance(types, str):
            types = types.split() 
        return self.Instantiate(types, rename)
        
#==============================================================================
# EnumInfo
#==============================================================================
class EnumInfo(DeclarationInfo):
    
    def __init__(self, name, include):
        DeclarationInfo.__init__(self)
        self._Attribute('name', name)
        self._Attribute('include', include)
        exporter = EnumExporter(InfoWrapper(self))
        exporters.exporters.append(exporter)


#==============================================================================
# HeaderInfo
#==============================================================================
class HeaderInfo(DeclarationInfo):

    def __init__(self, include):
        DeclarationInfo.__init__(self)
        self._Attribute('include', include)
        exporter = HeaderExporter(InfoWrapper(self))
        exporters.exporters.append(exporter)


#==============================================================================
# InfoWrapper
#==============================================================================
class InfoWrapper:
    'Provides a nicer interface for a info'

    def __init__(self, info):
        self.__dict__['_info'] = info # so __setattr__ is not called

    def __getitem__(self, name):
        return InfoWrapper(self._info[name])

    def __getattr__(self, name):
        return self._info._Attribute(name)

    def __setattr__(self, name, value):
        self._info._Attribute(name, value)


#==============================================================================
# Functions
#==============================================================================
def exclude(option):
    option._Attribute('exclude', True)

def set_policy(option, policy):
    option._Attribute('policy', policy)

def rename(option, name):
    option._Attribute('rename', name)

def set_wrapper(option, wrapper):
    if isinstance(wrapper, str):
        wrapper = FunctionWrapper(wrapper)
    option._Attribute('wrapper', wrapper)

def instantiate(template, types, rename=None):
    if isinstance(types, str):
        types = types.split()
    return template.Instantiate(types, rename)
    
