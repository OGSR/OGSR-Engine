from GCCXMLParser import ParseDeclarations
import tempfile
import shutil
import os
import os.path
import settings

class CppParserError(Exception): pass


class CppParser:
    'Parses a header file and returns a list of declarations'
    
    def __init__(self, includes=None, defines=None):
        'includes and defines ar the directives given to gcc'
        if includes is None:
            includes = []
        if defines is None:
            defines = []
        self.includes = includes
        self.defines = defines


    def _includeparams(self, filename):
        includes = self.includes[:]
        filedir = os.path.dirname(filename)
        if not filedir:
            filedir = '.'
        includes.insert(0, filedir)
        includes = ['-I "%s"' % x for x in includes]
        return ' '.join(includes)


    def _defineparams(self):
        defines = ['-D "%s"' % x for x in self.defines]
        return ' '.join(defines)
    
        
    def FindFileName(self, include):
        if os.path.isfile(include):
            return include
        for path in self.includes:
            filename = os.path.join(path, include)  
            if os.path.isfile(filename):
                return filename
        name = os.path.basename(include)
        raise RuntimeError, 'Header file "%s" not found!' % name
    
            
    def parse(self, include, symbols=None, tail=None):
        '''Parses the given filename, and returns (declaration, header). The
        header returned is normally the same as the given to this method,
        except if tail is not None: in this case, the header is copied to a temp
        filename and the tail code is appended to it before being passed on to gcc.
        This temp filename is then returned.
        '''        
        filename = self.FindFileName(include)
        # copy file to temp folder, if needed
        if tail:
            tempfilename = tempfile.mktemp('.h') 
            infilename = tempfilename
            shutil.copyfile(filename, infilename)
            f = file(infilename, 'a')
            f.write('\n\n'+tail)
            f.close()  
        else:
            infilename = filename
        xmlfile = tempfile.mktemp('.xml')
        try:            
            # get the params
            includes = self._includeparams(filename)
            defines = self._defineparams()
            # call gccxml
            cmd = 'gccxml %s %s %s -fxml=%s' \
               % (includes, defines, infilename, xmlfile)
            if symbols:
                cmd += ' -fxml-start=' + ','.join(symbols)
            status = os.system(cmd)        
            if status != 0 or not os.path.isfile(xmlfile):
                raise CppParserError, 'Error executing gccxml'
            # parse the resulting xml
            declarations = ParseDeclarations(xmlfile)
            # return the declarations             
            return declarations, infilename
        finally:
            if settings.DEBUG and os.path.isfile(xmlfile):
                filename = os.path.basename(include)
                shutil.copy(xmlfile, os.path.splitext(filename)[0] + '.xml')
            # delete the temporary files
            try:
                os.remove(xmlfile)
                if tail:
                    os.remove(tempfilename)
            except OSError: pass
