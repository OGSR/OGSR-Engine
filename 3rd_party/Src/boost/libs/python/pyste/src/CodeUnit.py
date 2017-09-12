from settings import *

#==============================================================================
# RemoveDuplicatedLines
#==============================================================================
def RemoveDuplicatedLines(text):
    includes = text.splitlines()
    d = dict([(include, 0) for include in includes])
    return '\n'.join(d.keys())


#==============================================================================
# CodeUnit
#==============================================================================
class CodeUnit:
    '''
    Represents a cpp file, where other objects can write in one of the     
    predefined sections.
    The avaiable sections are:
        include - The include area of the cpp file
        declaration - The part before the module definition
        module - Inside the BOOST_PYTHON_MODULE macro
    '''
    
    USING_BOOST_NS = True
    
    def __init__(self, modulename):
        self.modulename = modulename
        # define the avaiable sections
        self.code = {}
        self.code['include'] = ''
        self.code['declaration'] = ''
        self.code['module'] = ''


    def Write(self, section, code):
        'write the given code in the section of the code unit'
        if section not in self.code:
            raise RuntimeError, 'Invalid CodeUnit section: %s' % section
        self.code[section] += code
        
        
    def Section(self, section):
        return self.code[section]

    
    def Save(self, filename):
        'Writes this code unit to the filename'
        space = '\n\n'
        fout = file(filename, 'w')
        # includes
        includes = RemoveDuplicatedLines(self.code['include'])
        fout.write('\n' + self._leftEquals('Includes'))        
        fout.write('#include <boost/python.hpp>\n')
        fout.write(includes)
        fout.write(space)
        # using
        if self.USING_BOOST_NS:
            fout.write(self._leftEquals('Using'))
            fout.write('using namespace boost::python;\n\n')
        # declarations
        if self.code['declaration']:
            pyste_namespace = namespaces.pyste[:-2]
            fout.write(self._leftEquals('Declarations'))
            fout.write('namespace %s {\n\n\n' % pyste_namespace)
            fout.write(self.code['declaration']) 
            fout.write('\n\n}// namespace %s\n' % pyste_namespace)
            fout.write(space)
        # module
        fout.write(self._leftEquals('Module'))
        fout.write('BOOST_PYTHON_MODULE(%s)\n{\n' % self.modulename)
        fout.write(self.code['module']) 
        fout.write('}\n')

        
    def _leftEquals(self, s):
        s = '// %s ' % s
        return s + ('='*(80-len(s))) + '\n'
